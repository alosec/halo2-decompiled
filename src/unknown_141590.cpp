// @flags /O2 /Ob1 /Gr /arch:SSE
/* UNKNOWN_141590.CPP: transform4x3f and quaternion math */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include <math.h>
#include <string.h>

struct rigid_transform_scaled
{
	quaternionf rotation;
	point3f position;
	real scale;
};

vector3f *function_11d000(vector3f const *v, vector3f *out);

__inline void real_point3d_set(point3f *point, real x, real y, real z)
{
	point->x = x;
	point->y = y;
	point->z = z;
}

#ifndef H2_NATIVE_PORT
// @retail 0x141590
void function_141590(
	transform4x3f const *in,
	transform4x3f *out)
{
	if (in->scale != 0.f)
	{
		vector3f v;
		real t;
		v.i = 0.f - in->position.x;
		v.j = 0.f - in->position.y;
		v.k = 0.f - in->position.z;
		if (in->scale != 1.f)
		{
			real inverse = 1.f / in->scale;
			out->scale = inverse;
			v.i = inverse * v.i;
			v.j = inverse * v.j;
			v.k = inverse * v.k;
		}
		else
		{
			out->scale = 1.f;
		}
		out->forward.i = in->forward.i;
		out->left.j = in->left.j;
		out->up.k = in->up.k;
		t = in->left.i; out->left.i = in->forward.j; out->forward.j = t;
		t = in->up.i; out->up.i = in->forward.k; out->forward.k = t;
		t = in->up.j; out->up.j = in->left.k; out->left.k = t;
		out->position.x = out->forward.i * v.i + out->left.i * v.j + out->up.i * v.k;
		out->position.y = out->forward.j * v.i + out->left.j * v.j + out->up.j * v.k;
		out->position.z = out->forward.k * v.i + out->left.k * v.j + out->up.k * v.k;
	}
	else
	{
		memset(out, 0, sizeof(transform4x3f));
	}
}
#endif

// @retail 0x1416c0
void function_1416c0(
	point3f const *position,
	transform4x3f *out)
{
	out->scale = 1.f;
	out->forward.i = 1.f;
	out->forward.j = 0.f;
	out->forward.k = 0.f;
	out->left.i = 0.f;
	out->left.j = 1.f;
	out->left.k = 0.f;
	out->up.i = 0.f;
	out->up.j = 0.f;
	out->up.k = 1.f;
	out->position = *position;
}

// @retail 0x141710
void matrix4x3_from_forward_and_up(
	transform4x3f *out,
	vector3f const *forward,
	vector3f const *up)
{
	out->scale = 1.f;
	out->forward = *forward;
	vector3f left;
	left.k = forward->j * up->i;
	left.k -= forward->i * up->j;
	left.j = up->k * forward->i - forward->k * up->i;
	left.i = forward->k * up->j;
	left.i -= up->k * forward->j;
	out->left.k = left.k;
	out->left.j = left.j;
	out->left.i = left.i;
	out->up = *up;
	real_point3d_set(&out->position, 0.f, 0.f, 0.f);
}

real function_30bf0(vector3f *v);
bool function_143120(vector3f const *forward, vector3f const *left, vector3f const *up);

quaternionf *g_4687cc;
extern transform4x3f *g_4687d0;

/* rotates a vector by a unit quaternion */
static inline void quaternion_transform_vector(
	quaternionf const *q,
	vector3f const *v,
	vector3f *out)
{
	real dot = (q->i * v->i + q->j * v->j + q->k * v->k) * 2.f;
	real w2 = q->w * 2.f;
	real s = q->w * q->w * 2.f - 1.f;
	vector3f cross;

	cross.i = q->j * v->k - q->k * v->j;
	cross.j = q->k * v->i - q->i * v->k;
	cross.k = q->i * v->j - q->j * v->i;
	out->i = v->i * s + q->i * dot + cross.i * w2;
	out->j = v->j * s + q->j * dot + cross.j * w2;
	out->k = v->k * s + q->k * dot + cross.k * w2;
}

