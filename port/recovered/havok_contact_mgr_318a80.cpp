// @flags /O2 /GL-
// Havok (third-party physics library): the contact managers two entities'
// collision agents report their contact points to, the factories that make
// them, and the contact constraint the full manager keeps its points in.
//
//   the factories (header owned_reference_318b90.h)
//     0x318bb0  class_318b90's slot 1 (vtable 0x415060): a small manager
//               (vtable 0x415040, 0x18 bytes) for two collidables
//     0x3191c0  class_319080's slot 1 (vtable 0x41506c): the full manager
//               (vtable 0x415074, 0x120 bytes, built by 0x319110)
//   the full manager (vtable 0x415074; slot 0 its deleting destructor,
//   0x319190, not recovered)
//     slot 1 0x318c10 add_contact_point
//     slot 2 0x318e60 remove_contact_point
//     slot 3 0x318fb0 process_contact
//     slot 4 0x318a80 cleanup (also slot 4 of the small manager's 0x415040)
//     slot 5 0x318f30 get_contact_point_properties
//     slot 6 0x318f10 get_contact_point
//   its contact constraint (at 0x10; vtable 0x4153c8, built by 0x31c000)
//     0x31bac0, 0x31bb30  set the two entities
//     0x31be80, 0x31bc90  add and remove a point, telling the constraint's
//                         owner how much more or less solver room it needs
//     0x31be20            the point ids' table: the slot of a free id
//   the objects' collision filter
//     0x319230  slot 9 of vtable 0x4138a0: whether two collidables' entities
//               are both movable
//     0x3192a0  slots 5 and 6 of vtable 0x415100: always no
//
// The managers' and the constraint's vtables are their retail ones, stored
// by address: nothing here calls them natively but through the slot
// declarations below.
#include <xmmintrin.h>
#include "reference_counted_2d88a0.h"
#include "reference_base_318970.h"
#include "growable_array_2d91d0.h"
#include "listener_world_315d20.h"
#include "unknown_1cec30.h"

// ---------------------------------------------------------------------------
// recovered code this file calls, declared as src declares it

// sending events to a world's listeners (src/unknown_318320.cpp): a contact
// point added, removed, and a contact processed
struct listener_world_318320;
void __cdecl function_318320(listener_world_318320 *world, event_315d20 *event);
void __cdecl function_3183b0(listener_world_318320 *world, event_315d20 *event);
void __cdecl function_318440(listener_world_318320 *world, event_315d20 *event);
// the same to an entity's own listeners (src/unknown_315d20.cpp)
void __cdecl function_315d20(listener_world_315d20 *world, event_315d20 *event);
void __cdecl function_315d90(listener_world_315d20 *world, event_315d20 *event);
void __cdecl function_315e00(listener_world_315d20 *world, event_315d20 *event);

// the world taking in a constraint that links two entities
// (src/unknown_30d2b0.cpp)
struct s_1d1541
{
	void *function_30d2b0(void *arg_0);
};

// the world letting a constraint go (src/unknown_30ca30.cpp); the byte at
// 0x12c, while set, makes it collect the constraint instead
class c_1d1261
{
public:
	char unknown0[0x12c];
	bool holding_entries;

	hkBool function_30ca30(void *arg_0);
};

// an entity adding a constraint to its list (src/unknown_30bdc0.cpp)
class class_30bdc0
{
public:
	void function_30bdc0(long value);
};

// an entity dropping one (src/unknown_30bc60.cpp)
class entity_30bcc0
{
public:
	void function_30bcc0(void *listener);
};

// whether an address lies in the loaded cache file (src/unknown_279740.cpp):
// objects there are never deleted
bool __cdecl function_279740(long value);

// ---------------------------------------------------------------------------
// the objects the managers deal with

// an entity's material: its friction and restitution
struct material_318c10
{
	long field_0;
	float friction;
	float restitution;
};

// an entity (hkRigidBody) as the contact code reads it
class entity_318c10
{
public:
	void const *vtable;
	unsigned short memory_size;
	unsigned short reference_count;
	char unknown8[0x3c - 0x8];
	c_havok_body_motion_state *motion;
	bool fixed;
	char unknown41[0x48 - 0x41];
	material_318c10 material;
	char unknown54[0x5c - 0x54];
	// how many process calls pass between process-contact events
	unsigned short process_delay;
	char unknown5e[0x60 - 0x5e];
	listener_array_315d20 listeners_60;
};

