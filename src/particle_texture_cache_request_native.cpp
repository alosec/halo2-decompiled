// Source-local native reconstruction of texture-cache request lookup/wait policy.
// The Xbox body remains in unknown_12c0d0.cpp.

#include "unknown_11c920.h"
#include "data_array.h"
#include "physical_memory.h"
#include "async.h"
#include <string.h>

struct D3DTexture
{
    dword Common;
    dword Data;
    dword Lock;
    dword Format;
    dword Size;
};
static_assert(sizeof(D3DTexture) == 0x14, "D3DTexture");

struct s_type_7ba8e9;

struct s_bitmap_data
{
    byte unknown00[0x0a];
    short type;
    byte unknown0c[2];
    word flags;
    byte unknown10[6];
    byte level_bias;
    byte cache_format;
    byte unknown18[4];
    long data_offsets[3];
    long block_indices[3];
    long level_sizes[3];
    long unknown40[4];
    D3DTexture *texture;
    long unknown54;
    real minimum_scale;
    long hardware_common;
    dword hardware_data;
    long hardware_lock;
    dword hardware_format;
    dword hardware_size;
    long last_frame;
};
static_assert(sizeof(s_bitmap_data) == 0x74, "s_bitmap_data");

struct s_texture_cache_entry
{
    byte unknown00[2];
    byte flags;
    bool resident;
    long pending;
    byte unknown08[4];
    long hardware_format;
    s_bitmap_data *bitmap;
    byte resource[0x0c];
    byte unknown20[8];
};
static_assert(sizeof(s_texture_cache_entry) == 0x28, "s_texture_cache_entry");

extern s_record_pool *g_4e6454;
extern s_physical_object *g_4e6464;
extern dword g_4e6460;
extern long g_4e6484;

struct s_texture_cache_file
{
    HANDLE handle;
    byte unknown04[0x800];
};
static_assert(sizeof(s_texture_cache_file) == 0x804, "s_texture_cache_file");

extern s_texture_cache_file g_557c90[6];
extern long g_55aca8;
extern dword g_46e488;

// @retail 0x1a0e70
long __stdcall function_1a0e70(s_async_task *task)
{
    s_read_position_task *read = &task->read_position;
    LONG high = 0;
    DWORD bytes_read = 0;
    dword remaining = read->size - read->bytes_read;
    dword size = remaining > g_46e488 ? g_46e488 : remaining;
    if (SetFilePointer(read->file.handle, read->offset, &high, FILE_BEGIN) !=
        INVALID_SET_FILE_POINTER)
    {
        ReadFile(read->file.handle, (byte *)read->buffer + read->bytes_read, size,
            &bytes_read, NULL);
        read->bytes_read += bytes_read;
        read->offset += bytes_read;
        if (read->bytes_read_out)
            *read->bytes_read_out = read->bytes_read;
    }
    if (read->size == read->bytes_read || bytes_read < size)
        return 1;
    return 0;
}

// @retail 0x1208b0
s_job_node *function_1208b0(void)
{
    s_job_node *node = NULL;
    do
    {
        WaitForSingleObject(async_globals.free_list_mutex, INFINITE);
        if (async_globals.free_list)
        {
            node = async_globals.free_list;
            async_globals.free_list = node->next;
            node->next = NULL;
        }
        ReleaseMutex(async_globals.free_list_mutex);
        if (!node)
            SwitchToThread();
    }
    while (!node);
    return node;
}

// @retail 0x1a1720
bool async_task_should_run_before(long priority, long other_priority,
    s_async_task const *task, s_async_task const *other_task,
    s_async_insert_state *state, async_work_callback callback,
    async_work_callback other_callback)
{
    bool result = false;
    if (priority > other_priority)
        result = true;
    else if (priority < other_priority)
        result = false;
    else if (callback == function_1a0e70 && other_callback != function_1a0e70)
        result = true;
    else if (callback != function_1a0e70 && other_callback == function_1a0e70)
        result = false;
    else if (callback == function_1a0e70 && other_callback == function_1a0e70)
    {
        if (!state->valid)
            result = false;
        else if ((dword)task->read_position.file.handle - state->file <
                 (dword)other_task->read_position.file.handle - state->file)
            result = true;
        else if ((dword)task->read_position.file.handle - state->file >
                 (dword)other_task->read_position.file.handle - state->file)
            result = false;
        else
            result = task->read_position.offset - state->offset <
                     other_task->read_position.offset - state->offset;
    }
    if (other_callback == function_1a0e70)
    {
        state->file = (dword)other_task->read_position.file.handle;
        state->offset = other_task->read_position.offset;
        state->valid = true;
    }
    return result;
}

