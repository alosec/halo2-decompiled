// @flags /O2 /GL-
// Havok (third-party physics library): an entity in a world: waking its
// island, queueing it for the world, and dropping one of its listeners.
#include "reference_counted_2d88a0.h"

class c_havok_body_group
{
public:
	char unknown0[0x24];
	long index;
	char unknown28;
	bool active;
};

class hkWorld
{
public:
	void function_30cc20(void *pending);
	void function_30ccd0(c_havok_body_group *island);
	void function_30cd50(void *pending);
};

class class_30c560
{
public:
	void function_30c560(referenced_2d88a0 *entry);
};

class hkRigidBody
{
public:
	char unknown0[8];
	hkWorld *m_world;
	char unknown0c[0x44 - 0xc];
	c_havok_body_group *m_simulation_island;

	void activate(void);
};

// a byte-sized truth value, returned by value
struct flag_30bc90
{
	bool value;
};

class c_physics_body_island_view
{
public:
	char unknown0[8];
	hkWorld *m_world;
	char unknown0c[0x44 - 0xc];
	c_havok_body_group *m_simulation_island;

	flag_30bc90 island_active() const
	{
		flag_30bc90 result;
		result.value = m_simulation_island->active;
		return result;
	}

	void activate(void);
};

#include "lane_b_constraint_entities.h"


// @retail 0x30bc60
void hkRigidBody::activate(void)
{
	if (m_world)
	{
		((class_30c560 *)m_world)->function_30c560((referenced_2d88a0 *)this);
		c_havok_body_group *island = m_simulation_island;
		if (!island->active && island->index != -1)
		{
			m_world->function_30ccd0(island);
		}
	}
}

// @retail 0x30bc90
void c_physics_body_island_view::activate(void)
{
	hkWorld *world = m_world;
	if (world)
	{
		if (island_active().value && m_simulation_island->index != -1)
		{
			world->function_30cc20(this);
		}
	}
}

// clears the listener's slot, and hands the entity to the world's pending
// list
// @retail 0x30bcc0
void entity_30bcc0::function_30bcc0(void *listener)
{
	listeners[index_of(listener)] = 0;
	if (m_world)
	{
		m_world->function_30cd50(this);
	}
}
