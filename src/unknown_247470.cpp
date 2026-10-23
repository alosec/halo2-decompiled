#include "unknown_11c920.h"
#include "effects.h"

// @flags /Od /arch:SSE /Gr

struct s_247fd0;
struct s_247950;
struct s_object_2470e0;
struct s_object_247120;
struct s_particle_rate_definition;
struct s_particle_spawn_definition;
struct s_flag_holder;

vector3f *function_2470e0(s_object_2470e0 *arg_0);
real *function_2470f0(s_object_2470e0 *arg_0);
real function_247100(s_particle_rate_definition const *arg_0, real const *arg_1);
long function_247120(s_object_247120 const *arg_0);
long function_247130(s_object_247120 const *arg_0);
long function_173b80(s_flag_holder *arg_0);
long function_173b90(s_particle_system_datum *arg_0);
double __cdecl function_1c1a0(real arg_0);
vector3f *function_143070(matrix3x3 const *arg_1, vector3f const *arg_0, vector3f *arg_2);
matrix3x3 *function_142da0(real arg_0, real arg_1, real arg_2, matrix3x3 *arg_3);
matrix3x3 *function_142eb0(matrix3x3 const *right, matrix3x3 const *left, matrix3x3 *out);
void function_247950(s_particle_system_datum *arg_2, s_particle_spawn_definition const *arg_3,
	dword arg_4, real arg_5, real arg_6, real arg_7, s_247950 *arg_1, s_247fd0 *arg_0);

extern real const g_44ac18 = 0.0001f;

class c_247470
{
public:
	word field_0;
	word field_2;
	long field_4;
	long field_8;
	real field_c;
	matrix3x3 field_10;
	point3f field_34;
	point3f field_40;
	void function_247470(real arg_0, s_particle_system_datum *arg_1, s_object_2470e0 *arg_2,
		s_247fd0 *arg_3, transform4x3f const *arg_4, dword arg_5);
};

// @retail 0x247470
void c_247470::function_247470(real arg_0, s_particle_system_datum *arg_1, s_object_2470e0 *arg_2,
	s_247fd0 *arg_3, transform4x3f const *arg_4, dword arg_5)
{
	s_effect_particle_system_definition *local_0 = arg_1->function_1751d0();
	real local_1 = 1.f;
	field_40 = field_34;
	if (arg_4)
	{
		real const *local_2 = function_2470f0(arg_2);
		if ((byte)function_173b90(arg_1))
			local_1 = arg_4->scale;
		do {} while (false);
		field_10.forward = arg_4->forward;
		field_10.left = arg_4->left;
		field_10.up = arg_4->up;
		field_34 = arg_4->position;
		field_34.x *= local_1;
		field_34.y *= local_1;
		field_34.z *= local_1;
		vector3f local_3;
		function_143070(&field_10, function_2470e0(arg_2), &local_3);
		if (!(function_1c1a0(local_2[1] - 0.f) < g_44ac18) || !(function_1c1a0(local_2[0] - 0.f) < g_44ac18))
		{
			matrix3x3 local_4;
			function_142da0(local_2[0], local_2[1], 0.f, &local_4);
			function_142eb0(&field_10, &local_4, &field_10);
		}
		field_34.x += local_3.i;
		field_34.y += local_3.j;
		field_34.z += local_3.k;
	}
	if (!(byte)function_173b80((s_flag_holder *)local_0) || field_4 == NONE)
		field_c += function_247100((s_particle_rate_definition const *)arg_2, (real const *)arg_3) * arg_0;
	if (field_c + 0.0001f >= 1.f)
	{
		real local_5 = 0.f;
		real local_6;
		if ((byte)function_247130((s_object_247120 const *)arg_1) && (byte)function_247120((s_object_247120 const *)arg_1->function_1751d0()))
		{
			local_6 = 1.f / field_c;
			local_5 = 0.f;
		}
		else
		{
			local_6 = 0.f;
			local_5 = 1.f;
			arg_0 = 0.f;
		}
		while (field_c + 0.0001f >= 1.f)
		{
			long local_7; /* unreferenced; retail's frame keeps its slot at -0x48 */

			field_c -= 1.f;
			function_247950(arg_1, (s_particle_spawn_definition const *)arg_2, arg_5, local_5, arg_0, local_1, (s_247950 *)this, arg_3);
			local_5 += local_6;
		}
	}
}