// @retail 0x120900
void function_120900(s_job_node *node)
{
    s_async_insert_state state;
    memset(&state, 0, sizeof(state));
    s_job_node *previous = async_globals.work_list;
    if (!previous)
    {
        async_globals.work_list = node;
        node->next = previous;
        return;
    }
    long priority = node->priority;
    if (priority > previous->priority)
    {
        node->next = previous;
        async_globals.work_list = node;
        return;
    }
    async_work_callback callback = node->callback;
    while (previous->next)
    {
        s_job_node *next = previous->next;
        if (async_task_should_run_before(priority, next->priority, &node->task,
            &next->task, &state, callback, next->callback))
        {
            node->next = previous->next;
            previous->next = node;
            return;
        }
        previous = next;
    }
    previous->next = node;
    node->next = NULL;
}

// @retail 0x1209c0
long async_task_queue(s_job_node *node)
{
    WaitForSingleObject(async_globals.work_list_mutex, INFINITE);
    function_120900(node);
    node->state = node - async_globals.nodes;
    node->state = (async_globals.task_id_counter << 8) | node->state;
    async_globals.task_id_counter++;
    long task_id = node->state;
    ReleaseMutex(async_globals.work_list_mutex);
    LONG previous_count = 0;
    ReleaseSemaphore(async_globals.work_semaphore, 1, &previous_count);
    return task_id;
}

// @retail 0x120ba0
long function_120ba0(long priority, s_async_task *task, long category,
    async_work_callback callback, bool volatile *done)
{
    async_globals.tasks_added++;
    *done = false;
    s_job_node *node = function_1208b0();
    node->task = *task;
    node->done = done;
    node->priority = priority;
    node->callback = callback;
    node->category = category;
    return async_task_queue(node);
}

// @retail 0x1a0f10
long function_1a0f10(s_file_handle file, void *buffer, dword size, dword offset,
    long category, long priority, dword *bytes_read, bool volatile *done)
{
    s_async_task task;
    memset(&task, 0, sizeof(task));
    task.read_position.file = file;
    task.read_position.buffer = buffer;
    task.read_position.size = size;
    task.read_position.offset = offset;
    task.read_position.bytes_read_out = bytes_read;
    return function_120ba0(priority, &task, category, function_1a0e70, done);
}

// @retail 0x213760
long function_213760(dword location, long size, void *buffer, dword *bytes_read,
    bool *done, long type, long priority)
{
    long file_index = g_55aca8;
    long selector = location >> 30;
    if (selector == 1)
        file_index = 2;
    else if (selector == 2)
        file_index = 0;
    else if (selector == 3)
        file_index = 1;
    s_file_handle file = { g_557c90[file_index].handle };
    return function_1a0f10(file, buffer, size, location & 0x3fffffff, type,
        priority, bytes_read, done);
}

// @retail 0x120ce0
bool function_120ce0(long task, long priority)
{
    bool result = false;
    WaitForSingleObject(async_globals.work_list_mutex, INFINITE);
    s_job_node *node = &async_globals.nodes[task & 0xff];
    if (node->state == task)
    {
        if (async_globals.work_list == node)
            async_globals.work_list = node->next;
        else
        {
            s_job_node *previous = async_globals.work_list;
            while (previous->next != node)
                previous = previous->next;
            previous->next = node->next;
        }
        node->priority = priority;
        function_120900(node);
        result = true;
    }
    ReleaseMutex(async_globals.work_list_mutex);
    return result;
}

void function_120d50(volatile bool *complete, bool wait);

static __forceinline s_texture_cache_entry *texture_cache_entry_get_native(long datum_index)
{
    return (s_texture_cache_entry *)g_4e6454->data + (datum_index & 0xffff);
}

struct s_bitmap_texture_view
{
    dword signature;
    short width;
    short height;
    char depth;
    byte unknown09;
    short type;
    short format;
    word flags;
    byte unknown10[4];
    short mipmap_count;
    byte unknown16[0x40 - 0x16];
    dword level_addresses[3];
};
static_assert(sizeof(s_bitmap_texture_view) == 0x4c, "s_bitmap_texture_view");

extern long g_450768[8][24];
long function_1365a0(s_type_7ba8e9 const *bitmap, long level);
short bitmap_get_mipmap_count(short width, short height, short depth,
    short format, bool linear, short maximum_levels);
long log2_floor(dword value);

