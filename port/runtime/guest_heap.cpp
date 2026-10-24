/* The guest heap for whole images: the C++ allocation operators and the C
 * allocation calls. Small blocks come from per-size free lists carved out of
 * 1 MB chunks of host memory; large blocks are host allocations of their own.
 * Every block carries a 16-byte header so frees and reallocations know its
 * size. The game allocates from several threads, so a spinlock guards it. */
#include "h2/host_abi.h"
#include <string.h>

namespace {

constexpr unsigned int k_header = 16;
constexpr unsigned int k_chunk = 1u << 20;
constexpr unsigned int k_class_count = 9; /* 16 .. 4096 bytes */
constexpr unsigned int k_large = 0xffffffffu;

struct s_block_header
{
    unsigned int size_class;
    unsigned int size;
    unsigned int reserved[2];
};
static_assert(sizeof(s_block_header) == k_header, "heap header size");

struct s_free_block
{
    s_free_block *next;
};

s_free_block *g_free_lists[k_class_count];
unsigned char *g_chunk_cursor;
unsigned char *g_chunk_end;
volatile int g_heap_lock;

void lock()
{
    while (__atomic_exchange_n(&g_heap_lock, 1, __ATOMIC_ACQUIRE))
        while (__atomic_load_n(&g_heap_lock, __ATOMIC_RELAXED))
            ;
}

void unlock()
{
    __atomic_store_n(&g_heap_lock, 0, __ATOMIC_RELEASE);
}

unsigned int class_size(unsigned int size_class)
{
    return 16u << size_class;
}

unsigned int class_for(unsigned int size)
{
    for (unsigned int size_class = 0; size_class < k_class_count; ++size_class)
        if (size + k_header <= class_size(size_class))
            return size_class;
    return k_large;
}

void *allocate(unsigned int size)
{
    const unsigned int size_class = class_for(size);
    s_block_header *header;
    if (size_class == k_large)
    {
        header = reinterpret_cast<s_block_header *>(host_h2_xbox_memory_alloc(size + k_header, 0, 0, 0));
        if (!header)
            return nullptr;
    }
    else
    {
        lock();
        if (s_free_block *block = g_free_lists[size_class])
        {
            g_free_lists[size_class] = block->next;
            header = reinterpret_cast<s_block_header *>(block);
        }
        else
        {
            const unsigned int bytes = class_size(size_class);
            if (!g_chunk_cursor || g_chunk_cursor + bytes > g_chunk_end)
            {
                g_chunk_cursor = reinterpret_cast<unsigned char *>(host_h2_xbox_memory_alloc(k_chunk, 0, 0, 0));
                g_chunk_end = g_chunk_cursor ? g_chunk_cursor + k_chunk : nullptr;
            }
            if (!g_chunk_cursor)
            {
                unlock();
                return nullptr;
            }
            header = reinterpret_cast<s_block_header *>(g_chunk_cursor);
            g_chunk_cursor += bytes;
        }
        unlock();
    }
    header->size_class = size_class;
    header->size = size;
    return reinterpret_cast<unsigned char *>(header) + k_header;
}

s_block_header *header_of(void *memory)
{
    return reinterpret_cast<s_block_header *>(static_cast<unsigned char *>(memory) - k_header);
}

void release(void *memory)
{
    if (!memory)
        return;
    s_block_header *header = header_of(memory);
    if (header->size_class == k_large)
    {
        host_h2_xbox_memory_free(reinterpret_cast<unsigned int>(header));
        return;
    }
    lock();
    s_free_block *block = reinterpret_cast<s_free_block *>(header);
    block->next = g_free_lists[header->size_class];
    g_free_lists[header->size_class] = block;
    unlock();
}

} // namespace

void *operator new(unsigned int size)
{
    return allocate(size);
}

void *operator new[](unsigned int size)
{
    return allocate(size);
}

void operator delete(void *memory)
{
    release(memory);
}

void operator delete[](void *memory)
{
    release(memory);
}

extern "C" void *malloc(unsigned int size)
{
    return allocate(size);
}

extern "C" void *calloc(unsigned int count, unsigned int size)
{
    const unsigned long long total = (unsigned long long)count * size;
    if (total > 0x7fffffffull)
        return nullptr;
    void *memory = allocate((unsigned int)total);
    if (memory)
        memset(memory, 0, (unsigned int)total);
    return memory;
}

extern "C" void *realloc(void *memory, unsigned int size)
{
    if (!memory)
        return allocate(size);
    if (!size)
    {
        release(memory);
        return nullptr;
    }
    const unsigned int old_size = header_of(memory)->size;
    void *grown = allocate(size);
    if (grown)
    {
        memcpy(grown, memory, old_size < size ? old_size : size);
        release(memory);
    }
    return grown;
}

extern "C" void free(void *memory)
{
    release(memory);
}
