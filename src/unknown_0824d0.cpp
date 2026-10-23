// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "globals.h"
#include <string.h>
#include <math.h>
struct s_082b70_object
{
	byte unknown000[0xc];
	long next;
	long child;
	long parent;
	byte unknown018[0xaa - 0x18];
	char type;
	byte unknown0ab[0x10a - 0xab];
	struct { word bit0 : 1; word bit1 : 1; word bit2 : 1; word unused : 13; } flags10a;
	byte unknown10c[0x13c - 0x10c];
	long controller;
	byte unknown140[0x212 - 0x140];
	char weapon_slots[2];
	byte unknown214[4];
	long weapons[4];
	byte unknown228[0x248 - 0x228];
	long occupant;
	long driver;
};

struct s_082b70_object_header
{
	byte unknown00[8];
	s_082b70_object *object;
};

static inline s_082b70_object *object_get_082b70(long object_index)
{
	return ((s_082b70_object_header *)g_4e0300->data)[object_index & 0xffff].object;
}

struct s_object_relevance_source;
struct s_object_relevance_result
{
 real first;
 real second;
 long object_index;
 long identifier;
};
void function_82ac0(s_object_relevance_source *source, s_object_relevance_result *result);

struct s_weapon_activity_result
{
 byte unknown00[0x18];
 bool active[2][2];
 bool update_relevance;
 byte unknown1d[3];
 s_object_relevance_result relevance;
 bool consumed[2];
};

struct s_weapon_trigger_view
{
 byte unknown00[8];
 short barrel;
 byte unknown0a[2];
 short mode;
 byte unknown0e[0x40 - 0xe];
};

struct s_weapon_definition_view
{
 byte unknown00[0x2c8];
 long trigger_count;
 s_weapon_trigger_view *triggers;
 long barrel_count;
 byte *barrels;
};

void function_82d20(long object_index, s_object_relevance_source *source, s_weapon_activity_result *result);
// @retail 0x824d0
void function_824d0(const byte *input, s_weapon_activity_result *output, long object_index)
{
 byte *object = (byte *)object_get_082b70(object_index);
 byte *result = (byte *)output;
 memset(output, 0, 0x34);
 *(real *)result = (real)atan2(*(const real *)(input + 0x38), *(const real *)(input + 0x34));
 real y = *(const real *)(input + 0x38);
 real x = *(const real *)(input + 0x34);
 real z = *(const real *)(input + 0x3c);
 y *= y;
 *(real *)(result + 4) = (real)atan2(z, sqrt(y + x * x));
 if (*(real *)result < 0.0f)
  *(real *)result += 6.2831855f;
 *(long *)(result + 8) = *(const long *)(input + 0x14);
 *(long *)(result + 0xc) = *(const long *)(input + 0x18);
 if (*(const dword *)(input + 0x10) & 1) *(word *)(result + 0x10) |= 1; else result[0x10] &= ~1;
 if (*(const dword *)(input + 0x10) & 2) *(word *)(result + 0x10) |= 2; else result[0x10] &= ~2;
 if (*(const dword *)(input + 0x10) & 0x800) *(word *)(result + 0x10) |= 4; else result[0x10] &= ~4;
 if (*(const dword *)(input + 0x10) & 0x4000) *(word *)(result + 0x10) |= 8; else result[0x10] &= ~8;
 *(long *)(result + 0x12) = *(long *)(object + 0x210);
 if (*(const short *)(input + 6) == *(short *)(result + 0x12))
  *(long *)(result + 0x12) = *(const long *)(input + 6);
 *(short *)(result + 0x16) = *(const short *)(input + 0xc);
 function_82d20(object_index, (s_object_relevance_source *)(input + 0x58), output);
}