// @retail 0x12d6d0
void __stdcall function_12d6d0(s_bitmap_texture_view const *bitmap, long level,
    D3DTexture *texture)
{
    texture->Data = 0;
    texture->Lock = 0;
    texture->Common = 0x40001;
    if (bitmap->flags & 0x10)
    {
        long format = g_450768[1][bitmap->format];
        if ((bitmap->flags & 0x20) && (bitmap->format == 10 || bitmap->format == 11))
            format = 0x36;
        texture->Format = (format << 8) | 0x10029;
        long pitch = function_1365a0((s_type_7ba8e9 const *)bitmap, 0);
        texture->Size = ((((pitch - 1) / 64) << 12 | (bitmap->height - 1)) << 12) |
            (bitmap->width - 1);
    }
    else
    {
        short reduction = (short)level;
        long width = bitmap->width >> reduction;
        long height = bitmap->height >> reduction;
        long depth = bitmap->depth >> reduction;
        long levels = bitmap_get_mipmap_count((short)width, (short)height, (short)depth,
            bitmap->format, false, bitmap->mipmap_count - reduction);
        long format = g_450768[0][bitmap->format];
        if ((bitmap->flags & 0x20) && (bitmap->format == 10 || bitmap->format == 11))
            format = 0x33;
        short width_bits = (short)log2_floor(width);
        short height_bits = (short)log2_floor(height);
        short depth_bits = (short)log2_floor(depth);
        texture->Format = (((((((depth_bits << 4) | height_bits) << 4) | width_bits) << 12 |
            format) << 4 | (bitmap->type == 1 ? 3 : 2)) << 4) |
            (bitmap->type == 2 ? 4 : 0) | ((levels + 1) << 16) | 9;
        texture->Size = 0;
    }
    texture->Data = bitmap->level_addresses[level] & 0x0fffffff;
}

// @retail 0x12d5a0
bool function_12d5a0(s_bitmap_data *bitmap, long level, bool blocking,
    bool alternate, long priority)
{
    long block_index = function_13d370(g_4e6464, bitmap->level_sizes[level],
        (blocking || alternate) ? 2 : 5);
    if (blocking && !alternate)
        priority = 7;
    if (block_index == NONE)
        return false;

    long address =
        (((s_physical_block *)g_4e6464->blocks->data)[block_index & 0xffff].offset <<
            g_4e6464->page_shift) +
        g_4e6460;
    datum_new_at_index_with_salt(g_4e6454, block_index);
    s_texture_cache_entry *entry = texture_cache_entry_get_native(block_index);
    entry->resident = false;
    entry->flags &= ~1;
    entry->pending = level;
    entry->bitmap = bitmap;
    memset(&entry->resource, 0, sizeof(D3DTexture));
    bitmap->block_indices[level] = block_index;
    bitmap->unknown40[level] = address;
    if (!level)
        bitmap->unknown54 = address;
    function_12d6d0((s_bitmap_texture_view *)bitmap, level,
        (D3DTexture *)&entry->resource);

    long size = bitmap->level_sizes[level];
    if (size & 0x1ff)
        size = (size | 0x1ff) + 1;
    entry->hardware_format = function_213760(bitmap->data_offsets[level], size,
        (void *)address, NULL, &entry->resident, 3, priority);
    *(long *)entry->unknown08 = priority;
    g_4e6484++;
    return true;
}

// @retail 0x12c990
D3DTexture *function_12c990(s_bitmap_data *bitmap, long level, bool request,
    bool blocking, bool alternate, long priority, bool *waited, bool *failed)
{
    D3DTexture *texture = NULL;
    bool throttled = async_globals.tasks_added > 25 || g_4e6484 >= 10;
    if (request && (blocking || priority > 2 || !throttled))
    {
        if (bitmap->data_offsets[level] != NONE && bitmap->level_sizes[level] &&
            (!level || bitmap->level_sizes[level] > 0x400))
        {
            if (bitmap->block_indices[level] == NONE)
            {
                if (!function_12d5a0(bitmap, level, blocking, alternate, priority))
                    *failed = true;
            }
            if (bitmap->block_indices[level] != NONE)
            {
                long block_index = bitmap->block_indices[level];
                s_texture_cache_entry *entry = texture_cache_entry_get_native(block_index);
                ((s_physical_block *)g_4e6464->blocks->data)[block_index & 0xffff].time =
                    g_4e6464->time;
                if (!*(volatile bool *)&entry->resident)
                {
                    if (blocking)
                    {
                        *waited = true;
                        function_120ce0(entry->hardware_format, 7);
                        function_120d50(&entry->resident, true);
                    }
                    else if (priority > *(long *)entry->unknown08)
                    {
                        *(long *)entry->unknown08 = priority;
                        function_120ce0(entry->hardware_format, priority);
                    }
                }
                if (*(volatile bool *)&entry->resident)
                    texture = (D3DTexture *)&entry->resource;
            }
        }
    }
    else if (bitmap->block_indices[level] != NONE)
    {
        long block_index = bitmap->block_indices[level];
        s_texture_cache_entry *entry = texture_cache_entry_get_native(block_index);
        ((s_physical_block *)g_4e6464->blocks->data)[block_index & 0xffff].time =
            g_4e6464->time;
        if (*(volatile bool *)&entry->resident)
            texture = (D3DTexture *)&entry->resource;
    }
    return texture;
}
