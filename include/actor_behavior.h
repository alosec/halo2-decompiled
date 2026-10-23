/* ACTOR_BEHAVIOR.H: the per-slot states of the actor behaviors whose
   callbacks are in actor_behavior.cpp and actor_behavior_x87.cpp.

   Each actor (s_actor_view, slot_handler.h) runs a stack of up to four
   behavior slots (s_slot, 0x40 bytes). A slot starts with s_slot_header;
   the rest of its 0x40 bytes belongs to the behavior type that runs in it,
   and these structs give that rest for the types handled here. */

#ifndef ACTOR_BEHAVIOR_H
#define ACTOR_BEHAVIOR_H

#include "slot_handler.h"
#include <stddef.h>

/* behavior type 4: the actor runs its command script (s_actor_view's
   command_script_index) */
struct s_command_script_behavior_state
{
	s_slot_header header;
	byte unknown0c[5];
	bool finished;          /* the script ended or lost its thread */
	bool child_active;      /* the script started the behavior one level down */
	byte unknown13[0x40 - 0x13];
};
typedef char assert_command_script_behavior_state_size[(sizeof(s_command_script_behavior_state) == 0x40) ? 1 : -1];

/* behavior type 0x3a: the actor moves toward a reference */
struct s_actor_behavior_3a_data
{
	s_slot_header header;
	byte unknown0c[3];
	bool gave_up;           /* set when the reference is dropped */
	s_reference reference;  /* g_470fa0 when there is none */
	bool reference_flag;    /* passed on to the actor at +0x3f0 */
	byte unknown15[0x40 - 0x15];
};
typedef char assert_behavior_3a_state_size[(sizeof(s_actor_behavior_3a_data) == 0x40) ? 1 : -1];
typedef char assert_behavior_3a_state_reference[(offsetof(s_actor_behavior_3a_data, reference) == 0x10) ? 1 : -1];

/* behavior type 0x64: the actor walks to a point, then faces a prop */
struct s_actor_behavior_64_data
{
	s_slot_header header;
	long prop_index;
	byte unknown10[6];
	bool vocalized;         /* the unit made vocalization 0x37 about the prop */
	bool unknown17;
	bool arrived;
	byte unknown19[3];
	s_type_c3b527 destination;
	byte unknown2c[4];
	real radius;            /* movement stops inside it */
	byte unknown34[0x40 - 0x34];
};
typedef char assert_behavior_64_state_size[(sizeof(s_actor_behavior_64_data) == 0x40) ? 1 : -1];
typedef char assert_behavior_64_state_destination[(offsetof(s_actor_behavior_64_data, destination) == 0x1c) ? 1 : -1];
typedef char assert_behavior_64_state_radius[(offsetof(s_actor_behavior_64_data, radius) == 0x30) ? 1 : -1];

/* behavior type 0x6c moves the actor from point to point; behavior group 7
   waits a random time. Both keep one counter at +0xc: the index of the next
   point, or the ticks left. */
struct s_counter_behavior_state
{
	s_slot_header header;
	short counter;
	byte unknown0e[0x40 - 0xe];
};
typedef char assert_counter_behavior_state_size[(sizeof(s_counter_behavior_state) == 0x40) ? 1 : -1];

/* behavior type 0x50: the point it makes the actor look at */
struct s_actor_behavior_50_data
{
	s_slot_header header;
	byte unknown0c[0x1b - 0xc];
	bool has_look_point;
	point3f look_point;
	byte unknown28[0x40 - 0x28];
};
typedef char assert_behavior_50_state_size[(sizeof(s_actor_behavior_50_data) == 0x40) ? 1 : -1];
typedef char assert_behavior_50_state_look_point[(offsetof(s_actor_behavior_50_data, look_point) == 0x1c) ? 1 : -1];

/* behavior type 0x15: the actor keeps its distance from its prop */
struct s_actor_behavior_15_data
{
	s_slot_header header;
	long prop_index;        /* the actor's prop when the behavior took it */
	bool repositioned;      /* it searched for a new position */
	bool settled;           /* close enough after the search; it stops looking */
	byte unknown12[0x40 - 0x12];
};
typedef char assert_behavior_15_state_size[(sizeof(s_actor_behavior_15_data) == 0x40) ? 1 : -1];

/* behavior type 0x82 (unknown_1c10a0.cpp): the value at +0x10 */
struct s_actor_behavior_82_data
{
	s_slot_header header;
	byte unknown0c[4];
	long value;
};

/* the state of a prop of type 1: its position at +4 */
struct s_prop_position_state
{
	long unknown00;
	point3f position;
};

/* the block of the actor's character tag that function_1e5030 returns: a
   range of seconds */
struct s_character_wait_range
{
	real lower;
	real upper;
};

/* the actor's movement command (0x40 bytes at +0x4ac of s_actor_view) */
struct s_actor_move_command
{
	short mode;             /* 2: move to destination */
	bool mode_changed;
	byte unknown03;
	real value04;
	real value08;
	s_type_c3b527 destination;
	long path_index;
	real value20;
	real value24;
	bool flag28;
	bool flag29;
	byte unknown2a[0x38 - 0x2a];
	long index38;
	bool flag3c;
};
typedef char assert_actor_move_command_destination[(offsetof(s_actor_move_command, destination) == 0xc) ? 1 : -1];
typedef char assert_actor_move_command_path_index[(offsetof(s_actor_move_command, path_index) == 0x1c) ? 1 : -1];
typedef char assert_actor_move_command_index38[(offsetof(s_actor_move_command, index38) == 0x38) ? 1 : -1];

#endif
