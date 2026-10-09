// Native scalar split of the recovered record-list payload update at retail
// 0x2962e0.  The matching Xbox definition remains in unknown_2963f0.cpp;
// this source avoids unrelated x87 inline assembly in that translation unit.

#include "data_array.h"
#include <string.h>

struct s_296330_entry
{
    long unknown00;
    long key;
    long next;
};

void function_2962e0(s_record_pool *pool, long *head, long key, long size, void const *value)
{
    long index = *head;
    while (index != NONE)
    {
        s_296330_entry *entry = (s_296330_entry *)(pool->data + (index & 0xffff) * pool->size);
        if (entry->key == key)
        {
            memcpy(entry + 1, value, size);
            break;
        }
        index = entry->next;
    }
}