/* the rotation that takes one unit vector to another */
// @retail 0x1417b0
void matrix4x3_rotation_between_vectors(
	transform4x3f *matrix,
	vector3f const *arg_5f338b,
	vector3f const *arg_bc44c6)
{
	quaternionf rotation;
	real cosine = arg_5f338b->i * arg_bc44c6->i + arg_5f338b->j * arg_bc44c6->j + arg_5f338b->k * arg_bc44c6->k;

	if (cosine < -1.f)
	{
		cosine = -1.f;
	}
	else if (cosine > 1.f)
	{
		cosine = 1.f;
	}

	real cosine_half = (real)sqrt((cosine + 1.f) * 0.5f);
	real sine_half = (real)sqrt((1.f - cosine) * 0.5f);
	real sine = sine_half * cosine_half * 2.f;

	if (sine != 0.f)
	{
		real scale = sine_half / sine;

		rotation.i = (arg_5f338b->j * arg_bc44c6->k - arg_5f338b->k * arg_bc44c6->j) * scale;
		rotation.j = (arg_5f338b->k * arg_bc44c6->i - arg_5f338b->i * arg_bc44c6->k) * scale;
		rotation.k = (arg_5f338b->i * arg_bc44c6->j - arg_5f338b->j * arg_bc44c6->i) * scale;
		rotation.w = cosine_half;
	}
	else if (cosine < 0.f)
	{
		function_11d000(arg_5f338b, (vector3f *)&rotation);
		rotation.w = 0.f;
	}
	else
	{
		rotation = *g_4687cc;
	}

	*matrix = *g_4687d0;
	quaternion_transform_vector(&rotation, &matrix->forward, &matrix->forward);
	quaternion_transform_vector(&rotation, &matrix->up, &matrix->up);
	quaternion_transform_vector(&rotation, &matrix->left, &matrix->left);
	if (!function_143120(&matrix->forward, &matrix->left, &matrix->up))
	{
		function_30bf0(&matrix->up);
		matrix->left.i = matrix->up.j * matrix->forward.k - matrix->up.k * matrix->forward.j;
		matrix->left.j = matrix->up.k * matrix->forward.i - matrix->up.i * matrix->forward.k;
		matrix->left.k = matrix->up.i * matrix->forward.j - matrix->up.j * matrix->forward.i;
		function_30bf0(&matrix->left);
		matrix->forward.i = matrix->left.j * matrix->up.k - matrix->left.k * matrix->up.j;
		matrix->forward.j = matrix->left.k * matrix->up.i - matrix->left.i * matrix->up.k;
		matrix->forward.k = matrix->left.i * matrix->up.j - matrix->left.j * matrix->up.i;
	}
}

// @retail 0x141ce0
void function_141ce0(
	real a,
	real b,
	real c,
	transform4x3f *out)
{
	real cos_c, sin_c, cos_b, sin_b, cos_a, sin_a, sin_b_sin_c, sin_b_cos_c;
	cos_c = (real)cos(c);
	sin_c = (real)sin(c);
	cos_b = (real)cos(b);
	sin_b = (real)sin(b);
	cos_a = (real)cos(a);
	sin_b_sin_c = sin_b * sin_c;
	sin_b_cos_c = sin_b * cos_c;
	sin_a = (real)sin(a);
	out->scale = 1.f;
	out->forward.j = sin_a * cos_c - sin_b_sin_c * cos_a;
	out->forward.i = cos_a * cos_b;
	out->forward.k = sin_b_cos_c * cos_a + sin_a * sin_c;
	out->left.i = 0.f - sin_a * cos_b;
	out->left.j = sin_b_sin_c * sin_a + cos_a * cos_c;
	out->up.i = 0.f - sin_b;
	out->left.k = cos_a * sin_c - sin_b_cos_c * sin_a;
	out->up.j = 0.f - cos_b * sin_c;
	out->up.k = cos_b * cos_c;
	out->position.x = 0.f;
	out->position.y = 0.f;
	out->position.z = 0.f;
}

