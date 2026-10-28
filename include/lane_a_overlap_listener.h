#ifndef LANE_A_OVERLAP_LISTENER_H
#define LANE_A_OVERLAP_LISTENER_H
#include "world_object_30b340.h"
#include "growable_array_2d91d0.h"
// Prefix needed by listener append. Avoid importing unrelated description
// classes into the game's already-defined bounds-description translation unit.
struct lane_a_listener_array
{
	void **data;
	long size;
	long capacity_and_flags;
};
class c_2784c5 : public class_30b340
{
public:
	lane_a_listener_array field_3c;
	void function_315910(void *listener);
};
#endif
