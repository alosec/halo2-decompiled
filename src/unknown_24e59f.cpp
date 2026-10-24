// @flags /O1 /Oi /Gr /arch:SSE
/* UNKNOWN_24E59F.CPP: placing a marker list on the screen: projected inside
   the view's ellipse it is drawn where it is (with an alternate look when
   something blocks the line from the camera), outside it is pinned to the
   ellipse's edge pointing at it */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_24ecae.h"
#include "camera_storage.h"
#include "projection_storage.h"
#include "view_context_storage.h"
#include "unknown_0259a0.h"
#include "player_queries.h"
#include <math.h>

struct s_view;
struct s_24e9c7_marker;
struct s_24e9c7_placement;

real distance3d(point3f const *a, point3f const *b);
point3f *transform4x3f_apply_point(transform4x3f const *matrix, point3f const *point, point3f *out);
real function_30bf0(vector3f *v);
bool function_30710(s_camera const *camera, short_rectangle2d const *rect, vector3f const *v, point2f *out, s_view const *view);
real normalize2d(point2f *v);
double __cdecl function_2b460(real value);
bool function_24e554(s_marker_list const *list);
bool function_14ddc0(long local_player_index);
long function_14de70(long local_player_index);
long function_baf80(long object_index);
bool function_16a040(long flags, point3f const *point0, point3f const *point1, long ignore_object_index,
	long ignore_unit_index, s_collision_result_1697c0 *result);
real function_24e9c7(s_24e9c7_marker const *marker, s_24e9c7_placement const *placement);
long __stdcall function_24ecae(s_marker_list const *list, s_24ecae_placement const *placement, real offset);
void function_24f0fc(s_marker_list const *list, s_24ecae_placement const *placement, real offset, long count);

/* the unit of the view's local player (its outermost parent), NONE without one */
static __forceinline long marker_viewer_unit(void)
{
	long unit_index = NONE;

	if (function_14ddc0(g_4b9ed8))
	{
		long player_index = function_14de70(g_4b9ed8);
		unit_index = ((s_player_datum *)(g_4e8c24->data + (player_index & 0xffff) * sizeof(s_player_datum)))->unit_index;
	}
	if (unit_index != NONE)
		unit_index = function_baf80(unit_index);
	return unit_index;
}

// @retail 0x24e59f
void __stdcall function_24e59f(s_marker_list *list)
{
	struct s_marker_collision_result
	{
		byte unknown00[8];
		point3f point;
		byte unknown14[0x24 - 0x14];
		short unknown24;
		byte unknown26[0x4c - 0x26];
		long surface_references[3];
		byte surface_flags[2];
		short surface_index;
	};
	/* retail keeps list on the stack (ret 4); its callers push it */
	s_marker_list *const *list_reference = &list;
	point3f const *position = &(*list_reference)->position;
	real distance = distance3d(position, &g_4b9da0);
	real lift = list->r18;
	if (lift > distance * 0.35)
		lift = (real)(distance * 0.35);

	vector3f const *up = g_4687b0;
	real height = list->r14 + lift;
	point3f point;
	point.x = up->i * height + position->x;
	point.y = up->j * height + position->y;
	point.z = up->k * height + position->z;

	s_24ecae_placement placement;
	placement.size = 0.0f;
	placement.hidden = false;
	placement.unknown15 = 0;
	placement.fading = false;
	placement.alternate = false;
	placement.unknown1c = distance;
	placement.fade = 0.0f;

	point3f transformed;
	transform4x3f_apply_point((transform4x3f const *)&g_4b9e14, &point, &transformed);
	function_30bf0((vector3f *)&transformed);

	point2f offset;
	real nx, ny;
	long on_screen = function_30710(&g_4b9e14, 0, (vector3f const *)&transformed, &offset, (s_view const *)&g_camera_storage);
	if (on_screen)
	{
		long half_width = (g_4b9dd6 - g_4b9dd2) / 2;
		long half_height = (g_4b9dd4 - g_4b9dd0) / 2;
		nx = (offset.x - (real)(g_4b9dd2 + half_width)) / (real)half_width;
		ny = (offset.y - (real)(g_4b9dd0 + half_height)) / (real)half_height;
	}
	else
	{
		nx = transformed.x;
		ny = 0.0f - transformed.y;
	}

	real half_width = (real)(g_4b9dd8.right - g_4b9dd8.left) * 0.5f;
	real half_height = (real)(g_4b9dd8.bottom - g_4b9dd8.top) * 0.5f;
	real ex = half_height * nx;
	real ey = half_width * ny;
	real half_extents = half_height * half_width;

	if (on_screen && ey * ey + ex * ex < half_extents * half_extents)
	{
		placement.forward_i = 0.0f;
		placement.forward_j = 1.0f;
		offset.x = (real)((g_4b9dd6 - g_4b9dd2) / 2) * nx;
		offset.y = (real)((g_4b9dd4 - g_4b9dd0) / 2) * ny;
		if (function_24e554(list))
		{
			s_marker_collision_result collision;
			collision.unknown24 = NONE;
			long unit_index = marker_viewer_unit();
			if (function_16a040(0x16808c2d, &g_4b9da0, &point, unit_index, list->l20, (s_collision_result_1697c0 *)&collision))
				placement.alternate = true;
		}
	}
	else
	{
		placement.size = 1.0f;
		placement.hidden = true;
		placement.unknown15 = 1;
		real scale = (real)sqrt(half_extents * half_extents / (ey * ey + ex * ex));
		offset.x = scale * nx;
		offset.y = scale * ny;
		placement.forward_i = offset.x;
		placement.forward_j = offset.y;
		normalize2d((point2f *)&placement.forward_i);
	}

	offset.x += (real)((g_4b9dd6 - g_4b9dd2) / 2);
	offset.y += (real)((g_4b9dd4 - g_4b9dd0) / 2);
	placement.position = offset;
	placement.position.x = (real)g_4b9dd2 + placement.position.x;
	placement.position.y = (real)g_4b9dd0 + placement.position.y;

	real t = placement.unknown1c * 0.1f;
	if (0.0f > t)
		t = 0.0f;
	else if (t > 1.0f)
		t = 1.0f;
	placement.scale = (real)(function_2b460((1.0f - t) * 24.0f + t * 16.0f + 0.5f) * 0.03125f);
	placement.position.y -= list->r1c;

	real marker_offset = function_24e9c7((s_24e9c7_marker const *)list, (s_24e9c7_placement const *)&placement);
	long count = function_24ecae(list, &placement, marker_offset);
	function_24f0fc(list, &placement, marker_offset, count);
}