// @retail 0x141e10
matrix3x3 *function_141e10(
	matrix3x3 *out,
	quaternionf const *q)
{
	real norm = q->i * q->i + q->j * q->j + q->k * q->k + q->w * q->w;
	real s = norm > 0.0001f ? 2.f / norm : 0.f;
	real xs = q->i * s;
	real ys = q->j * s;
	real zs = q->k * s;
	real wx = q->w * xs;
	real wy = q->w * ys;
	real wz = q->w * zs;
	real xx = q->i * xs;
	real xy = q->i * ys;
	real xz = q->i * zs;
	real yy = q->j * ys;
	real yz = q->j * zs;
	real zz = q->k * zs;
	out->forward.i = 1.f - (yy + zz);
	out->left.i = xy - wz;
	out->up.i = xz + wy;
	out->forward.j = xy + wz;
	out->left.j = 1.f - (xx + zz);
	out->up.j = yz - wx;
	out->forward.k = xz - wy;
	out->left.k = yz + wx;
	out->up.k = 1.f - (xx + yy);
	return out;
}

// @retail 0x141f60
quaternionf *function_141f60(
	matrix3x3 const *matrix,
	quaternionf *out)
{
	real const *m = (real const *)matrix;
	real trace = matrix->left.j + matrix->forward.i + matrix->up.k;
	if (trace > 0.f)
	{
		real root = (real)(sqrt(trace + 1.f) * 0.5f);
		real inverse = 0.25f / root;
		out->w = root;
		out->i = (m[5] - m[7]) * inverse;
		out->j = (m[6] - m[2]) * inverse;
		out->k = (m[1] - m[3]) * inverse;
	}
	else
	{
		long i = 0, j, k;
		real root, inverse;
		if (m[4] > m[0])
			i = 1;
		if (m[8] > m[i * 4])
			i = 2;
		j = (i + 1) % 3;
		k = (i + 2) % 3;
		root = (real)(sqrt(m[i * 4] - m[j * 4] - m[k * 4] + 1.f) * 0.5f);
		inverse = 0.25f / root;
		out->n[i] = root;
		out->w = (m[j * 3 + k] - m[k * 3 + j]) * inverse;
		out->n[j] = (m[i * 3 + j] + m[j * 3 + i]) * inverse;
		out->n[k] = (m[i * 3 + k] + m[k * 3 + i]) * inverse;
	}
	if (out->w < 0.f)
	{
		out->w = 0.f - out->w;
		out->i = 0.f - out->i;
		out->j = 0.f - out->j;
		out->k = 0.f - out->k;
	}
	return out;
}

__declspec(noinline) void function_1420f0(transform4x3f *out, point3f const *position, vector3f const *forward, vector3f const *up);

// @retail 0x1420f0
inline void function_1420f0(
	transform4x3f *out,
	point3f const *position,
	vector3f const *forward,
	vector3f const *up)
{
	out->scale = 1.f;
	out->forward = *forward;
	vector3f left;
	left.k = forward->j * up->i;
	left.k -= forward->i * up->j;
	left.j = up->k * forward->i - forward->k * up->i;
	left.i = forward->k * up->j;
	left.i -= up->k * forward->j;
	out->left.k = left.k;
	out->left.j = left.j;
	out->left.i = left.i;
	out->up = *up;
	real_point3d_set(&out->position, 0.f, 0.f, 0.f);
	out->position = *position;
}

// @retail 0x1421b0
void function_1421b0(
	transform4x3f *out,
	point3f const *position,
	quaternionf const *rotation)
{
	function_141e10(&out->rotation, rotation);
	out->position.x = 0.f;
	out->position.y = 0.f;
	out->position.z = 0.f;
	out->scale = 1.f;
	out->position = *position;
}

real g_45dbdc = 0.0001f;
static const real g_45dbc0 = 1.f;
__declspec(align(16)) static const unsigned long g_453750[4] = {0x80000000, 0, 0, 0x80000000};

