/* SWARM.H: the ai's swarms. A swarm (the "swarm" data array at g_5044c8, 32
   entries of 0x34 bytes) is a chain of creature objects (object type 12)
   that one actor drives; each creature keeps its swarm data in the block at
   its object's ai_offset. g_5044c4 holds the five slots that spawn swarms at
   an object's marker. */

#ifndef SWARM_H
#define SWARM_H

#include "unknown_2551c0.h"
#include <stddef.h>

/* the object type of a swarm's creatures (the iterator's mask 0x1000) */
enum
{
	k_object_type_creature = 12
};

/* what a swarm creature is steering for (swarm_target_get_location) */
enum e_swarm_target_type
{
	_swarm_target_type_object = 1,
	_swarm_target_type_prop,
	_swarm_target_type_actor
};

/* a "swarm" datum (0x34 bytes) */
struct s_swarm
{
	byte unknown00[4];
	long actor_index;                 /* the actor that drives the swarm */
	point3f position;
	short creature_count;
	short initial_creature_count;
	long first_creature_index;        /* the chain runs through s_object_ai_data::next_creature_index */
	short event_count;                /* counted by 0x28fc50, cleared 2 seconds after the last */
	byte unknown1e[2];
	long last_event_time;             /* game time of the last event */
	point3f gather_position;
	bool gather_position_valid;
	bool spawn_slot_reference;        /* a spawn slot may still name this swarm */
	byte unknown32[2];
};
typedef char assert_swarm_size[(sizeof(s_swarm) == 0x34) ? 1 : -1];
typedef char assert_swarm_first_creature[(offsetof(s_swarm, first_creature_index) == 0x18) ? 1 : -1];
typedef char assert_swarm_spawn_slot_reference[(offsetof(s_swarm, spawn_slot_reference) == 0x31) ? 1 : -1];

inline s_swarm *swarm_get(long swarm_handle)
{
	return (s_swarm *)(g_5044c8->data + (swarm_handle & 0xffff) * sizeof(s_swarm));
}

/* a slot of the table at g_5044c4 (0x50 bytes): a swarm spawned at a marker
   of an object (0x2902b0 fills the rest) */
struct s_swarm_spawn_slot
{
	long object_index;
	byte unknown04[0x48 - 4];
	long swarm_handle;
	short count;
	byte unknown4e[2];
};
typedef char assert_swarm_spawn_slot_size[(sizeof(s_swarm_spawn_slot) == 0x50) ? 1 : -1];

enum
{
	k_maximum_swarm_spawn_slots = 5
};

/* g_5044c4: the swarm spawn slots (k_maximum_swarm_spawn_slots, in the game
   state); 0x290250, 0x2902b0 and 0x2909b0 read it as well */
extern s_swarm_spawn_slot *g_5044c4;

/* the swarm data of a creature, at its object's ai_offset */
struct s_object_ai_data
{
	union
	{
		word flags;
		struct { word attached : 1; word other_flags : 15; };    /* the object has a parent */
	};
	byte unknown02[2];
	long actor_index;
	long swarm_handle;
	long next_creature_index;
	long prop_index;                  /* forgotten with swarm_forget_prop */
	point3f position;
	long index20;                     /* cleared by swarm_creatures_clear_index20 */
	byte unknown24[0x4c - 0x24];
	short countdown;
	byte unknown4e[2];
	long goal_prop_ref;           /* the prop_ref the creature heads for */
	short target_state;
	bool flag56;
	bool velocity_pending;
	point3f pending_velocity;         /* 0x2900b0's launch velocity, taken by swarm_creature_get_pending_velocity */
	short index64;
	short count66;
	byte unknown68[0x74 - 0x68];
	point3f position74;
	vector3f forward;
	vector3f up;
	vector3f vector98;
	bool animation_pending;
	byte unknowna5[3];
	long animation_name4;
	long animation_name0;
};
typedef char assert_object_ai_data_target_prop[(offsetof(s_object_ai_data, goal_prop_ref) == 0x50) ? 1 : -1];
typedef char assert_object_ai_data_pending_velocity[(offsetof(s_object_ai_data, pending_velocity) == 0x58) ? 1 : -1];
typedef char assert_object_ai_data_forward[(offsetof(s_object_ai_data, forward) == 0x80) ? 1 : -1];
typedef char assert_object_ai_data_size[(sizeof(s_object_ai_data) == 0xb0) ? 1 : -1];

/* an object's orientation request (0x30 bytes, at object +0x13c; 0x118f90
   builds the default one) */
struct s_object_orientation_request
{
	long name0;
	long name4;
	word enabled : 1;
	word : 2;
	word flag3 : 1;
	word flag4 : 1;
	word : 11;
	byte unknown0a[2];
	vector3f vector0c;
	vector3f forward;
	vector3f up;
};
typedef char assert_object_orientation_request_size[(sizeof(s_object_orientation_request) == 0x30) ? 1 : -1];

