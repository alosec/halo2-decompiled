// @flags /O2 /arch:SSE /Gr
/* SCENERY_ANIMATION.CPP: starts an animation on a scenery object's own
   animation state, optionally relative to another object */

#include "object_dispatch.h"
#include "data_array.h"
#include "unknown_1dacb0.h"
#include "unknown_1cafc0.h"
#include "object_default_placement.h"
#include "scenery_animation.h"

void function_1ccb80(c_animation_channel *channel, real seconds);

/* plays the named animation of a graph on a scenery object from a frame;
   with an object to follow it is placed relative to that object */
// @retail 0x10a660
bool scenery_animation_begin(long scenery_index, long animation_name, short frame, long attached_object_index, bool interpolate, bool loop, long animation_graph_index)
{
	bool result = false;

	if (scenery_index != NONE && animation_graph_index != NONE && animation_name)
	{
		long header_offset = (scenery_index & 0xffff) * sizeof(s_object_header_datum);
		s_scenery_object_datum *scenery = (s_scenery_object_datum *)((s_object_header_datum *)(g_4e0300->data + header_offset))->object;
		if (*(short const volatile *)&scenery->animation_state_offset != NONE)
		{
			s_object_definition *definition = (s_object_definition *)g_4e3b44[scenery->definition_index & 0xffff].bytes;
			s_animation_state *state = (s_animation_state *)((byte *)scenery + scenery->animation_state_offset);
			if (state->graph_tag_index == animation_graph_index ||
				state->initialize(animation_graph_index, definition->model_tag_index, true))
			{
				c_type_709360 animation_id = state->animation_find(animation_name);
				if (animation_id.index != NONE)
				{
					long channel_flags = interpolate ? 0x4001 : 0x3f;
					if (loop)
						channel_flags |= 2;
					else
						channel_flags &= ~2;

					if (state->play(animation_id, channel_flags))
					{
						function_1ccb80(&state->channels[0], (real)frame * (1.0f / 30.0f));
						scenery->flags &= ~1;
						function_b7290(scenery_index);
						scenery->attached_object_index = attached_object_index;
						if (attached_object_index != NONE)
						{
							function_b9a50(scenery_index);
							if (TEST_FIELD_BIT(scenery->flag6))
								function_b8840(scenery_index);
							function_b75a0(scenery_index, g_468788, g_4687a8, g_4687b0, NULL, false);
							function_b7360(scenery_index);
							s_object_default_placement_view *placed = (s_object_default_placement_view *)
								((s_object_header_datum *)(g_4e0300->data + header_offset))->object;
							placed->hidden = false;
							if (!TEST_FIELD_BIT(placed->flag8) && g_4de2f4 && *(byte *)g_4de2f4)
								function_b8600(scenery_index, 0);
							function_b7290(scenery_index);
						}
						function_b9b90(scenery_index, true);
						function_bd020(scenery_index);
						result = true;
					}
				}
			}
		}
	}

	return result;
}

/* whether an animation graph has as many nodes as the model it animates; it
   lives in this file (not inline) so that only link-time code generation
   inlines it into s_animation_state::initialize (0x1cb0d0), as in retail */
bool animation_graph_fits_model(long graph_tag_index, long model_tag_index)
{
	bool result = false;
	if (graph_tag_index != NONE)
		result = (short)graph_tag_get(graph_tag_index)->node_count == *(long *)((byte *)graph_tag_get(model_tag_index) + 0x78);
	return result;
}
