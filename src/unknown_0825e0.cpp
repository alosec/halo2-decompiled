// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
struct s_weapon_activity_result;
struct s_player_action;
struct s_object_relevance_result;
struct s_object_relevance_source;
void player_action_initialize(s_player_action *action);
void function_82b30(const s_object_relevance_result *source, s_object_relevance_source *result);

#pragma inline_depth(0)
static __forceinline void function_825e1(s_player_action *arg_0)
{
 player_action_initialize(arg_0);
}
#pragma inline_depth(8)
// @retail 0x825e0
void function_825e0(const s_weapon_activity_result *input, s_player_action *action)
{
 const byte *source = (const byte *)input;
 byte *result = (byte *)action;
 function_825e1(action);
 *(long *)(result + 4) = *(const long *)source;
 *(long *)(result + 8) = *(const long *)(source + 4);
 *(long *)(result + 0xc) = *(const long *)(source + 8);
 *(long *)(result + 0x10) = *(const long *)(source + 0xc);
 if (source[0x10] & 1) *(dword *)result |= 1; else *(dword *)result &= ~1;
 if (source[0x10] & 2) *(dword *)result |= 2; else *(dword *)result &= ~2;
 if (source[0x10] & 4) *(dword *)result |= 0x800; else *(dword *)result &= ~0x800;
 if (source[0x10] & 8) *(dword *)result |= 0x4000; else *(dword *)result &= ~0x4000;
 *(long *)(result + 0x1e) = *(const long *)(source + 0x12);
 *(word *)(result + 0x24) = *(const word *)(source + 0x16);
 if (source[0x18]) *(dword *)result |= 0x40000; else *(dword *)result &= ~0x40000;
 if (source[0x19]) *(dword *)result |= 0x80000; else *(dword *)result &= ~0x80000;
 if (source[0x1a]) *(dword *)result |= 0x800000; else *(dword *)result &= ~0x800000;
 if (source[0x1b]) *(dword *)result |= 0x1000000; else *(dword *)result &= ~0x1000000;
 function_82b30((const s_object_relevance_result *)(source + 0x20), (s_object_relevance_source *)(result + 0x34));
 if (source[0x30]) *(dword *)result |= 0x40000000; else *(dword *)result &= ~0x40000000;
 if (source[0x31]) *(dword *)result |= 0x80000000; else *(dword *)result &= ~0x80000000;
 result[0x58] = source[0x32];
}