// a collidable: a child's parent, the root's owning entity
struct collidable_318c10
{
	char unknown0[0xc];
	collidable_318c10 *parent;
	char unknown10[0x20 - 0x10];
	entity_318c10 *owner;
};

inline collidable_318c10 const *root_318c10(collidable_318c10 const *collidable)
{
	while (collidable->parent)
	{
		collidable = collidable->parent;
	}
	return collidable;
}

// a contact point: its position (lane 3 the distance) and normal
struct contact_point_318c10
{
	__m128 position;
	__m128 normal;
};

// a contact point's solver properties (0x1c bytes)
struct contact_properties_318c10
{
	float impulse_limit;
	float field_4;
	float friction;
	float restitution;
	long field_10;
	float field_14;
	unsigned short field_18;
	unsigned short unknown1a;
};

// what the adding agent passes along (field_30 scales the restitution's
// part)
struct process_input_318c10
{
	char unknown0[0x30];
	float field_30;
};

// a processed point: where it is now, and its id
struct processed_point_318fb0
{
	contact_point_318c10 point;
	unsigned short id;
	char unknown22[0x30 - 0x22];
};

struct processed_points_318fb0
{
	processed_point_318fb0 *data;
	long size;
};

struct process_output_318fb0
{
	processed_points_318fb0 *points;
};

// ---------------------------------------------------------------------------
// the contact constraint

ELEMENTS_2D91D0(point_ids_31be20_base, unsigned char);
ELEMENTS_2D91D0_ALIGNED(contact_points_31c000, contact_point_318c10);
ELEMENTS_2D91D0(contact_properties_31c000, contact_properties_318c10);

// a point id's slot in the points (0xff when the id is free)
struct point_ids_31be20 : point_ids_31be20_base
{
	int allocate(unsigned char index);
};

class contact_constraint_31c000;

// how much solver room a constraint's points take, given to its owner as it
// changes
struct constraint_info_31be80
{
	long field_0;
	long field_4;
	long field_8;
	long field_c;
};

class constraint_owner_31be80
{
public:
	virtual void slot_0() = 0;
	virtual void slot_1() = 0;
	virtual void add_constraint_info(contact_constraint_31c000 *constraint, constraint_info_31be80 *info) = 0;
	virtual void remove_constraint_info(contact_constraint_31c000 *constraint, constraint_info_31be80 *info) = 0;
};

class contact_constraint_31c000
{
public:
	contact_constraint_31c000();

	void set_entity_a(entity_318c10 *entity);
	void set_entity_b(entity_318c10 *entity);
	int add_point(contact_point_318c10 **point_out, contact_properties_318c10 **properties_out);
	void remove_point(unsigned short id);

	void const *vtable;
	unsigned short memory_size;
	unsigned short reference_count;
	// an object of its own inside it, vtable 0x4153b8
	void const *atom_vtable;
	entity_318c10 *entity_a;
	entity_318c10 *entity_b;
	constraint_owner_31be80 *owner;
	long field_18;
	char unknown1c[0x20 - 0x1c];
	point_ids_31be20 ids;
	unsigned char ids_storage[8];
	contact_points_31c000 points;
	char unknown40[0x50 - 0x40];
	long flags;
	long field_54[7];
	float field_70[4];
	char unknown80[0x90 - 0x80];
	contact_properties_31c000 properties;
	contact_properties_318c10 properties_storage[4];
	char unknown10c[0x110 - 0x10c];
};

typedef char assert_contact_constraint_31c000_size[sizeof(contact_constraint_31c000) == 0x110 ? 1 : -1];

