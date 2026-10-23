// @flags /O2 /arch:SSE /Gr
#include "object_iterator.h"
#include "globals.h"

void __stdcall function_b8540(long object_index);

struct s_object_iterator_f8190
{
    s_object *datum;
    s_type_f1af8e iterator;
};

// Runs 0xb8540 on every projectile unless the game is in mode 4.
// @retail 0xf8190
void function_f8190(void)
{
    if (g_4e6948->mode != 4)
    {
        s_object_iterator_f8190 iterator;
        function_bae80(&iterator.iterator, 0x20, 0);
        while (iterator.datum = function_baeb0(&iterator.iterator))
            function_b8540(iterator.iterator.object_index);
    }
}
