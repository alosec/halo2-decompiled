#include "unknown_11c920.h"
#include "slot_handler.h"
#include "globals.h"
#include "unknown_1cec30.h"
#include "unknown_0259d0.h"
#include <new>
// @flags /O2 /arch:SSE /Gr

struct s_shape_state;
struct s_state_c570;
struct s_biped_physics_move;
struct s_biped_physics_output;
class c_z_shape_view
{
public:
 virtual void v0() = 0;
 virtual void v1() = 0;
 virtual void v2() = 0;
 virtual void v3() = 0;
 virtual void v4() = 0;
 virtual long v5() = 0;
};
#include "unknown_0a9f20.h"
struct s_shape_carrier_contact;
struct s_motion_adjustment_result;
struct s_motion_adjustment;
struct s_ease_input;
struct s_ease_output;
struct s_ease_state;
struct s_tracking_state;
struct s_tracking_output;
struct s_tracked_point;
struct s_contact_dispatch_output;
struct s_biped_physics_move;
struct s_havok_component;
extern real g_47f05c;
void __stdcall function_1f0450(s_shape_state *state, byte *out, s_shape_carrier_contact const *input);
void __stdcall function_1fa810(long *ticks, s_motion_adjustment_result *out, s_motion_adjustment const *input);
void function_1ec690(byte const *input, s_state_c570 *state, byte *output);
void function_1eb6f0(s_ease_input *input, s_ease_state *state, s_ease_output *output);
void function_1fc4b0(s_tracked_point *state, s_tracking_state const *input, s_tracking_output *output);
void function_1e5b50(byte const *input, s_contact_dispatch_output *output, byte const *state);
void function_b7740(long object_index, vector3f const *linear, vector3f const *angular, bool skip);
void function_1c4b00(long object_index, void *linear, void *angular, long force);
void __stdcall function_b9b90(long object_index, bool disable);
void function_b7360(long object_index);
void function_bba20(long object_index);
void __stdcall function_b87b0(long object_index);
void __stdcall function_b8600(long object_index, long mode);
void function_1c4a80(long object_index, bool first, bool second);
void function_b58c0(long object_index, dword mask);
bool function_1c5210(transform4x3f const *matrix, void *shape, long component, long filter);

static __forceinline long real_to_long(real value);

// @retail 0x1e55d0
void __stdcall function_1e55d0(s_biped_physics_move *move, void *physics, s_biped_physics_output *output)
{
	byte *out = (byte *)move;
	byte *state = (byte *)physics;
	byte const *input = (byte const *)output;
	*(long *)out = 0;
	*(vector3f *)(out + 0x24) = *(vector3f const *)(input + 0x100);
	*(vector3f *)(out + 0x18) = *(vector3f const *)(input + 0xf4);
	vector3f &velocity = *(vector3f *)(out + 0xc);
	velocity = *g_4687a4;
	*(long *)(out + 4) = 0;
	out[8] = false;
	out[0x54] = false;
	*(vector3f *)(out + 0x58) = *(vector3f *)g_468788;
	*(vector3f *)(out + 0x64) = *g_4687a4;
	if (state[1] > 0) --state[1];
	switch (*state)
	{
	case 1: function_1f0450((s_shape_state *)(state + 0x10), out, (s_shape_carrier_contact const *)input); break;
	case 2: function_1fa810((long *)(state + 0x10), (s_motion_adjustment_result *)out, (s_motion_adjustment const *)input); break;
	case 3: function_1ec690(input, (s_state_c570 *)(state + 0x10), out); break;
	case 4:
	case 5: function_1fc4b0((s_tracked_point *)(state + 0x10), (s_tracking_state const *)input, (s_tracking_output *)out); break;
	case 6:
	{
		real ticks = (real)g_510c54->field_2_3 * 0.3f;
		long rounded;
		rounded = real_to_long(ticks);
		state[1] = (byte)rounded;
		function_1eb6f0((s_ease_input *)input, (s_ease_state *)(state + 0x10), (s_ease_output *)out);
		break;
	}
	default: __assume(0);
	}
	function_1e5b50(input, (s_contact_dispatch_output *)out, state);
	c_z_shape_view *shape = *(c_z_shape_view **)(input + 0x14c);
	if (shape)
	{
		byte *component = g_51e9b8->data + (*(long *)(input + 0x18) & 0xffff) * 0xa0;
		byte *body = *(byte **)(*(byte **)(component + 0x70) + 0x40);
		c_z_shape_view *current = *(c_z_shape_view **)(body + 0xc);
		bool compound = current->v5() == 0x17;
		current = *(c_z_shape_view **)(body + 0xc);
		bool same = current == shape || compound && *(c_z_shape_view **)((byte *)current + 0x30) == shape;
		if (same) *(dword *)out |= 2;
		else if (shape->v5() == 7)
		{
			real margin = g_47f05c + 0.001f;
			real radius = *(real *)((byte *)shape + 0xc) - margin;
			if (g_47f05c > radius) radius = g_47f05c;
			byte capsule_storage[sizeof(c_z_capsule_temp) + 15];
			c_z_capsule_temp *capsule = new ((void *)(((dword)capsule_storage + 15) & ~15UL))
				c_z_capsule_temp((byte *)shape + 0x10, (byte *)shape + 0x20, radius);
			transform4x3f matrix;
			((s_havok_component *)component)->rigid_body_matrix_get(0, &matrix);
			matrix.position.z += margin * 0.5f;
			if (!function_1c5210(&matrix, capsule, *(long *)(input + 0x18), *(long *)(input + 0x24))) *(dword *)out |= 2;
			else *(dword *)out &= ~2;
			/* the capsule adds nothing to destroy: retail calls the reference object's destructor */
			capsule->c_reference_a9f20::~c_reference_a9f20();
		}
		else if (shape->v5() == 4 || (shape->v5() == 0x15 && (*(c_z_shape_view **)((byte *)shape + 0xc))->v5() == 4)) *(dword *)out |= 2;
		else *(dword *)out &= ~2;
	}
	if (**(dword const **)(input + 8) & 0x20)
	{
		long object_index = *(long *)(state + 4);
		function_b7740(object_index, &velocity, NULL, false);
		function_1c4b00(object_index, &velocity, NULL, 1);
		if (&velocity && velocity.i * velocity.i + velocity.j * velocity.j + velocity.k * velocity.k > 0.0001f)
		{
			function_b9b90(object_index, false);
			function_b7360(object_index);
			function_bba20(object_index);
		}
		point3f position;
		position.x = g_510c54->rate * velocity.i + *(real *)(input + 0xe8);
		position.y = g_510c54->rate * velocity.j + *(real *)(input + 0xec);
		position.z = g_510c54->rate * velocity.k + *(real *)(input + 0xf0);
		byte *object = *(byte **)(g_4e0300->data + (object_index & 0xffff) * 12 + 8);
		bool active = (*(dword *)(object + 4) & 0x100) != 0;
		if (active) function_b87b0(object_index);
		if (*(long *)(object + 0x14) != NONE ||
			(position.x >= -32768.0f && 32768.0f >= position.x && position.y >= -32768.0f &&
			32768.0f >= position.y && position.z >= -32768.0f && 32768.0f >= position.z))
		{
			*(point3f *)(object + 0x64) = position;
			object = *(byte **)(g_4e0300->data + (object_index & 0xffff) * 12 + 8);
			if (*(long *)(object + 0xd4) != NONE) function_b58c0(*(long *)(object + 0xd4), 2);
		}
		function_1c4a80(object_index, false, false);
		if (active) function_b8600(object_index, 0);
		function_bba20(object_index);
	}
}
