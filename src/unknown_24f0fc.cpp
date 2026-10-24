// @flags /O1 /Oi /Gr /arch:SSE
#include "unknown_11c920.h"
#include "unknown_24ecae.h"
#include "globals.h"
#include "view_context_storage.h"

// @retail 0x24f0fc
void function_24f0fc(s_marker_list const *list, s_24ecae_placement const *placement, real offset, long count)
{
	if (!g_510c94 || list->l4 < 0 || list->l4 >= ((s_24ecae_globals *)g_510c94)->label_count || !list->b1)
		return;
	s_24f0fc_kind const *kind = &((s_24ecae_globals *)g_510c94)->labels[list->l4];
	if (kind->bitmap_index == NONE || kind->shader_index == NONE || kind->border_bitmap == NONE ||
		placement->hidden || count == 0)
		return;
	point2f center;
	center.x = placement->position.x;
	center.y = placement->position.y;
	center.y -= placement->scale * 16.0f + offset + 1.0f;
	real right_width = count > 1 ? 32.0f : 16.0f;
	real left_width = count > 1 ? 32.0f : 16.0f;
	box2f rect;
	rect.x0 = center.x - (placement->scale * left_width + 1.0f);
	rect.x1 = center.x + (placement->scale * right_width + 1.0f);
	rect.y0 = center.y - (placement->scale * 16.0f + 1.0f);
	rect.y1 = center.y + (placement->scale * 16.0f + 1.0f);
	g_4e6950.field_0 = *(color3f const *)&list->color24;
	g_4e6950.field_c = *(point3f const *)&list->color30;
	g_4e6950.value18 = 0.0f;
	g_4e6950.value24 = list->r40;
	if (list->b0)
		g_4e6950.value1c = g_4e69c0[g_4b9ed8] * list->r3c;
	else if (placement->hidden)
		g_4e6950.value1c = 0.0f;
	else
		g_4e6950.value1c = (1.0f - placement->fade) * g_4e69c0[g_4b9ed8] * list->r3c;
	box2f bounds;
	bounds.x0 = 0.0f;
	bounds.x1 = 1.0f;
	bounds.y0 = 0.0f;
	bounds.y1 = 0.5f;
	point2f points[4];
	for (long corner = 0; corner < 4; ++corner)
	{
		bool upper = corner < 2;
		bool right_side = corner == 1 || corner == 2;
		real x = right_side ? rect.x1 : rect.x0;
		real y = upper ? rect.y0 : rect.y1;
		real outer_y = upper ? -2.0f : 2.0f;
		points[0].x = x + (right_side ? 2.0f : -2.0f);
		points[0].y = y + outer_y;
		points[1].y = y + (upper ? -2.0f : 2.0f);
		points[1].x = x;
		points[2].x = x;
		points[2].y = y;
		points[3].x = x + (right_side ? 2.0f : -2.0f);
		points[3].y = y;
		function_22a664((s_widget_quad_2b11 const *)points, (s_float_rect const *)&bounds,
			kind->border_bitmap, 0, kind->shader_index);
	}
	bounds.x0 = 0.0f;
	bounds.x1 = 1.0f;
	bounds.y0 = 0.5f;
	bounds.y1 = 1.0f;
	points[0].x = rect.x0;
	points[0].y = rect.y0 - 2.0f;
	points[1].x = rect.x1;
	points[1].y = rect.y0 - 2.0f;
	points[2].x = rect.x1;
	points[2].y = rect.y0;
	points[3].x = rect.x0;
	points[3].y = rect.y0;
	function_22a664((s_widget_quad_2b11 const *)points, (s_float_rect const *)&bounds,
		kind->border_bitmap, 0, kind->shader_index);
	points[0].x = rect.x1;
	points[0].y = rect.y0;
	points[1].x = rect.x1 + 2.0f;
	points[1].y = rect.y0;
	points[2].x = rect.x1 + 2.0f;
	points[2].y = rect.y1;
	points[3].x = rect.x1;
	points[3].y = rect.y1;
	function_22a664((s_widget_quad_2b11 const *)points, (s_float_rect const *)&bounds,
		kind->border_bitmap, 0, kind->shader_index);
	points[0].x = rect.x0;
	points[0].y = rect.y0;
	points[1].x = rect.x0 - 2.0f;
	points[1].y = rect.y0;
	points[2].x = rect.x0 - 2.0f;
	points[2].y = rect.y1;
	points[3].x = rect.x0;
	points[3].y = rect.y1;
	function_22a664((s_widget_quad_2b11 const *)points, (s_float_rect const *)&bounds,
		kind->border_bitmap, 0, kind->shader_index);
	points[0].x = rect.x0;
	real edge = rect.y1 + 2.0f;
	real split = center.x - placement->scale * 16.0f + 1.0f;
	points[0].y = edge;
	points[1].x = split;
	points[1].y = edge;
	points[2].x = split;
	points[2].y = rect.y1;
	points[3].x = rect.x0;
	points[3].y = rect.y1;
	function_22a664((s_widget_quad_2b11 const *)points, (s_float_rect const *)&bounds,
		kind->border_bitmap, 0, kind->shader_index);
	split = placement->scale * 16.0f + center.x;
	points[0].x = rect.x1;
	points[0].y = edge;
	points[1].x = split;
	points[1].y = edge;
	points[2].x = split;
	points[2].y = rect.y1;
	points[3].x = rect.x1;
	points[3].y = rect.y1;
	function_22a664((s_widget_quad_2b11 const *)points, (s_float_rect const *)&bounds,
		kind->border_bitmap, 0, kind->shader_index);
}
