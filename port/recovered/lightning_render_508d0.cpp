// @flags /O2 /arch:SSE /Gr
/* the render of a lightning-like bolt request (retail 0x508d0, the callback
   0x429a0 stores with its 0x44-byte payload): the payload's tag (+8) has a
   block of bolts (+0x68, 0xec bytes each); every bolt draws up to eight
   copies of a noise strip (0x516d0, drawn by 0x4f7e0) from the payload's
   position to a point along its first vector (or around a cone of it, or the
   payload's endpoint, or where a ray toward it hits, with the bolt's impact
   effects and sounds there), each copy as the bolt's strips (+0xd8, 0x38
   bytes each) whose colour, alpha and width come from the strips' functions.
   The whole request fades between the tag's near (+0x40) and far (+0x44)
   distances from the camera.

   Payload (s_429a0_payload in src/unknown_01cf50.cpp): +0 reduced detail,
   +1 opacity (0..255), +8 tag index, +0xc owner (a 0xc4210 array index, or
   NONE), +0x10 position, +0x1c endpoint, +0x28 first vector, +0x34 second. */

#include "unknown_11c920.h"
#include "globals.h"
#include "camera_storage.h"
#include "reset_storage.h"
#include "object_queries.h"
#include "sound_sources.h"
#include <xtl.h>

struct s_tag_data;
struct s_render_reset_state;
struct s_random_draw;
struct s_random_draw_definition;
struct s_noise_draw_definition;

/* as src/unknown_050690.cpp declares them */
struct s_noise_basis
{
	real scale;
	real forward[3];
	real left[3];
	real up[3];
	real origin[3];
};

struct s_noise_point
{
	real position[3];
	real width;
	real amount;
	dword flags;
};

/* as src/unknown_01cf50.cpp declares it (the same layout as s_noise_point) */
struct s_501e0
{
	point3f field_0;
	real field_c;
	real field_10;
	dword field_14;
};

/* a collision result (0x5c bytes), as src/unknown_1689b0.cpp declares it */
struct s_collision_location_508d0
{
	long leaf_index;
	short cluster_index;
	short bsp_index;
};

struct s_collision_result_1697c0
{
	long type;
	real t;
	point3f point;
	s_collision_location_508d0 start_location;
	s_collision_location_508d0 end_location;
	short material_type;
	byte unknown26[2];
	plane3f plane;
	byte unknown38[0x5c - 0x38];
};

extern byte g_46713c;
extern byte g_4670bc;
extern dword g_4b8474;
extern short g_47d8e0;
extern s_random_draw g_4c9828;

real function_30bf0(vector3f *v);
void function_16b10(s_render_reset_state *state);
byte *function_c4210(long index, long element_index);
long function_c4250(long index);
void __stdcall function_507e0(s_random_draw *draw, const s_random_draw_definition *definition, real elapsed);
bool __stdcall function_1697c0(long flags, point3f const *point, vector3f const *vector,
	long ignore_object_index, long ignore_unit_index, s_collision_result_1697c0 *result);
void function_1883e0(long tag_index, bool ignore_distance, point3f const *point, short element_index, long entry_element_index,
	long index, long variant, long *first_value04, long *second_value04, long *first_value, long *second_value,
	long *first_value0c, long *second_value0c);
long function_1765e0(point3f const *point, vector3f const *direction, vector3f const *normal, long tag_index, long mode,
	long deterministic);
dword vector3d_compress(vector3f const *vector);
long function_1895f0(s_sound_position const *position, real scale, long tag_index);
real function_13b390(void const *function, real input, real range);
dword function_13bc00(s_tag_data const *function, real input);
color3f *unpack_color3f(dword pixel, color3f *color);
long __stdcall function_516d0(bool reduced, dword seed,
	const s_noise_draw_definition *definition, const s_random_draw *draw,
	const s_noise_basis *basis, real amplitude, s_noise_point *points);
bool __stdcall function_4f7e0(long tag, s_501e0 const *points, long count, short bitmap,
	real cosine, real sine, bool segments, bool perspective, bool filtered);

