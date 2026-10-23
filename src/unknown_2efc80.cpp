// @flags /O2 /GL-
// Havok (third-party physics library): two spheres reported as a pair when
// their centres are closer than the sum of their radii, as a method and as a
// free function, each bracketed by two marks in the timing stream.
#include "timer_mark_2ebc80.h"
#include <xmmintrin.h>
#include "agent_table_2e5d50.h"

struct sphere_2efc80
{
	char unknown0[0xc];
	float radius;
};

struct motion_2efc80
{
	char unknown0[0x50];
	__m128 position;
};

struct body_2efc80
{
	sphere_2efc80 const *shape;
	long key;
	motion_2efc80 const *motion;
};

class collector_2efc80
{
public:
	virtual void slot_0() {}
	virtual void add_pair(body_2efc80 const *a, body_2efc80 const *b) {}
};

class sphere_pair_2efc80
{
public:
	void function_2efc80(body_2efc80 const *a, body_2efc80 const *b, void const *input, collector_2efc80 *collector);
};

// whether two spheres overlap: the squared distance of their centres against
// the squared sum of their radii
__forceinline bool overlap_2efc80(body_2efc80 const *a, body_2efc80 const *b)
{
	__m128 const offset = _mm_sub_ps(b->motion->position, a->motion->position);
	__m128 const squares = _mm_mul_ps(offset, offset);
	float distance_squared;
	_mm_store_ss(&distance_squared, _mm_add_ss(_mm_shuffle_ps(squares, squares, 0xaa), _mm_add_ss(_mm_shuffle_ps(squares, squares, 0x55), squares)));
	float const radius = a->shape->radius + b->shape->radius;
	return distance_squared < radius * radius;
}

// @retail 0x2efc80
void sphere_pair_2efc80::function_2efc80(body_2efc80 const *a, body_2efc80 const *b, void const *input, collector_2efc80 *collector)
{
	timer_add_2ebc80("TtSphereSphere");
	if (overlap_2efc80(a, b))
	{
		collector->add_pair(a, b);
	}
	timer_add_2ebc80("Et");
}

// @retail 0x2efd50
void __cdecl function_2efd50(body_2efc80 const *a, body_2efc80 const *b, void const *input, collector_2efc80 *collector)
{
	timer_add_2ebc80("TtSphereSphere");
	if (overlap_2efc80(a, b))
	{
		collector->add_pair(a, b);
	}
	timer_add_2ebc80("Et");
}

struct body_2ef9a0;
struct body_2efc80;
class collector_2ef9a0;
class collector_2efc80;
class indexed_2ef970;
struct input_2ef9a0;
indexed_2ef970 *__cdecl function_2ef970(long arg_0, long arg_1, long arg_2, long value);
void __cdecl function_2efd50(body_2efc80 const *a, body_2efc80 const *b, void const *input, collector_2efc80 *collector);
void __cdecl function_2efb10(body_2ef9a0 const *a, body_2ef9a0 const *b, input_2ef9a0 const *input, collector_2ef9a0 *collector);
void __cdecl function_2fe830(void const *a, void const *b, void const *input, void *collector, void *second_collector);

// registers handlers for pairs of shape types, each pair both ways round
// where the pair differs
// @retail 0x2efe20
void __cdecl function_2efe20(s_1c3a40 *table)
{
	handlers_2e5d50 handlers;
	handlers.handler[0] = (handler_2e5d50)function_2ef970;
	handlers.handler[1] = (handler_2e5d50)function_2efd50;
	handlers.handler[2] = (handler_2e5d50)function_2efb10;
	handlers.handler[3] = (handler_2e5d50)function_2fe830;
	table->function_2e5d50(&handlers, 4, 4);
}
