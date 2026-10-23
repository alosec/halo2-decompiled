// @flags /O2 /GL-
// Havok (third-party physics library): whether a dispatcher's table holds
// the creation function at 0x2dae40 for a pair of kinds, or else the one the
// test at 0x2f98a0 looks for.
#include "agent_table_2e5d50.h"
#include "convex_pair_2ee2e0.h"
#include "timer_mark_2ebc80.h"

struct result_2f98a0
{
	char value;

	result_2f98a0(bool value_) : value((char)value_) {}
};

typedef void *(__cdecl *create_function_2f98a0)(long arg_0, long arg_1, long arg_2, long value);

void *__cdecl function_2dae40(long arg_0, long arg_1, long arg_2, long value);

struct dispatcher_2f98a0
{
	char unknown0[0x18c];
	create_function_2f98a0 functions[32][32];
};

result_2f98a0 __cdecl function_2f98a0(dispatcher_2f98a0 const *dispatcher, long first, long second);

// the test the pair table uses for whether a pair has a handler; registering
// the handlers below puts the one above in place
typedef result_2f98a0 (__cdecl *pair_test_2daf00)(dispatcher_2f98a0 const *dispatcher, long first, long second);
pair_test_2daf00 g_461f0c = function_2f98a0;

// @retail 0x2daec0
result_2f98a0 __cdecl function_2daec0(dispatcher_2f98a0 const *dispatcher, long first, long second)
{
	if (dispatcher->functions[first][second] == function_2dae40)
	{
		return true;
	}
	return function_2f98a0(dispatcher, first, second);
}

void __cdecl function_2ee500(void const *a, void const *b, void const *input, void *collector);
void __cdecl function_2ee8b0(void const *a, void const *b, void const *input, void *collector);
void __cdecl function_2fe830(void const *a, void const *b, void const *input, void *collector, void *second_collector);

// registers handlers for pairs of shape types, each pair both ways round
// where the pair differs
// @retail 0x2daf00
void __cdecl function_2daf00(s_1c3a40 *table)
{
	handlers_2e5d50 handlers;
	handlers.handler[0] = (handler_2e5d50)function_2dae40;
	handlers.handler[1] = (handler_2e5d50)function_2ee500;
	handlers.handler[2] = (handler_2e5d50)function_2ee8b0;
	handlers.handler[3] = (handler_2e5d50)function_2fe830;
	table->function_2e5d50(&handlers, 23, 1);
	table->function_2e5d50(&handlers, 1, 23);
	table->function_2e5d50(&handlers, 23, 23);
	g_461f0c = function_2daec0;
}

class hkEntity;
bool __cdecl function_279670(hkEntity *entity);

struct motion_2daf70
{
	char unknown0[0x40];
	__m128 velocity;
};

struct entity_2daf70
{
	char unknown0[0x3c];
	motion_2daf70 const *motion;
};

// the side at the top of a chain of parents, which knows its entity
struct root_2daf70
{
	body_2ee3f0 body;
	char unknown10[0x20 - 0x10];
	entity_2daf70 *entity;
};

struct input_2daf70
{
	long field_0;
	long field_4;
	float tolerance;
	bool *keep_separation;
	float step;
};

// a vector of four copies of a float
inline __m128 splat_2daf70(float value)
{
	__m128 const single = _mm_set_ss(value);
	return _mm_shuffle_ps(single, single, 0);
}

// a vector's squared length
__forceinline float length_squared_2daf70(__m128 const &v)
{
	__m128 const squares = _mm_mul_ps(v, v);
	float result;
	_mm_store_ss(&result, _mm_add_ss(_mm_shuffle_ps(squares, squares, 0xaa), _mm_add_ss(_mm_shuffle_ps(squares, squares, 0x55), squares)));
	return result;
}

// a vector that can be set to a direction taken into a transform's frame, and
// scaled
struct vector_2daf70
{
	__m128 value;

	__forceinline void rotate_inverse(c_query_transform const *matrix, __m128 const &direction)
	{
		__m128 c0 = matrix->axes[0];
		__m128 c1 = matrix->axes[1];
		__m128 c2 = matrix->axes[2];
		__m128 t0 = _mm_shuffle_ps(c0, c1, 0x44);
		__m128 t2 = _mm_shuffle_ps(c0, c1, 0xee);
		__m128 t1 = _mm_shuffle_ps(c2, c2, 0x44);
		__m128 t3 = _mm_shuffle_ps(c2, c2, 0xee);
		__m128 row_0 = _mm_shuffle_ps(t0, t1, 0x88);
		__m128 row_1 = _mm_shuffle_ps(t0, t1, 0xdd);
		__m128 row_2 = _mm_shuffle_ps(t2, t3, 0x88);
		value = _mm_add_ps(_mm_add_ps(
			_mm_mul_ps(row_0, _mm_shuffle_ps(direction, direction, 0x00)),
			_mm_mul_ps(row_1, _mm_shuffle_ps(direction, direction, 0x55))),
			_mm_mul_ps(row_2, _mm_shuffle_ps(direction, direction, 0xaa)));
	}

	__forceinline void scale(float factor)
	{
		value = _mm_mul_ps(splat_2daf70(factor), value);
	}
};
// a shape that moves on its own: its velocity in its frame, and the squared
// speed below which it counts as still
struct moving_shape_2daf70
{
	char unknown0[0x10];
	vector_2daf70 velocity;
	float threshold;
};

// the pair kept for later, for shapes that may move on their own: notes which
// sides move, starts the search state again when that changes, and queries
// the pair without keeping a separating direction meanwhile
class moving_pair_2daf70 : public convex_pair_2ee2e0
{
public:
	long moving;