// @retail 0x1421f0
void __stdcall function_1421f0(
	transform4x3f *out,
	rigid_transform_scaled const *orientation)
{
	static real epsilon = g_45dbdc;
	__asm
	{
		mov eax, orientation
		mov ecx, out
		movlps xmm0, qword ptr [eax]
		movhps xmm0, qword ptr [eax + 8]
		movaps xmm7, xmm0
		mulps xmm0, xmm7
		movhlps xmm1, xmm0
		addps xmm0, xmm1
		movaps xmm1, xmm0
		shufps xmm1, xmm1, 0x55
		addss xmm0, xmm1
		comiss xmm0, epsilon
		ja big
		movss xmm0, g_45dbc0
		movlps qword ptr [ecx + 4], xmm0
		movhps qword ptr [ecx + 0xc], xmm0
		movlps qword ptr [ecx + 0x14], xmm0
		movhps qword ptr [ecx + 0x1c], xmm0
		movss dword ptr [ecx + 0x24], xmm0
		jmp done
	big:
		rcpss xmm1, xmm0
		mulss xmm0, xmm1
		mulss xmm0, xmm1
		addss xmm1, xmm1
		subss xmm1, xmm0
		addss xmm1, xmm1
		shufps xmm1, xmm1, 0
		mulps xmm7, xmm1
		movss xmm4, dword ptr [eax]
		shufps xmm4, xmm4, 0
		mulps xmm4, xmm7
		movss xmm5, dword ptr [eax + 0xc]
		shufps xmm5, xmm5, 0
		mulps xmm5, xmm7
		movss xmm6, dword ptr [eax + 4]
		shufps xmm6, xmm6, 0
		mulps xmm6, xmm7
		movhlps xmm7, xmm7
		mulss xmm7, dword ptr [eax + 8]
		movaps xmm0, xmm4
		shufps xmm0, xmm0, 0x99
		movaps xmm1, xmm5
		shufps xmm1, xmm1, 0x66
		xorps xmm1, xmmword ptr g_453750
		addps xmm0, xmm1
		movhps qword ptr [ecx + 8], xmm0
		movss dword ptr [ecx + 0x10], xmm0
		movlps qword ptr [ecx + 0x18], xmm0
		movhlps xmm2, xmm6
		shufps xmm6, xmm6, 0x55
		movss xmm0, g_45dbc0
		subss xmm0, xmm7
		movss xmm1, xmm0
		subss xmm0, xmm6
		movss dword ptr [ecx + 4], xmm0
		subss xmm1, xmm4
		movss dword ptr [ecx + 0x14], xmm1
		movss xmm3, g_45dbc0
		subss xmm3, xmm4
		subss xmm3, xmm6
		movss dword ptr [ecx + 0x24], xmm3
		movss xmm7, xmm2
		subss xmm7, xmm5
		movss dword ptr [ecx + 0x20], xmm7
		addss xmm2, xmm5
		movss dword ptr [ecx + 0x18], xmm2
	done:
		movss xmm0, dword ptr [eax + 0x1c]
		movss dword ptr [ecx], xmm0
		movlps xmm0, qword ptr [eax + 0x10]
		movlps qword ptr [ecx + 0x28], xmm0
		movss xmm0, dword ptr [eax + 0x18]
		movss dword ptr [ecx + 0x30], xmm0
	}
}

// @retail 0x142360
void orientation_from_matrix4x3(
	transform4x3f const *matrix,
	rigid_transform_scaled *out)
{
	function_141f60(&matrix->rotation, &out->rotation);
	out->position = matrix->position;
	out->scale = matrix->scale;
}

__inline real normalize3d(vector3f *v)
{
	real m = (real)sqrt(v->i * v->i + v->j * v->j + v->k * v->k);
	if (!(fabs(m) < 0.0001f))
	{
		real inv = 1.f / m;
		v->i = inv * v->i;
		v->j = v->j * inv;
		v->k = inv * v->k;
		return m;
	}
	return 0.f;
}

// @retail 0x142390
void function_142390(
	plane3f const *plane,
	transform4x3f *out)
{
	vector3f w;
	point3f position;
	function_11d000(&plane->n, &w);
	normalize3d(&w);
	position.x = plane->d * plane->n.i;
	position.y = plane->n.j * plane->d;
	position.z = plane->n.k * plane->d;
	function_1420f0(out, &position, &w, &plane->n);
}

int __fastcall function_142a60(transform4x3f const *a, transform4x3f const *b, transform4x3f *result);
void function_11d790(quaternionf const *q, vector3f *axis, real *angle);

// @retail 0x1424f0
vector3f *matrix4x3_rotation_between(
	transform4x3f const *a,
	transform4x3f const *b,
	vector3f *out)
{
	transform4x3f inverse;
	transform4x3f relative;
	quaternionf rotation;
	real angle;

	function_141590(a, &inverse);
	function_142a60(b, &inverse, &relative);
	function_141f60(&relative.rotation, &rotation);
	function_11d790(&rotation, out, &angle);
	out->i *= angle;
	out->j *= angle;
	out->k *= angle;
	return out;
}

