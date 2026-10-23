/* UNKNOWN_1E3920.H: the actor (0x888 bytes, g_4f55f0) as the movement code of
   0x1f0000..0x1fffff sees it (lane B). slot_handler.h's s_actor_view is the
   slot handlers' view of the same element; the two are not folded together
   yet because they disagree at two places: path[3] (0x59c..0x5b8) here covers
   s_actor_view's unknown5ac..unknown5b6, and unknown6fe here lies inside
   s_actor_view's dword unknown6fc. Elsewhere their offsets agree. */

#ifndef UNKNOWN_1E3920_H
#define UNKNOWN_1E3920_H

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "globals.h"
#include "unknown_20fe20.h"
#include "path.h"

/* a point of the actor's path (0x1c bytes, from +0x548) */
struct s_actor_path_point
{
	s_type_c3b527 node;
	byte unknown10[0x1c - 0x10];
};

struct s_actor_moving
{
	byte unknown000[0x7];
	bool unknown007;
	byte unknown008[0x18 - 0x8];
	long unit_index;
	byte unknown01c[0x40 - 0x1c];
	bool unknown040;
	byte unknown041[0x54 - 0x41];
	long tag_index;
	byte unknown058[0x86 - 0x58];
	short unknown086;
	byte unknown088[0x227 - 0x88];
	bool unknown227;
	byte unknown228;
	bool unknown229;
	byte unknown22a[0x238 - 0x22a];
	point3f position;
	byte unknown244[0x264 - 0x244];
	bool unknown264;
	byte unknown265;
	bool unknown266;
	byte unknown267;
	bool unknown268;
	byte unknown269[0x26c - 0x269];
	long unknown26c;
	byte unknown270[0x274 - 0x270];
	long unknown274;
	byte unknown278[0x27c - 0x278];
	s_path_point location;
	long unknown28c;
	vector3f unknown290;
	byte unknown29c[0x300 - 0x29c];
	long unknown300;
	short unknown304;
	byte unknown306[0x328 - 0x306];
	short unknown328;
	byte unknown32a[0x338 - 0x32a];
	long prop_index;
	byte unknown33c[0x350 - 0x33c];
	long unknown350;
	byte unknown354[0x3e4 - 0x354];
	byte unknown3e4;
	byte unknown3e5[0x484 - 0x3e5];
	bool unknown484;
	bool unknown485;
	byte unknown486[0x48b - 0x486];
	bool unknown48b;
	byte unknown48c[0x4ac - 0x48c];
	short unknown4ac;
	bool unknown4ae;
	byte unknown4af[0x4b8 - 0x4af];
	union
	{
		s_type_c3b527 unknown4b8;
		s_reference unknown4b8_reference;
	};
	byte unknown4c8[0x4cc - 0x4c8];
	real unknown4cc;
	byte unknown4d0[0x4e8 - 0x4d0];
	bool unknown4e8;
	byte unknown4e9[0x4ec - 0x4e9];
	s_type_c3b527 unknown4ec;
	byte unknown4fc[0x504 - 0x4fc];
	short unknown504;
	bool unknown506;
	byte unknown507[0x50c - 0x507];
	bool unknown50c;
	byte unknown50d[0x528 - 0x50d];
	s_type_c3b527 path_start;
	bool unknown538;
	char path_count;
	char path_index;
	byte unknown53b[0x548 - 0x53b];
	s_actor_path_point path[4];
	byte unknown5b8[0x5d0 - 0x5b8];
	bool unknown5d0;
	byte unknown5d1;
	bool unknown5d2;
	byte unknown5d3[0x5d8 - 0x5d3];
	bool unknown5d8;
	byte unknown5d9[0x5ec - 0x5d9];
	vector3f unknown5ec;
	byte unknown5f8[0x605 - 0x5f8];
	bool unknown605;
	byte unknown606[0x608 - 0x606];
	plane3f unknown608;
	real unknown618;
	short unknown61c;
	short unknown61e;
	byte unknown620[0x622 - 0x620];
	bool unknown622;
	byte unknown623;
	real unknown624;
	real unknown628;
	real unknown62c;
	real unknown630;
	long unknown634;
	s_type_c3b527 unknown638;
	vector3f unknown648;
	byte unknown654[0x6c0 - 0x654];
	bool unknown6c0;
	byte unknown6c1[0x6d1 - 0x6c1];
	bool unknown6d1;
	byte unknown6d2[0x6fe - 0x6d2];
	short unknown6fe;
	short unknown700;
	byte unknown702[0x722 - 0x702];
	short unknown722;
	long unknown724;
	byte unknown728[0x858 - 0x728];
	long unknown858;
	byte unknown85c[0x888 - 0x85c];
};

inline s_actor_moving *actor_moving_get(long actor_index)
{
	return (s_actor_moving *)(g_4f55f0->data + (actor_index & 0xffff) * sizeof(s_actor_moving));
}

/* the objects: g_4e0300 holds 12 byte headers with the object at +8 */
struct s_moving_object_header
{
	short salt;
	byte flags;
	byte type;
	byte unknown04[4];
	byte *object;
};

struct s_moving_object
{
	long tag_index;
	byte unknown004[0x88 - 0x4];
	vector3f velocity;
};

inline s_moving_object *moving_object_get(long object_index)
{
	return (s_moving_object *)((s_moving_object_header *)g_4e0300->data)[object_index & 0xffff].object;
}

/* unknown_1e3920.cpp: the radius within which the actor counts as arrived */
real function_1e3920(long actor_index);

/* unknown_1f2fe0.cpp */
void function_1f2fe0(long actor_index);
bool function_1f3100(long actor_index);
void function_1f3190(long actor_index);
bool function_1f3230(long actor_index, real radius);
bool function_1f3430(long actor_index);
bool function_1f34b0(long actor_index, point3f const *point, vector3f const *normal, real distance, short ticks);
bool function_1f3540(long actor_index, vector3f *normal);
bool function_1f3610(long actor_index, short value);

/* unknown_1f8640.cpp */
bool function_1f8660(long index);
void function_1f86a0(long index);
void function_1f8780(long actor_index, bool unknown);

#endif