// @retail 0x31c000
contact_constraint_31c000::contact_constraint_31c000()
{
	reference_count = 1;
	// (retail stores the atom's base vtable, 0x415340, first)
	vtable = (void const *)0x4153c8;
	atom_vtable = (void const *)0x4153b8;
	owner = 0;
	entity_b = 0;
	entity_a = 0;
	field_18 = 0;
	ids.size = 0;
	ids.capacity_and_flags = 8 | 0x80000000;
	ids.data = ids_storage;
	points.data = 0;
	points.size = 0;
	points.capacity_and_flags = 0x80000000;
	flags = 0;
	for (int i = 0; i < 7; i++)
	{
		field_54[i] = 0;
	}
	// 1, 2, 3 and 3 over the square root of 14
	field_70[0] = 0.267261237f; // 0x3e88d677
	field_70[1] = 0.534522474f; // 0x3f08d677
	field_70[2] = 0.801783741f; // 0x3f4d41b3
	field_70[3] = 0.801783741f; // 0x3f4d41b3
	properties.size = 0;
	properties.capacity_and_flags = 4 | 0x80000000;
	properties.data = properties_storage;
	for (int i = 0; i < 4; i++)
	{
		*(long *)&properties_storage[i].impulse_limit = 0;
		*(long *)&properties_storage[i].field_4 = 0;
	}
}

// takes a reference to the new entity and puts the constraint on its list;
// takes the constraint off the old one's list and drops its reference (an
// entity in the cache file is kept at one reference instead of deleted)
#define CONTACT_CONSTRAINT_SET_ENTITY_31BAC0(slot, entity) \
	if (entity) \
	{ \
		entity->reference_count++; \
		((class_30bdc0 *)entity)->function_30bdc0((long)this); \
	} \
	if (slot) \
	{ \
		entity_318c10 *old = slot; \
		((entity_30bcc0 *)old)->function_30bcc0(this); \
		if (--old->reference_count == 0) \
		{ \
			if (function_279740((long)old)) \
			{ \
				old->reference_count = 1; \
			} \
			else \
			{ \
				delete (reference_base_318970 *)old; \
			} \
		} \
	} \
	slot = entity;

// @retail 0x31bac0
void contact_constraint_31c000::set_entity_a(entity_318c10 *entity)
{
	CONTACT_CONSTRAINT_SET_ENTITY_31BAC0(entity_a, entity)
}

// @retail 0x31bb30
void contact_constraint_31c000::set_entity_b(entity_318c10 *entity)
{
	CONTACT_CONSTRAINT_SET_ENTITY_31BAC0(entity_b, entity)
}

// the last free id takes the slot; with none free a new id is made
// @retail 0x31be20
int point_ids_31be20::allocate(unsigned char index)
{
	for (int id = size - 1; id >= 0; id--)
	{
		if (data[id] == 0xff)
		{
			data[id] = index;
			return id;
		}
	}
	append(index);
	return size - 1;
}

// a new point after the others, its properties inserted at the same place
// (the first two words zero; the flag at 0x18 set when the point before has
// it clear); returns the point's id
// @retail 0x31be80
int contact_constraint_31c000::add_point(contact_point_318c10 **point_out, contact_properties_318c10 **properties_out)
{
	long count = properties.size;
	constraint_info_31be80 info;
	info.field_0 = count * 0x30 + 0xb0;
	long extra_4 = 0;
	long extra_8 = 0;
	long extra_c = 0;
	if (count == 1)
	{
		extra_8 = 4;
		extra_4 = 0x20;
		extra_c = count;
	}
	info.field_4 = extra_4 + 0x30;
	info.field_8 = extra_8 + 4;
	info.field_c = extra_c + 1;
	owner->add_constraint_info(this, &info);
	flags |= 6;

	long index = points.size;
	points.resize(index + 1);
	contact_point_318c10 *point = &points[index];

	properties.ensure_capacity(count + 1);
	for (long i = count - 1; i >= index; i--)
	{
		properties[i + 1] = properties[i];
	}
	properties.size = count + 1;
	contact_properties_318c10 *added = &properties[index];
	*(long *)&added->impulse_limit = 0;
	*(long *)&added->field_4 = 0;
	added->field_18 = 0;
	if (index > 0 && properties[index - 1].field_18 == 0)
	{
		added->field_18 = 1;
	}

	*point_out = point;
	*properties_out = added;
	return ids.allocate((unsigned char)index);
}