// @retail 0x142570
point3f *transform4x3f_apply_point(
	transform4x3f const *matrix,
	point3f const *point,
	point3f *out)
{
	/* retail passes the matrix on the stack (ret 4) while 0x142640, with the
	   same body, takes it in ecx: the parameter's address is taken here, and
	   the optimizer removes the indirection only after LTCG has chosen the
	   convention. Its callers' conventions (0x2104b0 ...) follow from it. */
	transform4x3f const *const *matrix_reference = &matrix;
	real x = point->x;
	real y = point->y;
	real z = point->z;
	if ((*matrix_reference)->scale != 1.f)
	{
		x = matrix->scale * x;
		y = matrix->scale * y;
		z = matrix->scale * z;
	}
	out->x = matrix->up.i * z + matrix->left.i * y + matrix->forward.i * x + matrix->position.x;
	out->y = matrix->up.j * z + matrix->left.j * y + matrix->forward.j * x + matrix->position.y;
	out->z = matrix->up.k * z + matrix->left.k * y + matrix->forward.k * x + matrix->position.z;
	return out;
}

// @retail 0x142640
vector3f *function_142640(
	transform4x3f const *matrix,
	vector3f const *vector,
	vector3f *out)
{
	real x = vector->i;
	real y = vector->j;
	real z = vector->k;
	if (matrix->scale != 1.f)
	{
		x = matrix->scale * x;
		y = matrix->scale * y;
		z = matrix->scale * z;
	}
	out->i = matrix->up.i * z + matrix->left.i * y + matrix->forward.i * x;
	out->j = matrix->up.j * z + matrix->left.j * y + matrix->forward.j * x;
	out->k = matrix->up.k * z + matrix->left.k * y + matrix->forward.k * x;
	return out;
}

#if !H2_NATIVE_PORT
// @retail 0x142700
point3f *function_142700(
	transform4x3f const *matrix,
	point3f const *point,
	point3f *out)
{
	if (matrix->scale != 0.f)
	{
		real x = point->x - matrix->position.x;
		real y = point->y - matrix->position.y;
		real z = point->z - matrix->position.z;
		if (matrix->scale != 1.f)
		{
			real inverse = 1.f / matrix->scale;
			x = inverse * x;
			y = inverse * y;
			z = inverse * z;
		}
		out->x = matrix->forward.k * z + matrix->forward.j * y + matrix->forward.i * x;
		out->y = matrix->left.k * z + matrix->left.j * y + matrix->left.i * x;
		out->z = matrix->up.k * z + matrix->up.j * y + matrix->up.i * x;
	}
	else
	{
		out->x = 0.f;
		out->y = 0.f;
		out->z = 0.f;
	}
	return out;
}
#endif

// @retail 0x1427f0
vector3f *function_1427f0(
	transform4x3f const *matrix,
	vector3f const *vector,
	vector3f *out)
{
	real x = vector->i;
	real y = vector->j;
	real z = vector->k;
	if (matrix->scale != 1.f)
	{
		real inverse = 1.f / matrix->scale;
		x = inverse * x;
		y = inverse * y;
		z = inverse * z;
	}
	out->i = matrix->forward.k * z + matrix->forward.j * y + matrix->forward.i * x;
	out->j = matrix->left.k * z + matrix->left.j * y + matrix->left.i * x;
	out->k = matrix->up.k * z + matrix->up.j * y + matrix->up.i * x;
	return out;
}

bool function_a0190(vector3f const *vector);
bool function_a0200(real a, real b);

// @retail 0x143120
bool function_143120(
	vector3f const *forward,
	vector3f const *left,
	vector3f const *up)
{
	return function_a0190(forward) &&
		function_a0190(left) &&
		function_a0190(up) &&
		function_a0200(dot3f(forward, left), 0.f) &&
		function_a0200(dot3f(left, up), 0.f) &&
		function_a0200(dot3f(forward, up), 0.f);
}

// @retail 0x143250
void function_143250(
	real *out,
	unsigned long n,
	real const *a,
	real const *b)
{
	unsigned long i, j;
	for (i = 0; i < n; i++)
	{
		out[i] = b[0] * a[i * n];
		for (j = 1; j < n; j++)
			out[i] = b[j] * a[i * n + j] + out[i];
	}
}
