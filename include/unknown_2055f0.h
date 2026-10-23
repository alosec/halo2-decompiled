/* UNKNOWN_2055F0.H: views of an object's and a component's impact records, shared by
   src/unknown_2055f0.cpp and src/unknown_2078f0.cpp */

#ifndef UNKNOWN_2055F0_H
#define UNKNOWN_2055F0_H

#include "unknown_11c920.h"

struct s_impact_object_view
{
	byte unknown00[0xb4];
	long component_index;
};

struct s_impact_object_header
{
	byte unknown00[8];
	s_impact_object_view *object;
};

struct s_component_impact_view
{
	byte unknown00[8];
	short count;
	byte unknown0a[3];
	char type;
	byte unknown0e[0xa0 - 0xe];
};

#endif
