// @flags /O2 /GL-
// Havok (third-party physics library): the rest of the contact managers of
// port/recovered/havok_contact_mgr_318a80.cpp:
//   0x318950  operator delete of the objects allocated with tag 0x1d (the
//             managers): the block back to the Havok memory, its size kept
//             in the object (only the unwind funclets call it by address)
//   the small manager (vtable 0x415040, 0x18 bytes, made by 0x318bb0): it
//   keeps no points, only telling the listeners
//     slot 1 0x318a90 add_contact_point (always id 0)
//     slot 2 0x318980 remove_contact_point
//     slot 3 0x3189f0 process_contact
//   the full manager (vtable 0x415074)
//     0x3190a0  its destructor (the deleting one, 0x319190, is in
//               port/recovered/idiom_functions_generated.cpp): the world lets
//               its constraint go when it still has points, then the
//               constraint is destroyed (0x315510)
//     0x318f50  slot 7 (through the thunk 0x318fa0, this + 0x30: the
//               constraint's point ids): every id in use appended to an
//               array of shorts
// The events are those of havok_contact_mgr_318a80.cpp; the managers'
// vtables are the retail ones.
#include <xmmintrin.h>
#include "reference_counted_2d88a0.h"
#include "growable_array_2d91d0.h"
#include "listener_world_315d20.h"
#include "unknown_1cec30.h"

// ---------------------------------------------------------------------------
// recovered code this file calls, declared as src declares it

// sending events to a world's listeners (src/unknown_318320.cpp)
struct listener_world_318320;
void __cdecl function_318320(listener_world_318320 *world, event_315d20 *event);
void __cdecl function_3183b0(listener_world_318320 *world, event_315d20 *event);
void __cdecl function_318440(listener_world_318320 *world, event_315d20 *event);
// and to an entity's own (src/unknown_315d20.cpp)
void __cdecl function_315d20(listener_world_315d20 *world, event_315d20 *event);
void __cdecl function_315d90(listener_world_315d20 *world, event_315d20 *event);
void __cdecl function_315e00(listener_world_315d20 *world, event_315d20 *event);

// the world letting a constraint go (src/unknown_30ca30.cpp)
class c_1d1261
{
public:
	hkBool function_30ca30(void *arg_0);
};

// the contact constraint's destructor (0x315510)
extern "C" void h2_retail_315510(void *constraint);

// ---------------------------------------------------------------------------

// @retail 0x318950
void __cdecl function_318950(void *object)
{
	g_memory_2d88a0->release_block(object, ((referenced_2d88a0 *)object)->memory_size, 0x1d);
}

// an entity as the managers read it (havok_contact_mgr_318a80.cpp's
// entity_318c10)
class entity_318a90
{
public:
	void const *vtable;
	unsigned short memory_size;
	unsigned short reference_count;
	char unknown8[0x3c - 0x8];
	c_havok_body_motion_state *motion;
	char unknown40[0x5c - 0x40];
	unsigned short process_delay;
	char unknown5e[0x60 - 0x5e];
	listener_array_315d20 listeners_60;
};

typedef char assert_entity_318a90_listeners[__builtin_offsetof(entity_318a90, listeners_60) == 0x60 ? 1 : -1];

struct collidable_318a90
{
	char unknown0[0x20];
	entity_318a90 *owner;
};

struct contact_point_318a90
{
	__m128 position;
	__m128 normal;
};

struct contact_added_event_318a90
{
	unsigned short id;
	unsigned short unknown2;
	void *manager;
	collidable_318a90 const *collidable_a;
	collidable_318a90 const *collidable_b;
	void *world;
	contact_point_318a90 *point;
	void *properties;
	float projected_velocity;
	long status;
};

struct contact_removed_event_318a90
{
	unsigned short id;
	unsigned short unknown2;
	void *manager;
	entity_318a90 *entity_a;
	entity_318a90 *entity_b;
	void *world;
};

struct contact_processed_event_318a90
{
	void *manager;
	collidable_318a90 const *collidable_a;
	collidable_318a90 const *collidable_b;
	void *output;
	void *world;
};

// the small manager (havok_contact_mgr_318a80.cpp's contact_mgr_318a90)
class contact_mgr_small_318a90
{
public:
	unsigned short add_contact_point(collidable_318a90 const *a, collidable_318a90 const *b,
		void const *input, contact_point_318a90 const *point);
	void remove_contact_point(unsigned short id);
	void process_contact(collidable_318a90 const *a, collidable_318a90 const *b, void *input, void *output);

	void const *vtable;
	unsigned short memory_size;
	unsigned short reference_count;
	void *world;
	entity_318a90 *entity_a;
	entity_318a90 *entity_b;
	long process_countdown;
};

typedef char assert_contact_mgr_small_318a90_size[sizeof(contact_mgr_small_318a90) == 0x18 ? 1 : -1];