/* the object fields the swarm code reads (an object of g_4e0300) */
struct s_swarm_object
{
	byte unknown000[0x14];
	long parent_index;
	byte unknown018[0x30 - 0x18];
	vector3f velocity;
	byte unknown03c[0x10a - 0x3c];
	word : 2;
	word convertible : 1;             /* on a parent unit: swarm_creature_convert_parent_unit may act */
	word : 2;
	word flag5 : 1;
	word flag6 : 1;
	word : 9;
	byte unknown10c[0x12c - 0x10c];
	word orientation_pending : 1;
	word : 4;
	word paused : 1;
	word : 10;
	short team;
	byte unknown130[0x13c - 0x130];
	s_object_orientation_request orientation;
};
typedef char assert_swarm_object_team[(offsetof(s_swarm_object, team) == 0x12e) ? 1 : -1];
typedef char assert_swarm_object_orientation[(offsetof(s_swarm_object, orientation) == 0x13c) ? 1 : -1];

/* the state of a prop (prop_node_state) */
struct s_swarm_prop_state
{
	byte unknown00[4];
	point3f position;
	byte unknown10[0x30 - 0x10];
	vector3f velocity;
};

/* an object header of g_4e0300 */
struct s_swarm_object_header
{
	byte unknown0[3];
	byte type;
	byte unknown4[4];
	s_swarm_object *object;
};
typedef char assert_swarm_object_header_size[(sizeof(s_swarm_object_header) == 0xc) ? 1 : -1];

inline s_swarm_object_header *swarm_object_header_get(long object_index)
{
	return &((s_swarm_object_header *)g_4e0300->data)[object_index & 0xffff];
}

/* the actor fields the swarm code reads (an actor of g_4f55f0, 0x888 bytes) */
struct s_swarm_leader
{
	byte unknown000[0xc];
	bool flag00c;
	byte unknown00d[0x18 - 0xd];
	long unit_index;
	long swarm_handle;
	byte unknown020[0x30 - 0x20];
	dword squad_index : 16;
	dword : 16;
	byte unknown034[0x7c - 0x34];
	long clump_index;
	byte unknown080[0x22c - 0x80];
	vector3f velocity;
	point3f position;
	byte unknown244[0x338 - 0x244];
	long prop_index;
	byte unknown33c[0x888 - 0x33c];
};
typedef char assert_swarm_actor_size[(sizeof(s_swarm_leader) == 0x888) ? 1 : -1];
typedef char assert_swarm_actor_position[(offsetof(s_swarm_leader, position) == 0x238) ? 1 : -1];
typedef char assert_swarm_actor_prop_index[(offsetof(s_swarm_leader, prop_index) == 0x338) ? 1 : -1];

inline s_swarm_leader *swarm_leader_get(long actor_index)
{
	return &((s_swarm_leader *)g_4f55f0->data)[actor_index & 0xffff];
}

/* the swarm's iterator (swarm_iterator_new; 0x256bd0 steps it) */
struct s_290c50_iterator
{
	long unknown00;
	s_record_pool_iterator records;
	long object_index;
};

/* swarm.cpp */
void function_28d930(void);
void function_28d9d0(void);
void swarm_iterator_new(s_290c50_iterator *iterator);
void __stdcall swarm_delete(long swarm_handle);
bool __stdcall swarm_remove_creature(long swarm_handle, long object_index, bool remove_from_world, bool delete_when_empty);
void __stdcall swarm_leader_attach(long actor_index, long swarm_handle);
void swarm_leader_detach(long actor_index);
void swarm_set_team(long swarm_handle, short team);
void swarm_forget_prop(long swarm_handle, long prop_index);
void swarm_creatures_clear_index20(long swarm_handle);
void swarm_creatures_set_object_flag(long swarm_handle, bool flag6);
void swarm_reset_orientations(long swarm_handle);
void swarm_apply_impulse(long swarm_handle, vector3f const *impulse);
void swarm_spawn_slots_clear_object(long object_index);
void swarm_creature_initialize(s_object_ai_data *creature);
void swarm_creature_reset_position(long object_index, s_object_ai_data *creature);
void swarm_creature_reset_orientation(long object_index);
void swarm_creature_update_countdown(long object_index, s_object_ai_data *creature);
bool swarm_creature_get_pending_velocity(long object_index, point3f *velocity);
void swarm_creature_apply_orientation(long object_index);
void swarm_creature_dispose(long object_index);
bool swarm_target_get_location(long target_index, short target_type, long *object_index, point3f *position, vector3f *velocity);

/* swarm_sse.cpp */
void __stdcall swarm_update(long actor_index);
void __stdcall swarm_creature_convert_parent_unit(long object_index);

#endif