// frees the id, closes the properties up over the point's (the points are
// only counted down), and moves the ids after it one slot down
// @retail 0x31bc90
void contact_constraint_31c000::remove_point(unsigned short id)
{
	long index = ids.data[id];
	ids.data[id] = 0xff;
	long count = properties.size;
	constraint_info_31be80 info;
	info.field_0 = 0;
	info.field_4 = 0;
	info.field_8 = 0;
	info.field_c = 0;
	if (count == 2)
	{
		info.field_8 = 4;
		info.field_4 = 0x20;
		info.field_c = 1;
	}
	count--;
	for (long i = index; i < count; i++)
	{
		properties[i] = properties[i + 1];
	}
	properties[index].field_18 = 0;
	properties.size = count;
	points.size--;
	for (long i = ids.size - 1; i >= 0; i--)
	{
		if (ids.data[i] != 0xff && ids.data[i] > index)
		{
			ids.data[i]--;
		}
	}
	info.field_4 += 0x30;
	info.field_8 += 4;
	info.field_c += 1;
	owner->remove_constraint_info(this, &info);
	flags |= 5;
}

// ---------------------------------------------------------------------------
// the full contact manager

// a contact point added: the world's and the entities' listeners may set the
// status to 1 to reject it
struct contact_added_event_318c10
{
	unsigned short id;
	unsigned short unknown2;
	void *manager;
	collidable_318c10 const *collidable_a;
	collidable_318c10 const *collidable_b;
	void *world;
	contact_point_318c10 *point;
	contact_properties_318c10 *properties;
	float projected_velocity;
	long status;
};

struct contact_removed_event_318e60
{
	unsigned short id;
	unsigned short unknown2;
	void *manager;
	entity_318c10 *entity_a;
	entity_318c10 *entity_b;
	void *world;
};

struct contact_processed_event_318fb0
{
	void *manager;
	collidable_318c10 const *collidable_a;
	collidable_318c10 const *collidable_b;
	process_output_318fb0 *output;
	void *world;
};

// the manager's own slots, called through its vtable
class contact_mgr_slots_319110
{
public:
	virtual void *slot_0(unsigned int flags) = 0;
	virtual unsigned short add_contact_point(collidable_318c10 const *a, collidable_318c10 const *b,
		process_input_318c10 const *input, contact_point_318c10 const *point) = 0;
	virtual void remove_contact_point(unsigned short id) = 0;
};

class contact_mgr_319110
{
public:
	REFERENCED_NEW_2D88A0(0x1d)

	contact_mgr_319110(void *world, entity_318c10 *entity_a, entity_318c10 *entity_b);

	unsigned short add_contact_point(collidable_318c10 const *a, collidable_318c10 const *b,
		process_input_318c10 const *input, contact_point_318c10 const *point);
	void remove_contact_point(unsigned short id);
	void process_contact(collidable_318c10 const *a, collidable_318c10 const *b, void *input,
		process_output_318fb0 *output);
	void cleanup();
	contact_properties_318c10 *get_contact_point_properties(unsigned short id);
	contact_point_318c10 *get_contact_point(unsigned short id);

	void const *vtable;
	unsigned short memory_size;
	unsigned short reference_count;
	void *world;
	// counts process calls down to the next process-contact event
	long process_countdown;
	contact_constraint_31c000 constraint;
};

typedef char assert_contact_mgr_319110_size[sizeof(contact_mgr_319110) == 0x120 ? 1 : -1];

// @retail 0x319110
contact_mgr_319110::contact_mgr_319110(void *world_, entity_318c10 *entity_a, entity_318c10 *entity_b)
{
	vtable = (void const *)0x415074;
	world = world_;
	constraint.set_entity_a(entity_a);
	constraint.set_entity_b(entity_b);
	process_countdown = 1;
}

