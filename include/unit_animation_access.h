/* UNIT_ANIMATION_ACCESS.H: the animation data of a unit. Each unit keeps two
   self-relative blocks: the animation state at the offset stored at +0x12a
   (unknown_1cafc0.cpp's s_animation_state) and the animation control at the
   offset stored at +0x33e, whose layers blend extra animations over it
   (src/unit_animation_access.cpp, src/unit_animation_blend.cpp). */

#ifndef UNIT_ANIMATION_ACCESS_H
#define UNIT_ANIMATION_ACCESS_H

#include "unknown_11c920.h"
#include "unknown_1dacb0.h"
#include "unknown_1c62f0.h"
#include <stddef.h>

/* the unit (a view of the object data) */
struct s_animated_unit
{
	long definition_index;
	byte unknown004[0xb3 - 4];
	byte value_b3; /* the timing query runs only while this is zero */
	byte unknown0b4[0x12a - 0xb4];
	short animation_state_offset;
	byte unknown12c[0x14c - 0x12c];
	long stance_name; /* a string id the stance query maps to a mode */
	byte unknown150[0x33e - 0x150];
	short control_offset;
};
typedef char assert_animated_unit_state[(offsetof(s_animated_unit, animation_state_offset) == 0x12a) ? 1 : -1];
typedef char assert_animated_unit_stance[(offsetof(s_animated_unit, stance_name) == 0x14c) ? 1 : -1];
typedef char assert_animated_unit_size[(sizeof(s_animated_unit) == 0x340) ? 1 : -1];

struct s_animated_unit_header
{
	byte unknown00[8];
	s_animated_unit *unit;
};

/* a layer that fades a triggered animation in and out and plays a periodic
   one when a random countdown runs out (0x2c bytes at control +0x30) */
struct s_unit_animation_periodic_layer
{
	c_type_709360 triggered_animation;
	c_type_709360 periodic_animation;
	long trigger; /* NONE when idle; while set the weight ramps up */
	byte unknown0c[4];
	real weight;
	byte unknown14[0x20 - 0x14];
	signed char blend_weights[12];
};
typedef char assert_unit_animation_periodic_layer_size[(sizeof(s_unit_animation_periodic_layer) == 0x2c) ? 1 : -1];

/* a blend of 13 named channels toward one of them (0x20 bytes at control
   +0x5c) */
struct s_unit_animation_channel_blend
{
	c_type_709360 animation;
	real rate;
	real progress;
	real target_weight;
	short channel;
	byte weights[13];
	byte unknown1f;
};
typedef char assert_unit_animation_channel_blend_size[(sizeof(s_unit_animation_channel_blend) == 0x20) ? 1 : -1];

enum
{
	k_unit_animation_blend_channel_count = 13
};

/* the animation control at the unit's offset +0x33e */
struct s_unit_animation_control
{
	word : 7;
	word flag7 : 1;
	word : 8;
	byte countdown; /* ticks until the periodic animation plays */
	char stance_request_delay;
	byte unknown04[0x30 - 4];
	s_unit_animation_periodic_layer periodic_layer;
	s_unit_animation_channel_blend channel_blend;
	byte unknown7c[0x9c - 0x7c];
	c_animation_channel channels[3];
};
typedef char assert_unit_animation_control_periodic[(offsetof(s_unit_animation_control, periodic_layer) == 0x30) ? 1 : -1];
typedef char assert_unit_animation_control_blend[(offsetof(s_unit_animation_control, channel_blend) == 0x5c) ? 1 : -1];
typedef char assert_unit_animation_control_channels[(offsetof(s_unit_animation_control, channels) == 0x9c) ? 1 : -1];
typedef char assert_unit_animation_control_size[(sizeof(s_unit_animation_control) == 0xfc) ? 1 : -1];

#define ANIMATED_UNIT_GET(index) (((s_animated_unit_header *)g_4e0300->data)[(index) & 0xffff].unit)
#define UNIT_ANIMATION_STATE(unit) ((s_animation_state *)((byte *)(unit) + (unit)->animation_state_offset))
#define UNIT_ANIMATION_CONTROL(unit) ((s_unit_animation_control *)((byte *)(unit) + (unit)->control_offset))
#define ANIMATION_GRAPH_OF(index) ((s_graph_tag *)g_4e3b44[(index) & 0xffff].bytes)

void unit_animation_channel_blend_set_by_name(long unit_index, long name, real weight, real seconds);

#endif