// the listeners hear of the point (with no properties, and the closing
// speed along its normal); nothing is kept, and the id is always 0
// @retail 0x318a90
unsigned short contact_mgr_small_318a90::add_contact_point(collidable_318a90 const *a, collidable_318a90 const *b,
	void const *input, contact_point_318a90 const *point)
{
	entity_318a90 *const first = entity_a;
	entity_318a90 *const second = entity_b;
	hkVector4 velocity_a;
	hkVector4 velocity_b;
	first->motion->getPointVelocity(*(hkVector4 const *)&point->position, velocity_a);
	second->motion->getPointVelocity(*(hkVector4 const *)&point->position, velocity_b);
	__m128 const along = _mm_mul_ps(_mm_sub_ps(velocity_b.m_quad, velocity_a.m_quad), point->normal);
	float projected_velocity;
	_mm_store_ss(&projected_velocity, _mm_add_ss(_mm_shuffle_ps(along, along, 0xaa),
		_mm_add_ss(_mm_shuffle_ps(along, along, 0x55), along)));

	contact_point_318a90 copy;
	copy.position = point->position;
	copy.normal = point->normal;
	contact_added_event_318a90 event;
	event.id = 0;
	event.manager = this;
	event.collidable_a = a;
	event.collidable_b = b;
	event.point = &copy;
	event.properties = 0;
	event.projected_velocity = projected_velocity;
	// (retail leaves the status unset: nothing reads it back)
	event.status = 0;
	function_318320((listener_world_318320 *)world, (event_315d20 *)&event);
	if (first->listeners_60.size)
	{
		function_315d20((listener_world_315d20 *)first, (event_315d20 *)&event);
	}
	if (second->listeners_60.size)
	{
		function_315d20((listener_world_315d20 *)second, (event_315d20 *)&event);
	}
	return 0;
}

// @retail 0x318980
void contact_mgr_small_318a90::remove_contact_point(unsigned short id)
{
	entity_318a90 *const first = entity_a;
	entity_318a90 *const second = entity_b;
	contact_removed_event_318a90 event;
	event.id = id;
	event.manager = this;
	event.entity_a = first;
	event.entity_b = second;
	function_3183b0((listener_world_318320 *)world, (event_315d20 *)&event);
	if (first->listeners_60.size)
	{
		function_315d90((listener_world_315d20 *)first, (event_315d20 *)&event);
	}
	if (second->listeners_60.size)
	{
		function_315d90((listener_world_315d20 *)second, (event_315d20 *)&event);
	}
}

// every so many calls (the smaller of the two entities' delays, plus one)
// the listeners hear of it
// @retail 0x3189f0
void contact_mgr_small_318a90::process_contact(collidable_318a90 const *a, collidable_318a90 const *b,
	void *input, void *output)
{
	if (--process_countdown != 0)
	{
		return;
	}
	entity_318a90 *const owner_b = b->owner;
	entity_318a90 *const owner_a = a->owner;
	unsigned short delay = owner_b->process_delay;
	if (owner_a->process_delay < delay)
	{
		delay = owner_a->process_delay;
	}
	process_countdown = delay + 1;
	contact_processed_event_318a90 event;
	event.manager = this;
	event.collidable_a = a;
	event.collidable_b = b;
	event.output = output;
	function_318440((listener_world_318320 *)world, (event_315d20 *)&event);
	if (owner_a->listeners_60.size)
	{
		function_315e00((listener_world_315d20 *)owner_a, (event_315d20 *)&event);
	}
	if (owner_b->listeners_60.size)
	{
		function_315e00((listener_world_315d20 *)owner_b, (event_315d20 *)&event);
	}
}

// ---------------------------------------------------------------------------
// the full manager

ELEMENTS_2D91D0(ids_318f50, unsigned short);

// the contact constraint's point ids: each id's slot in the points, 0xff
// when free
struct point_ids_318f50
{
	unsigned char *data;
	long size;
	long capacity_and_flags;

	void get_ids(ids_318f50 *ids);
};

// @retail 0x318f50
void point_ids_318f50::get_ids(ids_318f50 *ids)
{
	for (long id = 0; id < size; id++)
	{
		if (data[id] != 0xff)
		{
			ids->append((unsigned short)id);
		}
	}
}

class contact_mgr_3190a0
{
public:
	void destruct();

	void const *vtable;
	unsigned short memory_size;
	unsigned short reference_count;
	c_1d1261 *world;
	long process_countdown;
	// the contact constraint (havok_contact_mgr_318a80.cpp's
	// contact_constraint_31c000); its points' properties' count at 0x94
	char constraint[0x94];
	long point_count;
	char unknowna8[0x120 - 0xa8];
};

typedef char assert_contact_mgr_3190a0_size[sizeof(contact_mgr_3190a0) == 0x120 ? 1 : -1];
typedef char assert_contact_mgr_3190a0_points[__builtin_offsetof(contact_mgr_3190a0, point_count) == 0xa4 ? 1 : -1];

// @retail 0x3190a0
void contact_mgr_3190a0::destruct()
{
	vtable = (void const *)0x415074;
	if (point_count)
	{
		world->function_30ca30(constraint);
	}
	h2_retail_315510(constraint);
	// the base's table (0x452c30)
	vtable = (void const *)0x452c30;
}
