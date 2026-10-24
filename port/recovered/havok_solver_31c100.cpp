// @flags /O2 /GL-
// Havok (third-party physics library): the simulation's step handed to the
// constraint solver (0x31c100, called from 0x316a20 when the simulation has
// constraints to solve).
//
// The solver works in one scratch area laid out as
//   base           a byte 2, the rest of the first 0x80 bytes cleared
//   base + 0x80    each island's integrated bodies, written by slot 3 of the
//                  object each island holds at 0x3c (which records its offset
//                  from base at its field 8), then a byte 3
//   base + 0x90 + bodies_size
//                  the constraints' Jacobians (jacobians_size bytes)
//   then           one word per Jacobian plus two (cleared before solving)
//   then           the constraints' schemas (schemas_size bytes plus the end
//                  word 0x400)
// The sizes are those the constraints have told the simulation of (fields
// 8 to 0x18). The area is taken from the Havok buffer singleton (g_484b38,
// flagged in use meanwhile); what does not fit takes one block from the top
// of the Havok memory's stack area. When only the Jacobians overflow, the
// writer the constraints build into (slot 9 of each) switches to that block
// once it passes the buffer's end less the largest Jacobian
// (max_jacobian_size), leaving room for one more there.
//
// After the solver (0x31c940, not recovered) reports success, slot 4 of each
// island's integrated object reads its bodies' results back.
#include "reference_counted_2d88a0.h"
#include "timer_mark_2ebc80.h"

// the solver itself (0x31c940, not recovered): the step's solver info, the
// schemas, the area's base and the per-Jacobian words; returns 1 when solved
extern "C" int h2_retail_31c940(void *solver_info, void *schemas, void *base, void *accumulators);

// the Havok buffer singleton (port/recovered/havok_base_system.cpp)
struct buffer_view_31c100
{
	void const *vtable;
	long field_4;
	char *cursor;
	long capacity;
	bool in_use;
};
extern buffer_view_31c100 *g_484b38;

// the object an island (and an entity) holds at 0x3c: its bodies' place in
// the scratch area, written and read back through slots 3 and 4
class integrated_31c100
{
public:
	virtual void slot_0() = 0;
	virtual void slot_1() = 0;
	virtual void slot_2() = 0;
	virtual char *write_bodies(void *step_info, void *world_info, char *out) = 0;
	// scale: the solver info's float at 0x20 (retail pushes the dword; the
	// motions' slot 4, e.g. 0x31a8c0, scale the solved velocities by it)
	virtual char *read_bodies(void *step_info, float scale, char *in) = 0;

	long field_4;
	// the offset of the bodies from the area's base
	long offset;
};

struct holder_31c100
{
	char unknown0[0x3c];
	integrated_31c100 *integrated;
};

// where a constraint's Jacobians and schemas go
struct jacobian_writer_31c100
{
	char *jacobians;
	char *schemas;
	// past this the Jacobians move to the overflow block (-1 then)
	char *jacobians_limit;
	char *overflow;
};

// the two bodies a constraint links, for its slot 9
struct constraint_bodies_31c100
{
	char *body_a;
	char *body_b;
};

class constraint_31c100
{
public:
	virtual void slot_0() = 0;
	virtual void slot_1() = 0;
	virtual void slot_2() = 0;
	virtual void slot_3() = 0;
	virtual void slot_4() = 0;
	virtual void slot_5() = 0;
	virtual void slot_6() = 0;
	virtual void slot_7() = 0;
	virtual void slot_8() = 0;
	virtual void build_jacobians(constraint_bodies_31c100 *bodies, jacobian_writer_31c100 *writer) = 0;

	long field_4;
	long field_8;
	holder_31c100 *entity_a;
	holder_31c100 *entity_b;
};

// the simulation's solver sizes
struct simulation_31c100
{
	char unknown0[0x8];
	long max_jacobian_size;
	long jacobians_size;
	long schemas_size;
	long bodies_size;
	long jacobian_count;
};

struct solver_info_31c100
{
	char unknown0[0x20];
	// the scale slot 4 of the integrated objects takes
	float results_scale;
};

static __forceinline char *stack_allocate_31c100(long size)
{
	return (char *)g_memory_2d88a0->allocate_stack((size + 0xf) & ~0xf);
}