/* a tag function with its range ({long count; byte *data}): the function's
   value, mapped into its range [+4, +8] unless the function has its own
   range (a flag of 0xf0 in the header's second byte); 0 without data */
static inline real curve_value(byte const *curve, real input)
{
	long size = *(long const *)curve;
	byte const *data = *(byte const *const *)(curve + 4);
	if (!data || size <= 0)
		return 0.0f;
	real result = function_13b390(curve, input, 0.0f);
	if (!(data[1] & 0xf0))
	{
		real lower = *(real const *)(data + 4);
		real upper = *(real const *)(data + 8);
		if (result < 0.0f)
			result = 0.0f;
		else if (result > 1.0f)
			result = 1.0f;
		result = lower + (upper - lower) * result;
	}
	return result;
}

/* fistp: the x87's default rounding, to nearest with ties to even */
static inline long round_to_nearest_even(real value)
{
	long result = (long)value;
	real fraction = value - (real)result;
	if (fraction > 0.5f || (fraction == 0.5f && (result & 1)))
		++result;
	else if (fraction < -0.5f || (fraction == -0.5f && (result & 1)))
		--result;
	return result;
}

static inline void cross_product3d(real const *a, real const *b, real *out)
{
	out[0] = a[1] * b[2] - a[2] * b[1];
	out[1] = a[2] * b[0] - a[0] * b[2];
	out[2] = a[0] * b[1] - a[1] * b[0];
}

