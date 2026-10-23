/* SCENERY_ANIMATION.H: the scenery datum as scenery_animation_begin
   (scenery_animation.cpp) reads it */

#ifndef SCENERY_ANIMATION_H
#define SCENERY_ANIMATION_H

#include "object_dispatch.h"

/* a scenery object ("scenery" datum_size 0x13c): the object datum, then the
   scenery's own fields */
struct s_scenery_object_datum : s_object_common_datum
{
	dword flags;                          /* bit 0 is cleared when an animation starts */
	byte unknown130[0x138 - 0x130];
	long attached_object_index;           /* the object the animation is relative to, or NONE */
};
typedef char assert_scenery_object_datum_flags[(offsetof(s_scenery_object_datum, flags) == 0x12c) ? 1 : -1];
typedef char assert_scenery_object_datum_size[(sizeof(s_scenery_object_datum) == 0x13c) ? 1 : -1];

bool scenery_animation_begin(long scenery_index, long animation_name, short frame, long attached_object_index, bool interpolate, bool loop, long animation_graph_index);

#endif