// at most 250 points; the first one adds the constraint to the world. The
// point's friction and restitution are the geometric means of the two
// entities' materials; the listeners hear of it (and may reject it); then
// the properties take the restitution's part of the closing speed along the
// normal
// @retail 0x318c10
unsigned short contact_mgr_319110::add_contact_point(collidable_318c10 const *a, collidable_318c10 const *b,
	process_input_318c10 const *input, contact_point_318c10 const *point)
{
	if (constraint.properties.size > 250)
	{
		return 0xffff;
	}
	entity_318c10 *entity_a = root_318c10(a)->owner;
	entity_318c10 *entity_b = root_318c10(b)->owner;
	if (constraint.properties.size == 0)
	{
		((s_1d1541 *)world)->function_30d2b0(&constraint);
	}
	contact_point_318c10 *stored;
	contact_properties_318c10 *properties;
	unsigned short id = (unsigned short)constraint.add_point(&stored, &properties);

	hkVector4 velocity_a;
	hkVector4 velocity_b;
	entity_a->motion->getPointVelocity(*(hkVector4 const *)&point->position, velocity_a);
	entity_b->motion->getPointVelocity(*(hkVector4 const *)&point->position, velocity_b);
	__m128 const along = _mm_mul_ps(_mm_sub_ps(velocity_b.m_quad, velocity_a.m_quad), point->normal);
	float projected_velocity;
	_mm_store_ss(&projected_velocity, _mm_add_ss(_mm_shuffle_ps(along, along, 0xaa),
		_mm_add_ss(_mm_shuffle_ps(along, along, 0x55), along)));

	properties->friction = __builtin_sqrtf(entity_b->material.friction * entity_a->material.friction);
	properties->restitution = __builtin_sqrtf(entity_b->material.restitution * entity_a->material.restitution);
	*stored = *point;

	contact_added_event_318c10 event;
	event.id = id;
	event.manager = this;
	event.collidable_a = a;
	event.collidable_b = b;
	event.point = stored;
	event.properties = properties;
	event.projected_velocity = projected_velocity;
	event.status = 0;
	function_318320((listener_world_318320 *)world, (event_315d20 *)&event);
	if (entity_a->listeners_60.size)
	{
		function_315d20((listener_world_315d20 *)entity_a, (event_315d20 *)&event);
	}
	if (entity_b->listeners_60.size)
	{
		function_315d20((listener_world_315d20 *)entity_b, (event_315d20 *)&event);
	}
	if (event.status == 1)
	{
		((contact_mgr_slots_319110 *)this)->remove_contact_point(id);
		return 0xffff;
	}

	properties->field_4 = properties->restitution * input->field_30 * projected_velocity * 1.3f; // 0x45dff0
	properties->field_14 = properties->field_4 + point->position.m128_f32[3];
	float const inverse_masses = entity_b->motion->m_inertia_and_mass_inverse.m_quad.m128_f32[3]
		+ entity_a->motion->m_inertia_and_mass_inverse.m_quad.m128_f32[3]
		+ 1e-10f; // 0x415068
	properties->impulse_limit = 1.0f / inverse_masses * (properties->restitution + 1.0f)
		* projected_velocity * 0.2f; // 0x45dc5c
	return id;
}

// the listeners hear of it first; the last point gone, the world lets the
// constraint go (collecting nothing meanwhile)
// @retail 0x318e60
void contact_mgr_319110::remove_contact_point(unsigned short id)
{
	entity_318c10 *entity_a = constraint.entity_a;
	entity_318c10 *entity_b = constraint.entity_b;
	contact_removed_event_318e60 event;
	event.id = id;
	event.manager = this;
	event.entity_a = entity_a;
	event.entity_b = entity_b;
	function_3183b0((listener_world_318320 *)world, (event_315d20 *)&event);
	if (entity_a->listeners_60.size)
	{
		function_315d90((listener_world_315d20 *)entity_a, (event_315d20 *)&event);
	}
	if (entity_b->listeners_60.size)
	{
		function_315d90((listener_world_315d20 *)entity_b, (event_315d20 *)&event);
	}
	constraint.remove_point(id);
	if (constraint.properties.size == 0)
	{
		c_1d1261 *owner = (c_1d1261 *)world;
		bool const holding = owner->holding_entries;
		owner->holding_entries = false;
		owner->function_30ca30(&constraint);
		owner->holding_entries = holding;
	}
}