// @retail 0x508d0
void __stdcall function_508d0(void *payload_pointer)
{
	byte *payload = (byte *)payload_pointer;
	long tag_index = *(long *)(payload + 8);
	if (tag_index == NONE)
		return;
	byte *definition = g_4e3b44[tag_index & 0xffff].bytes;
	if (*(long *)(definition + 0x68) <= 0)
		return;

	point3f const *position = (point3f const *)(payload + 0x10);
	point3f const *endpoint = (point3f const *)(payload + 0x1c);
	vector3f const *first = (vector3f const *)(payload + 0x28);
	vector3f const *second = (vector3f const *)(payload + 0x34);

	/* the fade between the near and far distances from the camera */
	real fade = 1.0f;
	vector3f displacement;
	displacement.i = g_4b9da0.x - position->x;
	displacement.j = g_4b9da0.y - position->y;
	displacement.k = g_4b9da0.z - position->z;
	real distance = function_30bf0(&displacement);
	real fade_near = *(real *)(definition + 0x40);
	real fade_far = *(real *)(definition + 0x44);
	if (fade_far > fade_near)
	{
		real t = (fade_far - distance) / (fade_far - fade_near);
		if (t < 0.0f)
			return;
		if (t > 1.0f)
			fade = 1.0f;
		else
		{
			fade = t;
			if (fade <= 0.0f)
				return;
		}
	}

	g_46713c = 1;
	g_4670bc = 1;
	function_16b10((s_render_reset_state *)g_485b48);

	s_noise_basis basis[2];
	s_noise_point points[257];

	for (long bolt_index = 0; bolt_index < *(long *)(definition + 0x68); ++bolt_index)
	{
		byte *bolt = *(byte **)(definition + 0x6c) + bolt_index * 0xec;
		if (*(long *)(bolt + 0xd4) <= 0)
			continue;
		short copy_total = *(short *)(bolt + 8);
		if (copy_total <= 0)
			continue;

		long owner = *(long *)(payload + 0xc);
		s_random_draw *draw = owner != NONE ? (s_random_draw *)function_c4210(owner, bolt_index) : &g_4c9828;
		long copy_count = copy_total > 8 ? 8 : copy_total;
		if (owner == NONE)
			function_507e0(NULL, (s_random_draw_definition const *)bolt, 1.0f / 30.0f);

		/* the bolt's frame: up along the first vector, left along the second
		   (or the global up), forward their cross product */
		basis[0].scale = 1.0f;
		basis[0].origin[0] = position->x;
		basis[0].origin[1] = position->y;
		basis[0].origin[2] = position->z;
		basis[0].up[0] = first->i;
		basis[0].up[1] = first->j;
		basis[0].up[2] = first->k;
		if (*bolt & 1)
		{
			basis[0].left[0] = second->i;
			basis[0].left[1] = second->j;
			basis[0].left[2] = second->k;
			cross_product3d(basis[0].up, basis[0].left, basis[0].forward);
		}
		else
		{
			basis[0].left[0] = g_4687b0->i;
			basis[0].left[1] = g_4687b0->j;
			basis[0].left[2] = g_4687b0->k;
			cross_product3d(basis[0].up, basis[0].left, basis[0].forward);
			if (function_30bf0((vector3f *)basis[0].forward) < 0.0001f)
			{
				vector3f const *zero = g_4687a4;
				basis[0].forward[0] = zero->i;
				basis[0].forward[1] = zero->j;
				basis[0].forward[2] = zero->k;
				basis[0].left[0] = zero->i;
				basis[0].left[1] = zero->j;
				basis[0].left[2] = zero->k;
			}
		}
		real length = *(real *)(bolt + 4);
		basis[1] = basis[0];
		basis[1].origin[0] = first->i * length + basis[0].origin[0];
		basis[1].origin[1] = first->j * length + basis[0].origin[1];
		basis[1].origin[2] = first->k * length + basis[0].origin[2];

		for (long copy_index = 0; copy_index < copy_count; ++copy_index)
		{
			real fraction = 1.0f;
			real cone = *(real *)(bolt + 0xc);
			if (cone != 0.0f)
			{
				/* the copies spread around a cone of the first vector */
				real angle = (real)copy_index * 6.2831855f / (real)copy_count;
				if (*(real *)(bolt + 0x10) != 0.0f)
					angle = *(real *)((byte *)draw + 0x48) * 6.2831855f + angle;
				real radius = (real)tan(cone) * length;
				real forward_scale = (real)cos(angle) * radius;
				real left_scale = -((real)sin(angle) * radius);
				real end_x = first->i * length + basis[0].origin[0];
				real end_y = first->j * length + basis[0].origin[1];
				real end_z = first->k * length + basis[0].origin[2];
				basis[1].origin[0] = forward_scale * basis[0].forward[0] + end_x + basis[0].left[0] * left_scale;
				basis[1].origin[1] = basis[0].forward[1] * forward_scale + end_y + basis[0].left[1] * left_scale;
				basis[1].origin[2] = basis[0].forward[2] * forward_scale + end_z + basis[0].left[2] * left_scale;
			}

			if (*(short *)(definition + 2) == 1)
			{
				/* the request gives the end */
				basis[1].origin[0] = endpoint->x;
				basis[1].origin[1] = endpoint->y;
				basis[1].origin[2] = endpoint->z;
				if (length > 0.0f)
				{
					real dx = basis[1].origin[0] - basis[0].origin[0];
					real dy = basis[1].origin[1] - basis[0].origin[1];
					real dz = basis[1].origin[2] - basis[0].origin[2];
					fraction = (real)sqrt(dx * dx + dy * dy + dz * dz) / length;
					fraction = fraction < 0.0f ? 0.0f : (fraction > 1.0f ? 1.0f : fraction);
				}
			}
			else if (*bolt & 4)
			{
				/* the bolt stops where it hits, with its impact effects */
				displacement.i = basis[1].origin[0] - basis[0].origin[0];
				displacement.j = basis[1].origin[1] - basis[0].origin[1];
				displacement.k = basis[1].origin[2] - basis[0].origin[2];
				s_collision_result_1697c0 collision;
				collision.material_type = NONE;
				long ignore = function_c4250(*(long *)(payload + 0xc));
				if (function_1697c0(0x680000d, (point3f const *)basis[0].origin, &displacement, ignore, NONE, &collision))
				{
					long impact = *(long *)(bolt + 0x20);
					fraction = collision.t;
					basis[1].origin[0] = collision.point.x;
					basis[1].origin[1] = collision.point.y;
					basis[1].origin[2] = collision.point.z;
					if (impact != NONE && function_30bf0(&displacement) > 0.0001f)
					{
						long sounds[3];
						long effects[3];
						function_1883e0(impact, false, &collision.point, g_47d8e0, collision.material_type, 0xf, 0,
							&sounds[0], &effects[0], &sounds[1], &effects[1], &sounds[2], &effects[2]);
						for (long i = 0; i < 3; ++i)
						{
							if (effects[i] != NONE)
								function_1765e0(&collision.point, &displacement, &collision.plane.n, effects[i], 0, 0);
							if (sounds[i] != NONE)
							{
								s_sound_position sound;
								memset(&sound, 0, sizeof(sound));
								sound.position = collision.point;
								sound.compressed_forward = vector3d_compress(&displacement);
								sound.velocity = *g_4687a4;
								sound.location.leaf_index = collision.end_location.leaf_index;
								sound.location.cluster_index = collision.end_location.cluster_index;
								sound.location.bsp_index = collision.end_location.bsp_index;
								function_1895f0(&sound, 1.0f, sounds[i]);
							}
						}
					}
					fraction = fraction < 0.0f ? 0.0f : (fraction > 1.0f ? 1.0f : fraction);
				}
			}

			real amplitude = curve_value(bolt + 0xdc, fraction);
			real brightness = curve_value(bolt + 0xe4, fraction);
			long point_count = function_516d0(payload[0] != 0, (dword)copy_index << 8,
				(s_noise_draw_definition const *)bolt, draw, basis, amplitude, points);
			if (point_count <= 1)
				continue;
			real point_step = 1.0f / (real)(point_count - 1);

			for (long strip_index = 0; strip_index < *(long *)(bolt + 0xd4); ++strip_index)
			{
				byte *strip = *(byte **)(bolt + 0xd8) + strip_index * 0x38;
				real time = (real)strip_index + *(real *)((byte *)draw + 0x4c);
				real alpha = (real)payload[1] * brightness * fade * (1.0f / 255.0f);
				alpha = curve_value(strip + 0x20, time) * alpha;
				vector3f const *forward = &g_4b9dac;
				real dot = forward->i * first->i + forward->j * first->j + forward->k * first->k;
				real facing = dot < 0.0f ? -dot : dot;
				alpha = curve_value(strip + 0x28, facing) * alpha;

				for (long point_index = 0; point_index < point_count; ++point_index)
				{
					real t = (real)point_index * point_step;
					s_noise_point *point = &points[point_index];

					/* the colour: the strip's colour function at t, white
					   (the global colour) without one */
					byte *color_function = strip + 0x18;
					real color_input = function_13b390(color_function, t, 0.0f);
					color4f color;
					if (*(byte **)(color_function + 4) && *(long *)color_function > 0)
					{
						color3f rgb;
						unpack_color3f(function_13bc00((s_tag_data const *)color_function, color_input), &rgb);
						color.red = rgb.red;
						color.green = rgb.green;
						color.blue = rgb.blue;
					}
					else
					{
						color.red = g_4686cc->red;
						color.green = g_4686cc->green;
						color.blue = g_4686cc->blue;
					}
					color.alpha = alpha;
					dword blue = (dword)round_to_nearest_even(color.blue * 255.0f);
					dword green = (dword)round_to_nearest_even(color.green * 255.0f);
					dword red = (dword)round_to_nearest_even(color.red * 255.0f);
					dword alpha_byte = (dword)round_to_nearest_even(color.alpha * 255.0f);
					point->flags = blue | (green << 8) | (red << 16) | (alpha_byte << 24);
					point->width = curve_value(strip + 0x30, t);
					point->amount = curve_value(strip + 0x10, t);
				}

				function_4f7e0(*(long *)(bolt + 0x28), (s_501e0 const *)points, point_count, *(short *)(strip + 0xc),
					1.0f, 0.0f, true, !((*bolt >> 3) & 1), false);
			}
		}
	}

	if (!g_46713c)
	{
		g_4b8474 = 0;
		D3DDevice_SetVertexShader(0);
	}
	g_46713c = 1;
}