	found_2ee610 function_2daf70(body_2ee3f0 const *a, body_2ee3f0 const *b, void const *input, kept_contact_2ee610 *out);
	void function_2db2b0(body_2ee3f0 const *a, body_2ee3f0 const *b, void const *input, void *out);
};

// @retail 0x2daf70
found_2ee610 moving_pair_2daf70::function_2daf70(body_2ee3f0 const *a, body_2ee3f0 const *b, void const *input_, kept_contact_2ee610 *out)
{
	input_2daf70 const *input = (input_2daf70 const *)input_;
	found_2ee610 changed(false);
	bool const keep_separation = *input->keep_separation;
	long now_moving = 0;
	if (a->shape->type() == 23)
	{
		moving_shape_2daf70 *shape = (moving_shape_2daf70 *)a->shape;
		body_2ee3f0 const *root = a;
		while (root->parent)
		{
			root = root->parent;
		}
		entity_2daf70 *entity = ((root_2daf70 const *)root)->entity;
		if (length_squared_2daf70(entity->motion->velocity) > shape->threshold && function_279670((hkEntity *)entity))
		{
			shape->velocity.rotate_inverse(&a->placement->transform, entity->motion->velocity);
			shape->velocity.scale(input->step);
			now_moving = 1;
			changed.value = true;
		}
		else
		{
			shape->velocity.value = _mm_setzero_ps();
		}
	}
	if (b->shape->type() == 23)
	{
		moving_shape_2daf70 *shape = (moving_shape_2daf70 *)b->shape;
		body_2ee3f0 const *root = b;
		while (root->parent)
		{
			root = root->parent;
		}
		entity_2daf70 *entity = ((root_2daf70 const *)root)->entity;
		if (length_squared_2daf70(entity->motion->velocity) > shape->threshold && function_279670((hkEntity *)entity))
		{
			shape->velocity.rotate_inverse(&b->placement->transform, entity->motion->velocity);
			shape->velocity.scale(input->step);
			now_moving += 2;
			changed.value = true;
		}
		else
		{
			shape->velocity.value = _mm_setzero_ps();
		}
	}
	if (now_moving != moving)
	{
		*input->keep_separation = false;
	}
	else if (!changed.value)
	{
		goto query;
	}
	{
		convex_2fefd0 const *first = a->shape;
		convex_2fefd0 const *second = b->shape;
		c_query_transform transform;
		transform.inverse_product(&a->placement->transform, &b->placement->transform);
		if (second->type() == 5)
		{
			state.function_2ff0c0(first, (triangle_2ff0c0 const *)second, &transform);
		}
		else
		{
			state.start(first, second, &transform);
		}
		separation.m128_f32[3] = -1.0f;
	}
query:
	found_2ee610 found = function_2ee610(a, b, input, out);
	moving = now_moving;
	*input->keep_separation = keep_separation;
	return found;
}

// the same for a query that brings the pair's kept contact points up to date
// @retail 0x2db2b0
void moving_pair_2daf70::function_2db2b0(body_2ee3f0 const *a, body_2ee3f0 const *b, void const *input_, void *out)
{
	input_2daf70 const *input = (input_2daf70 const *)input_;
	timer_add_2ebc80("TtConvexSweep");
	found_2ee610 changed(false);
	bool const keep_separation = *input->keep_separation;
	long now_moving = 0;
	if (a->shape->type() == 23)
	{
		moving_shape_2daf70 *shape = (moving_shape_2daf70 *)a->shape;
		body_2ee3f0 const *root = a;
		while (root->parent)
		{
			root = root->parent;
		}
		entity_2daf70 *entity = ((root_2daf70 const *)root)->entity;
		if (length_squared_2daf70(entity->motion->velocity) > shape->threshold && function_279670((hkEntity *)entity))
		{
			shape->velocity.rotate_inverse(&a->placement->transform, entity->motion->velocity);
			shape->velocity.scale(input->step);
			now_moving = 1;
			changed.value = true;
		}
		else
		{
			shape->velocity.value = _mm_setzero_ps();
		}
	}
	if (b->shape->type() == 23)
	{
		moving_shape_2daf70 *shape = (moving_shape_2daf70 *)b->shape;
		body_2ee3f0 const *root = b;
		while (root->parent)
		{
			root = root->parent;
		}
		entity_2daf70 *entity = ((root_2daf70 const *)root)->entity;
		if (length_squared_2daf70(entity->motion->velocity) > shape->threshold && function_279670((hkEntity *)entity))
		{
			shape->velocity.rotate_inverse(&b->placement->transform, entity->motion->velocity);
			shape->velocity.scale(input->step);
			now_moving += 2;
			changed.value = true;
		}
		else
		{
			shape->velocity.value = _mm_setzero_ps();
		}
	}
	if (now_moving != moving)
	{
		*input->keep_separation = false;
	}
	else if (!changed.value)
	{
		goto query;
	}
	{
		convex_2fefd0 const *first = a->shape;
		convex_2fefd0 const *second = b->shape;
		c_query_transform transform;
		transform.inverse_product(&a->placement->transform, &b->placement->transform);
		if (second->type() == 5)
		{
			state.function_2ff0c0(first, (triangle_2ff0c0 const *)second, &transform);
		}
		else
		{
			state.start(first, second, &transform);
		}
		separation.m128_f32[3] = -1.0f;
	}
query:
	function_2eeb40(a, b, input, out);
	moving = now_moving;
	*input->keep_separation = keep_separation;
	timer_add_2ebc80("Et");
}