// @retail 0x31c100
void __cdecl function_31c100(void *step_10, void *step_20, void *step_50, void *world_info,
	void *simulation_in, void *islands_in, long island_count, long constraints_in, long constraint_count)
{
	simulation_31c100 *simulation = (simulation_31c100 *)simulation_in;
	holder_31c100 **islands = (holder_31c100 **)islands_in;
	constraint_31c100 **constraints = (constraint_31c100 **)constraints_in;
	solver_info_31c100 *solver_info = (solver_info_31c100 *)step_20;
	constraint_bodies_31c100 *bodies = (constraint_bodies_31c100 *)step_50;

	buffer_view_31c100 *buffer = g_484b38;
	buffer->in_use = true;
	char *const buffer_start = buffer->cursor;
	char *end = buffer->cursor + buffer->capacity;
	long const accumulators_size = simulation->jacobian_count * 4 + 8;
	char *block = 0;
	char *overflow = 0;

	char *base = buffer_start;
	char *jacobians = base + simulation->bodies_size + 0x90;
	char *jacobians_limit = jacobians + simulation->jacobians_size;
	char *accumulators = jacobians_limit;
	char *schemas;
	for (;;)
	{
		schemas = accumulators + accumulators_size;
		char *const needed = schemas + simulation->schemas_size + 4;
		if (needed <= end)
		{
			break;
		}
		if (jacobians >= end)
		{
			// nothing past the bodies fits: everything goes in the block
			long const size = needed - buffer_start;
			block = stack_allocate_31c100(size);
			end = block + size;
			base = block;
			jacobians = base + simulation->bodies_size + 0x90;
			jacobians_limit = jacobians + simulation->jacobians_size;
			accumulators = jacobians_limit;
		}
		else if (jacobians_limit < end)
		{
			// the Jacobians fit: the words and the schemas go in the block
			long const size = simulation->schemas_size + accumulators_size + 4;
			block = stack_allocate_31c100(size);
			end = block + size;
			accumulators = block;
		}
		else
		{
			// the Jacobians overflow: the rest of them, with room for one
			// more past the limit, and all after them go in the block
			long const rest = simulation->max_jacobian_size * 2 - (long)end + simulation->jacobians_size + (long)jacobians;
			long const size = simulation->schemas_size + rest + accumulators_size + 4;
			block = stack_allocate_31c100(size);
			jacobians_limit = end - simulation->max_jacobian_size;
			end = block + size;
			overflow = block;
			accumulators = block + rest;
		}
	}

	base[0] = 2;
	for (int i = 0x10; i < 0x80; i++)
	{
		base[i] = 0;
	}
	char *const bodies_start = base + 0x80;
	char *cursor = bodies_start;
	for (holder_31c100 **island = islands; island < islands + island_count; island++)
	{
		integrated_31c100 *integrated = (*island)->integrated;
		long const offset = cursor - base;
		if (integrated->offset != offset)
		{
			integrated->offset = offset;
		}
		cursor = integrated->write_bodies(step_10, world_info, cursor);
	}
	*cursor = 3;

	jacobian_writer_31c100 writer;
	writer.jacobians = jacobians;
	writer.schemas = schemas;
	writer.jacobians_limit = jacobians_limit;
	writer.overflow = overflow;
	for (constraint_31c100 **constraint = constraints; constraint < constraints + constraint_count; constraint++)
	{
		bodies->body_a = base + (*constraint)->entity_a->integrated->offset;
		bodies->body_b = base + (*constraint)->entity_b->integrated->offset;
		if (writer.jacobians >= writer.jacobians_limit)
		{
			writer.jacobians = writer.overflow;
			writer.jacobians_limit = (char *)-1;
		}
		(*constraint)->build_jacobians(bodies, &writer);
	}
	*(long *)writer.schemas = 0x400;

	for (long i = 0; i < simulation->jacobian_count; i++)
	{
		((long *)accumulators)[i] = 0;
	}
	int const solved = h2_retail_31c940(step_20, schemas, base, accumulators);

	// the Jacobian count as a monitor value ("MiNumJacobians", 0x4153f4)
	if (g_480154 < g_480158)
	{
		char *mark = g_480154;
		*(char const **)mark = "MiNumJacobians";
		*(float *)(mark + 4) = (float)simulation->jacobian_count;
		g_480154 = mark + 8;
	}

	if (solved == 1)
	{
		float const results_scale = solver_info->results_scale;
		char *in = bodies_start;
		for (holder_31c100 **island = islands; island < islands + island_count; island++)
		{
			in = (*island)->integrated->read_bodies(step_10, results_scale, in);
		}
	}

	g_484b38->in_use = false;
	if (block)
	{
		g_memory_2d88a0->release_stack(block, (end - block + 0xf) & ~0xf);
	}
}
