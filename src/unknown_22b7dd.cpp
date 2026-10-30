// @flags /O1 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "globals.h"
#include "unknown_03bcb0.h"

long function_13a690(long mode);
bool function_22acb4(long local_player_index);

struct s_bitmap_widget_22b7dd
{
    byte unknown00[0x1c];
    word mode;
    byte unknown1e[6];
    long bitmap_tag;
    byte unknown28[4];
    long reference0;
    byte unknown30[4];
    long reference1;
    byte unknown38[4];
    long reference2;
    char sequences[3];
};

struct s_bitmap_sprite_22b7dd
{
    short bitmap_index;
    byte unknown02[0x1e];
};

struct s_bitmap_sequence_22b7dd
{
    byte unknown00[0x20];
    short first_bitmap;
    short bitmap_count;
    byte unknown24[0x10];
    long sprite_count;
    s_bitmap_sprite_22b7dd *sprites;
};

struct s_bitmap_sequence_block_22b7dd
{
    long count;
    s_bitmap_sequence_22b7dd *elements;
};

struct s_bitmap_tag_22b7dd
{
    byte unknown00[0x3c];
    s_bitmap_sequence_block_22b7dd sequences;
    long bitmap_count;
    byte *bitmaps;
};

/* predicts the texture of the bitmap a widget shows in its current mode;
   false when the texture is not ready */
// score 38 (was 38): the register that holds zero is EBX where retail uses EDX, so the index and the
// count are kept in the other registers. Our frame index takes the zero register (its 0 and the
// shared zero merge, so the merged value must survive 0x22acb4's call and moves to a saved
// register); retail keeps the index in ESI (`xor esi, esi` in each arm, `inc esi`), the sequence in
// EBX and the zero in EDX, as the matched 0x22ae21 does (there the arms' extra live values keep
// the two apart). Tried (plain checks): the index declared in each arm 49, with ++ 49, with = 1
// (0x22ae21's form) 49, with `+= 1` and the index-first sum 49; a shared frame-offset helper 83 (out
// of line), forceinline 49; the index as `cond ? 1 : 0` or as the && value 80 (two constant arms);
// a bool frame flag 100 (271 bytes: zero in EDX, but the bool takes BL with movzx at each use);
// per-branch bools set in the test 105 (aligned 18: the bool in BL where retail's index is ESI, the
// sequence in ESI where retail uses EBX); bools initialised from the && 84; short index 73; dword
// or unsigned index 38 (same code); index declared at the top 39; index declared at function
// scope and zeroed in each arm 49; index declared before the sequence pointer 38; the frame test
// as nested ifs 38; no sequence-block pointer local 60; a byte/long result 47/96; short/char
// sequence selector 81/74; result declared first 62; one mode dispatch 91; a structured body
// without gotos (NONE reference default, NULL bitmap test) 84 (269 bytes).
// shared zero merge, so the zero moves to a saved register); retail keeps index in ESI (xor esi, esi;
// inc esi) and zero in EDX, as the matched 0x22ae21 does. Round 93 (plain checks): shared frame-offset
// helper 83 (out of line), forceinline 49; index as the && value 80 (two constant arms); per-branch
// index with ++ 49, with = 1 (0x22ae21's form) 49; short index 73; dword index 38; index declared at
// the top 39; a bool frame choice 100 (271 bytes, but zero in EDX and aligned 19: only the bool's
// byte register and movzx differ); per-branch bools set in the test 105 (aligned 18: every
// instruction retail's but the bool sits in BL with movzx where retail's index is ESI, and the
// sequence in ESI where retail uses EBX); bools initialised from the && 84 (constant arms);
// no sequence-block pointer local 60. Round 96: the bitmap pointer computed inside each branch
// separates them (EDX zero, ESI index, aligned 8) but sinks `push ebx` and cross-jumps the two
// multiply tails the other way: 67 (per-branch index and pointer; also with per-branch
// bitmap_index locals or the bitmap branch as the last arm); `index = call ? 1 : 0` 69.
// @retail 0x22b7dd
bool function_22b7dd(byte const *arg_1, long arg_2)
{
    s_bitmap_widget_22b7dd const *widget = (s_bitmap_widget_22b7dd const *)arg_1;
    long mode = function_13a690(widget->mode);
    bool result = true;
    long reference;
    switch (mode)
    {
    case 0:
        reference = widget->reference0;
        break;
    case 1:
        reference = widget->reference1;
        break;
    case 2:
        reference = widget->reference2;
        break;
    default:
        goto done;
    }
    if (reference != NONE && widget->bitmap_tag != NONE)
    {
        s_bitmap_tag_22b7dd *tag = (s_bitmap_tag_22b7dd *)g_4e3b44[widget->bitmap_tag & 0xffff].bytes;
        long sequence_index = 0;
        switch (mode)
        {
        case 0:
            sequence_index = widget->sequences[0];
            break;
        case 1:
            sequence_index = widget->sequences[1];
            break;
        case 2:
            sequence_index = widget->sequences[2];
            break;
        }
        if (sequence_index >= 0)
        {
            byte *bitmap;
            s_bitmap_sequence_block_22b7dd const *sequences = &tag->sequences;
            if (sequence_index < sequences->count)
            {
                s_bitmap_sequence_22b7dd *sequence = &sequences->elements[sequence_index];
                long bitmap_index;
                long index = 0;
                if (sequence->sprite_count > 0)
                {
                    if (sequence->sprite_count > 1 && function_22acb4(arg_2))
                        index++;
                    bitmap_index = sequence->sprites[index].bitmap_index;
                }
                else if (sequence->bitmap_count > 0)
                {
                    if (sequence->bitmap_count > 1 && function_22acb4(arg_2))
                        index++;
                    bitmap_index = sequence->first_bitmap + index;
                }
                else
                {
                    goto done;
                }
                bitmap = tag->bitmaps + bitmap_index * 0x74;
            }
            else if (sequence_index == 0 && tag->sequences.count == 0 && tag->bitmap_count > 0)
            {
                bitmap = tag->bitmaps;
            }
            else
            {
                goto done;
            }
            if (!function_3bcb0((s_bitmap_data *)bitmap))
                result = false;
        }
    }
done:
    return result;
}