// the processed points' new positions and normals; every so many calls (the
// smaller of the two entities' delays, plus one) the listeners hear of it
// @retail 0x318fb0
void contact_mgr_319110::process_contact(collidable_318c10 const *a, collidable_318c10 const *b, void *input,
	process_output_318fb0 *output)
{
	processed_points_318fb0 *processed = output->points;
	processed_point_318fb0 *entry = processed->data;
	for (long i = processed->size; i > 0; i--, entry++)
	{
		*get_contact_point(entry->id) = entry->point;
	}
	if (--process_countdown != 0)
	{
		return;
	}
	entity_318c10 *entity_a = a->owner;
	entity_318c10 *entity_b = b->owner;
	unsigned short delay = entity_a->process_delay < entity_b->process_delay
		? entity_a->process_delay : entity_b->process_delay;
	process_countdown = delay + 1;
	contact_processed_event_318fb0 event;
	event.manager = this;
	event.collidable_a = a;
	event.collidable_b = b;
	event.output = output;
	function_318440((listener_world_318320 *)world, (event_315d20 *)&event);
	if (entity_a->listeners_60.size)
	{
		function_315e00((listener_world_315d20 *)entity_a, (event_315d20 *)&event);
	}
	if (entity_b->listeners_60.size)
	{
		function_315e00((listener_world_315d20 *)entity_b, (event_315d20 *)&event);
	}
}

// @retail 0x318a80
void contact_mgr_319110::cleanup()
{
	delete (reference_base_318970 *)this;
}

// @retail 0x318f30
contact_properties_318c10 *contact_mgr_319110::get_contact_point_properties(unsigned short id)
{
	return &constraint.properties[constraint.ids.data[id]];
}

// @retail 0x318f10
contact_point_318c10 *contact_mgr_319110::get_contact_point(unsigned short id)
{
	return &constraint.points[constraint.ids.data[id]];
}

// ---------------------------------------------------------------------------
// the factories

// the small manager (vtable 0x415040; its slots 0x318a90, 0x318980 and
// 0x3189f0 are not recovered)
struct contact_mgr_318a90
{
	REFERENCED_NEW_2D88A0(0x1d)

	void const *vtable;
	unsigned short memory_size;
	unsigned short reference_count;
	void *world;
	entity_318c10 *entity_a;
	entity_318c10 *entity_b;
	long process_countdown;
};

// a factory: a reference-counted object keeping the world it was made for
class contact_mgr_factory_318bb0
{
public:
	void const *vtable;
	unsigned short memory_size;
	unsigned short reference_count;
	void *world;

	contact_mgr_318a90 *create_small_contact_mgr(collidable_318c10 const *a, collidable_318c10 const *b,
		void const *input);
	contact_mgr_319110 *create_contact_mgr(collidable_318c10 const *a, collidable_318c10 const *b,
		void const *input);
};

// @retail 0x318bb0
contact_mgr_318a90 *contact_mgr_factory_318bb0::create_small_contact_mgr(collidable_318c10 const *a,
	collidable_318c10 const *b, void const *input)
{
	entity_318c10 *entity_a = a->owner;
	entity_318c10 *entity_b = b->owner;
	contact_mgr_318a90 *manager = new contact_mgr_318a90;
	manager->vtable = (void const *)0x415040;
	manager->world = world;
	unsigned short delay = entity_a->process_delay < entity_b->process_delay
		? entity_a->process_delay : entity_b->process_delay;
	manager->entity_b = entity_b;
	manager->entity_a = entity_a;
	manager->process_countdown = delay + 1;
	return manager;
}

// @retail 0x3191c0
contact_mgr_319110 *contact_mgr_factory_318bb0::create_contact_mgr(collidable_318c10 const *a,
	collidable_318c10 const *b, void const *input)
{
	return new contact_mgr_319110(world, a->owner, b->owner);
}

// ---------------------------------------------------------------------------
// the collision filters

class collision_filter_319230
{
public:
	void function_319230(bool *result, collidable_318c10 const *a, collidable_318c10 const *b);
	void function_3192a0(bool *result, long arg_1);
};

// whether either entity is fixed or has a motion of kind 6 (keyframed)
// @retail 0x319230
void collision_filter_319230::function_319230(bool *result, collidable_318c10 const *a, collidable_318c10 const *b)
{
	entity_318c10 *entity_a = a->owner;
	entity_318c10 *entity_b = b->owner;
	*result = entity_a->fixed || entity_b->fixed
		|| entity_a->motion->kind_code() == 6 || entity_b->motion->kind_code() == 6;
}

// @retail 0x3192a0
void collision_filter_319230::function_3192a0(bool *result, long arg_1)
{
	*result = false;
}
