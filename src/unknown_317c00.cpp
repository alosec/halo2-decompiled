// @flags /O2 /GL- /GX
// Havok (third-party physics library): the narrow phase over a list of
// pairs: every pair with an agent is asked for its contacts into a scratch
// array, and when it finds any they are passed on to the agent's handler;
// bracketed by two marks in the timing stream.
#include "reference_counted_2d88a0.h"
#include "timer_mark_2ebc80.h"

__forceinline void timer_add_317c00(char const *label)
{
	timer_mark_2ebc80 *mark = (timer_mark_2ebc80 *)g_480154;
	bool const room = (char *)mark < g_480158;
	if (room)
	{
		mark->label = label;
		unsigned long time;
		__asm
		{
			rdtsc
			mov time, eax
		}
		mark->time = time;
		g_480154 = (char *)(mark + 1);
	}
}

struct element_317060
{
	char bytes[0x30];
};

// an array of 48-byte elements whose storage comes from the allocator's
// stack area (src/unknown_317060.cpp)
struct array_317060
{
	element_317060 *data;
	long size;
	unsigned long capacity_and_flags;

	array_317060() : data(0), size(0), capacity_and_flags(0x80000000) {}
	~array_317060()
	{
		if ((capacity_and_flags & 0x80000000) == 0)
		{
			g_memory_2d88a0->release_block(data, (capacity_and_flags & 0x7fffffff) * sizeof(element_317060), 0x12);
		}
	}
};

struct stack_array_317060 : array_317060
{
	element_317060 *stack_block;
	long stack_capacity;

	stack_array_317060(long capacity);
	~stack_array_317060();
};

// where a pair's contacts are gathered (16-byte aligned)
struct contacts_317c00
{
	array_317060 *results;
};

struct object_317c00;

class handler_317c00
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	// the contacts found for a pair
	virtual void handle(object_317c00 *a, object_317c00 *b, void const *input, contacts_317c00 *contacts) {}
};

class agent_317c00
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	// the pair's contacts
	virtual void find_contacts(object_317c00 *a, object_317c00 *b, void const *input, contacts_317c00 *contacts) {}

	long field_4;
	handler_317c00 *handler;
};

// a pair: the two objects (each held by its part at +0x10) and their agent
struct pair_317c00
{
	char *a;
	char *b;
	agent_317c00 *agent;
	long field_c;
};

// the world the narrow phase belongs to, with a flag at +0x23e that picks
// which of two ways it runs
struct world_317d30
{
	char unknown0[0x23e];
	bool flag_23e;
};

class narrow_phase_317c00
{
public:
	char unknown0[0x20];
	world_317d30 *world;
	char unknown24[0x68 - 0x24];
	pair_317c00 *pairs;
	long pair_count;

	void function_3175e0(void const *step, void const *input);
	void function_317950(void const *input);
	void function_317c00(void const *input);
	void function_317d30(void const *step, void const *input);
};

// @retail 0x317c00
void narrow_phase_317c00::function_317c00(void const *input)
{
	timer_add_317c00("TtNarrowPhase");
	{
		__declspec(align(16)) contacts_317c00 contacts;
		stack_array_317060 results(0x40);
		pair_317c00 *pair = pairs;
		int const count = pair_count;
		contacts.results = &results;
		for (int index = count - 1; index >= 0; index--, pair++)
		{
			if (pair->agent != 0)
			{
				contacts.results->size = 0;
				object_317c00 *a = (object_317c00 *)(pair->a - 0x10);
				object_317c00 *b = (object_317c00 *)(pair->b - 0x10);
				pair->agent->find_contacts(a, b, input, &contacts);
				if (contacts.results->size != 0)
				{
					pair->agent->handler->handle(a, b, input, &contacts);
				}
			}
		}
	}
	timer_add_317c00("Et");
}

struct s_278e80_state;
void __cdecl function_278e60(void *context);
void __cdecl function_278e80(s_278e80_state *state);

// runs a step: the step's setup, then the narrow phase one of two ways as
// the world's flag says, all inside the object's guard
// @retail 0x317d30
void narrow_phase_317c00::function_317d30(void const *step, void const *input)
{
	function_278e60(this);
	function_3175e0(step, input);
	if (world->flag_23e)
	{
		function_317950(input);
	}
	else
	{
		function_317c00(input);
	}
	function_278e80((s_278e80_state *)this);
}
