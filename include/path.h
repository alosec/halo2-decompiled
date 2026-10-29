/* PATH.H: the ai's path search (src/unknown_271e50.cpp) and the query an
   actor builds for it (src/unknown_1f9240.cpp fills its settings, 0x1f90f0
   its source). */

#ifndef PATH_H
#define PATH_H

#include "unknown_11c920.h"
#include "unknown_0259d0.h"

/* what an actor may path through (0x1c bytes, function_1f9240) */
struct s_path_settings
{
	long flags;
	long unknown04;
	long unknown08;
	bool unknown0c;
	byte unknown0d;
	short unknown0e;
	short unknown10;
	short unknown12;
	short unknown14;
	byte unknown16[2];
	long unknown18;
};

/* a point and the cluster it lies in (0x10 bytes) */
struct s_path_point
{
	point3f point;
	short cluster_index;
	short unknown0e;
};

/* where a path starts (0x50 bytes; function_1f90f0 fills it) */
struct s_path_source
{
	real radius;
	byte unknown04;
	byte unknown05[3];
	long object_index;
	long unknown0c;
	bool has_point;
	byte unknown11[3];
	s_path_point point;
	long unknown24;
	byte unknown28[0x44 - 0x28];
	byte unknown44;
	bool unknown45;
	byte unknown46[2];
	real unknown48;
	real unknown4c;
};

struct s_path_query
{
	s_path_settings settings;
	s_path_source source;
};

/* a location a search may begin from (0x1c bytes) */
struct s_path_location
{
	short unknown00;
	short unknown02;
	byte unknown04[0x18];
};

struct s_type_136112
{
	short heap_index;
	byte unknown02[0x42];
};

struct path_heap_entry
{
	short node;
	short cost;
};

struct s_type_f17a25
{
	s_path_source source;
	long flags;
	byte unknown54;
	byte unknown55[0x70 - 0x55];
	s_path_location location;
	void *pathfinding;
	short unknown90;
	byte unknown92[0xac - 0x92];
	bool unknownac;
	byte unknownad;
	short unknownae;
	byte unknownb0[0xf0 - 0xb0];
	s_type_136112 nodes[1023];
	byte unknown_pad[4];
	short heap_count;
	path_heap_entry heap[1024];
	byte unknown120b2[0x140b8 - 0x120b2];
	s_path_settings settings;
	byte unknown140d4[0x14188 - 0x140d4];
	short unknown14188;
};

void function_271300(s_type_f17a25 *state, s_path_location const *location, s_path_settings const *settings,
	s_path_source const *source, long flags);

#endif
