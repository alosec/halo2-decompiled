#include "unknown_11c920.h"
#include "globals.h"

// @flags /O2 /Gr

void function_246c60(long arg_1, void *arg_0);

struct s_212300
{
    byte field_0[8];
    byte *field_8;
    long field_c;
    byte *field_10;
};

struct s_2122e2
{
    long field_0;
    long field_4;
    s_212300 *field_8;
};

struct s_2122b0
{
    byte field_0[0x1c];
    long field_1c;
    s_2122e2 *field_20;
};

// @retail 0x2122b0
void function_2122b0(long arg_0)
{
    s_2122b0 *local_0 = (s_2122b0 *)g_4e3b44[arg_0 & 0xffff].bytes;
    for (long local_1 = 0; local_1 < local_0->field_1c; local_1++)
    {
        s_2122e2 *local_2 = &local_0->field_20[local_1];
        for (long local_3 = 0; local_3 < local_2->field_4; local_3++)
        {
            s_212300 *local_4 = &local_2->field_8[local_3];
            for (long local_5 = 0; local_5 < local_4->field_c; local_5++)
            {
                byte *blocks = local_4->field_8;

                function_246c60((long)(local_4->field_10 + local_5 * 0x70), blocks + local_5 * 0x44 + 4);
            }
        }
    }
}
