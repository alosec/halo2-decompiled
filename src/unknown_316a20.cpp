// @flags /O2 /GL-
// Havok (third-party physics library): a step of the simulation: the step's
// listeners are told first; then each island, last first, is integrated and
// told of the step, between two timing marks, unless the work is handed to
// 0x31c100.
#include "timer_mark_2ebc80.h"

// the step as the islands see it
struct step_316a20
{
	char unknown0[0x10];
	char field_10[0x10];
	char field_20[0x30];
	char field_50[0x10];
};

class step_listener_316a20
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void slot_4() {}
	// told of the step before the islands
	virtual void slot_5(void *step) {}
};

// what each island holds at 0x3c: integrated (slot 2), then told of the step
// (slot 1)
class integrated_316a20
{
public:
	virtual void slot_0() {}
	virtual void slot_1(void *step) {}
	virtual void slot_2(void *step, void *world_info) {}
};

struct island_316a20
{
	char unknown0[0x3c];
	integrated_316a20 *integrated;
};

struct world_316a20
{
	char unknown0[0xe0];
	char field_e0[0x10];
};

// the step's listeners
struct step_listeners_316a20
{
	step_listener_316a20 **data;
	long size;
	long capacity_and_flags;

	step_listener_316a20 **begin() { return data; }
	step_listener_316a20 **end() { return data + size; }
};

void __cdecl function_31c100(void *step_10, void *step_20, void *step_50, void *world_info,
	void *simulation, void *islands, long island_count, long field_58, long field_5c);

class simulation_316a20
{
public:
	char unknown0[0x20];
	world_316a20 *world;
	char unknown24[0x3c - 0x24];
	island_316a20 **islands;
	long island_count;
	char unknown44[0x4c - 0x44];
	step_listeners_316a20 listeners;
	long field_58;
	long field_5c;

	void function_316a20(step_316a20 *step);
};

// @retail 0x316a20
void simulation_316a20::function_316a20(step_316a20 *step)
{
	step_listener_316a20 **listener = listeners.begin();
	step_listener_316a20 **end = listeners.end();
	for (; listener != end; listener++)
	{
		(*listener)->slot_5(step->field_10);
	}
	if (field_5c == 0)
	{
		timer_add_2ebc80("TtIntegrate");
		for (int i = island_count - 1; i >= 0; i--)
		{
			integrated_316a20 *integrated = islands[i]->integrated;
			integrated->slot_2(step->field_10, world->field_e0);
			integrated->slot_1(step->field_10);
		}
		timer_add_2ebc80("Et");
	}
	else
	{
		function_31c100(step->field_10, step->field_20, step->field_50, world->field_e0, this, islands,
			island_count, field_58, field_5c);
	}
}
