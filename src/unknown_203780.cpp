#include "unknown_11c920.h"
#include "globals.h"
#include "object_markers.h"
#include "unknown_11cc90.h"

// @flags /O2 /arch:SSE /Gr

struct s_203780
{
    long field_0;
    point3f field_4;
    short field_10;
    byte field_12[2];
    vector2f field_14;
    byte field_1c[4];
    short field_20;
    byte field_22[0x64 - 0x22];
};

vector2f *function_11df30(vector2f *arg_0, vector3f const *arg_1);
void function_bf380(void);
long __stdcall function_1e0850(long arg_0, long arg_1, s_203780 const *arg_2, bool arg_3);

// score 70 (was 70): retail keeps NONE in esi (or esi,-1 at entry; four
// compares and two exits use it) with arg_0 in ebx and ebp/edi pushed past
// the first guard; ours keeps arg_0 in esi, NONE in eax for the first exit
// only (immediates elsewhere), pushes all four at entry, and shares one fail
// exit (345 vs 369 bytes). No source form enregisters the NONE constant: VC
// propagates it per web. Early returns 109 (shrink-wrap appears but NONE
// stays immediate); only the arg_1 guard early 109; local_8 initialized at
// declaration 109; the marker && split into nested ifs 75; marker index as a
// ternary 70; marker-found through a bool 70; result assigned inside the test
// 70; store through an address local 70; global optimization off 210
// (diagnostic). Round 78: single exit with every guard nested 115 (arg_1 then
// arrives in dx), with `if (local_0 == NONE) return NONE;` kept 100, plus an
// early `return NONE` for arg_1 123; the squad-index product named first 70;
// the actor store under `if (local_0 != NONE)` 71. Lane 11 r12 (aligned
// counts, base 44): the cell pointer as a typed element address or index-first
// sum 44 (identical code); the guards nested to one exit with the call result
// in its own local, or `local_0 = call; if (local_0 != NONE) store` 102/97
// (arg_1 moves to dx); the call result in its own local copied to local_0 on
// success, early returns kept 93 (NONE in ebx, local_0 spilled, frame 0xdc).
// Round 96: local_10 as a ternary 70 (identical); local_5 as a short 70
// (identical); `if (arg_1 == NONE) return local_0;` as a guard 109 (the exit
// is duplicated at the top, NONE immediate); the call result in its own local
// the actor store under `if (local_0 != NONE)` 71.
// Lane 11 r12 (aligned counts, base 44): the cell pointer as a typed element
// address or index-first sum 44 (identical code); the guards nested to one exit
// with the call result in its own local, or `local_0 = call; if (local_0 !=
// NONE) store` 102/97 (arg_1 moves to dx); the call result in its own local
// copied to local_0 on success, early returns kept 93 (NONE in ebx, local_0
// spilled, frame 0xdc).
// Round 16: the chain starts at the cell pointer: VC loads the cell table pointer while arg_1
// is still live in cx, so that temporary takes ebx (retail: arg_1 sign-extended first, temporary
// in ecx, arg_0 in ebx, a free esi for NONE). Index-first sum 70; typed `+ arg_1` 70; the squad
// lookup written into the cell address 69 (table base in edi as retail, temporary still ebx),
// also index-first, declared before the squad pointer, or as `&cells[arg_1]` 69 each; the cell
// offset in its own statement first 70 (VC folds it back).
// spilled, frame 0xdc). Round 96: local_10 as a ternary 70 (identical); local_5 as
// a short 70 (identical); `if (arg_1 == NONE) return local_0;` as a guard 109 (the
// exit is duplicated at the top, NONE immediate); the call result in its own local
// returned directly 108 (370 bytes); that local copied to local_0 inside an
// `!= NONE` test 136. Round 21: retail's -1 is a constant register (esi free once arg_0 takes ebx
// and the cell-table temporary ecx); ours has no free callee-saved register for it, so all NONE exits
// merge into one `or eax, -1`. A single exit keeps the result in eax in the all-stack first pass, so
// arg_1 is loaded into dx (early returns load it into cx, as retail). Inline squad and cell getters
// 69/70; single exit through `goto done` 100, nested 100; `local_8 = local_4` default 109; the marker
// lookup nested under `arg_0 != NONE` 109; the marker declared first 70; literal `return NONE` guards 70;
// the cell index in a long local first 70.
// `!= NONE` test 136. Round 82: every guard nested with the call-fail
// `return NONE` kept 100 (local_0 in eax, arg_1 in dx); the call result in its
// own local copied to local_0 on success 70 (identical code now); local_8
// initialized to the cell and overwritten by the marker branch 109; the index
// guards returning NONE 70 (identical); the arg_1 guard early with the rest
// nested 123.
// `!= NONE` test 136. Round 120 (real checks of an 18-form grid: arg_1 guard
// nested / `return local_0` / `return NONE`, x range guards the same three
// ways, x the call tail `== NONE return NONE` or `!= NONE store`): every early
// arg_1 guard 109 (366 bytes, arg_0 in esi), with nested range guards 123/138;
// nested arg_1 guard with early range returns 70/71; all nested 100/115
// (arg_0 in ebx and retail's schedule, but local_0 in eax, arg_1 in dx); the
// call result in its own local with `return NONE` at the end 134; NONE
// assigned in each failing arm of an if/else chain to one exit 109/134.
// @retail 0x203780
long function_203780(long arg_0, short arg_1, short arg_2, bool arg_3)
{
    long local_0 = NONE;
    if (arg_1 != NONE)
    {
        byte *local_3 = *(byte **)((byte *)g_4e0350 + 0x164) + (word)arg_2 * 0x74;
        s_203780 *local_4 = (s_203780 *)(*(byte **)(local_3 + 0x4c) + arg_1 * 0x64);
        word local_5 = local_4->field_20;
        short local_10;
        if ((short)local_5 != NONE)
            local_10 = (short)local_5;
        else
            local_10 = *(short *)(local_3 + 0x36);
        if (local_10 < 0 || local_10 >= *(long *)((byte *)g_4e0350 + 0x178))
            return local_0;
        long *local_6 = (long *)(*(byte **)((byte *)g_4e0350 + 0x17c) + local_10 * 8);
        if (local_6[1] == NONE)
            return local_0;
        s_203780 local_7;
        s_203780 const *local_8;
        s_object_marker local_9;
        if (arg_0 != NONE && function_b8d30(arg_0, 0x700061d, &local_9, 1, false) > 0)
        {
            local_7 = *local_4;
            local_7.field_4 = local_9.matrix.position;
            local_7.field_10 = NONE;
            function_11df30(&local_7.field_14, &local_9.matrix.forward);
            local_8 = &local_7;
        }
        else
            local_8 = local_4;
        if (arg_3)
            function_bf380();
        local_0 = function_1e0850(local_6[1], arg_2, local_8, true);
        if (local_0 == NONE)
            return NONE;
        *(long *)(g_4f55f0->data + (local_0 & 0xffff) * 0x888 + 0x38) = local_4->field_0;
    }
    return local_0;
}
