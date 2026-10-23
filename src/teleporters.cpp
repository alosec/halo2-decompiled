#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0259d0.h"
#include "unknown_19ec40.h"
#include "data_array.h"
#include "game_engine_events.h"
#include "object_default_placement.h"
#include "teleporters.h"
#include <math.h>
#include <string.h>

// @flags /O2 /arch:SSE /Gr

/* TELEPORTERS.CPP: the multiplayer teleporters. Each of the 16 keys that has
   both an entrance marker (type 7) and an exit marker (type 8) makes a
   teleporter. Every tick the nearest player whose unit stands within half a
   unit of an entrance (and rides nothing) is moved to the exit, turned by the
   angle between the two markers, and the teleporter rests for its recharge
   time. When a shape already fills the exit, a player's unit there is flagged
   as blocking and the waiting player sees the blocked constants every 4
   seconds. Moved here from unknown_19ca10.cpp. */

void function_1a0180(long tag_index, long string_handle, word *buffer);
void function_24cbee(long player_index, word const *text);

/* shows a player the multiplayer globals' "teleporter blocked" message */
// @retail 0x19ca10
void teleporter_display_blocked_message(long player_index)
{
	s_teleporter_player *player;
	s_teleporter_globals_definition *definition;
	word text[0x100];

	text[0] = 0;
	player = (s_teleporter_player *)(g_4e8c24->data + (player_index & 0xffff) * sizeof(s_teleporter_player));
	wcsncpy(text, L"", 0xff);
	text[0xff] = 0;
	definition = (s_teleporter_globals_definition *)g_4e3b44[g_4e034c->index & 0xffff].bytes;
	if (definition->runtime_count)
	{
		long string_handle = definition->runtime->constants->blocked_message;

		text[0] = 0;
		if (g_510c94 && g_510c94->string_list != NONE)
			function_1a0180(g_510c94->string_list, string_handle, text);
	}
	function_24cbee(player->local_index, text);
}

/* finds the first entrance and exit markers that carry a key; true when
   both exist */
// @retail 0x19cb20
bool teleporter_find_markers(s_teleporter *teleporter, long key)
{
	long marker_index;

	marker_index = NONE;
	function_19ec40(NULL, 0.0f, 7, NONE, (short)key, 1, &marker_index, 0.0f);
	teleporter->entrance_marker_index = marker_index;
	marker_index = NONE;
	function_19ec40(NULL, 0.0f, 8, NONE, (short)key, 1, &marker_index, 0.0f);
	teleporter->exit_marker_index = marker_index;
	teleporter->recharge_ticks = 0;
	teleporter->blocked_message_ticks = 0;

	return teleporter->entrance_marker_index != NONE && teleporter->exit_marker_index != NONE;
}

/* builds the teleporters from the scenario's markers, one per key */
// @retail 0x19cad0
void teleporters_initialize()
{
	s_mp_globals *globals = g_4e9ae8;
	long key;

	globals->teleporter_count = 0;
	for (key = 0; key < 16; key++)
	{
		if (teleporter_find_markers(&globals->teleporters[globals->teleporter_count], key))
			globals->teleporter_count++;
	}
}

bool player_iterator_next_with_unit(long *iterator);
real distance_sq3f(point3f const *a, point3f const *b);

/* the square of how near an entrance a unit must stand (half a unit) */
real const g_teleporter_entrance_radius_squared = 0.25f;

/* flags the players whose unit stands near the teleporter's entrance */
// @retail 0x19ce10
void teleporter_mark_players_near_entrance(s_teleporter *teleporter)
{
	if (!teleporter->recharge_ticks)
	{
		s_marker_entry const &entrance = g_4e0350->marker_entries[teleporter->entrance_marker_index];
		point3f position = entrance.position;
		s_teleporter_player_iterator iterator;

		iterator.data = g_4e8c24;
		iterator.index = NONE;
		iterator.datum_index = NONE;
		while (player_iterator_next_with_unit((long *)&iterator))
		{
			s_teleporter_player *player = iterator.player;
			long unit_index = player->unit_index;
			point3f unit_position;
			vector3f delta;

			function_b9dd0(unit_index, &unit_position);
			vector3d_from_points3d(&position, &unit_position, &delta);
			if (((s_teleporter_object_header *)g_4e0300->data)[unit_index & 0xffff].object->parent_index == NONE &&
				delta.j * delta.j + delta.i * delta.i + delta.k * delta.k < g_teleporter_entrance_radius_squared)
			{
				player->near_entrance = true;
			}
		}
	}
}

struct s_shapes;
struct s_shape_result;
void __stdcall function_df5f0(long object_index, point3f *center, real *height, real *radius);
bool __stdcall function_16a440(dword flags, point3f const *position, real extent, real height, real radius,
	long ignore_object, long ignore_parent, s_shapes *shapes);
bool function_245ef0(s_shapes const *shapes, point3f const *point, s_shape_result *result);
void __stdcall function_a7840(short player_index, dword mask);

/* whether a shape fills the exit where the player's unit would stand; a
   player's unit there is flagged as blocking the teleporter */
// @retail 0x19cb90
bool teleporter_exit_blocked(s_teleporter *teleporter, long player_index)
{
	bool blocked = false;
	s_teleporter_player *player = (s_teleporter_player *)(g_4e8c24->data + (player_index & 0xffff) * sizeof(s_teleporter_player));
	s_marker_entry const &exit = g_4e0350->marker_entries[teleporter->exit_marker_index];
	point3f destination = exit.position;
	point3f center;
	real radius;
	real height;
	function_df5f0(player->unit_index, &center, &height, &radius);
	real extent = height * 0.5f + radius;
	point3f origin = destination;
	origin.z += extent;
	byte shapes_storage[0xb808];
	if (function_16a440(0x500038, &origin, extent, height, radius, player->unit_index, NONE, (s_shapes *)shapes_storage))
	{
	point3f point = destination;
	point.z += radius;
	s_teleporter_exit_shape shape;
	if (function_245ef0((s_shapes *)shapes_storage, &point, (s_shape_result *)&shape))
	{
	long object_index = shape.object_index;
	if (object_index != NONE)
	{
		s_teleporter_object *object = ((s_teleporter_object_header *)g_4e0300->data)[object_index & 0xffff].object;
		if ((1 << object->type) & 3)
		{
			long blocking_player_index = object->player_index;
			if (blocking_player_index != NONE)
			{
				byte *blocking_player = g_4e8c24->data + (blocking_player_index & 0xffff) * sizeof(s_teleporter_player);
				((s_teleporter_player *)blocking_player)->blocking_teleporter = true;
				function_a7840(object->player_index_short, 8);
			}
		}
	}
	blocked = true;
	}
	}
	return blocked;
}

void marker_get_position_and_angle(long index, point3f *position, real *angle);
real function_30bf0(vector3f *vector);
void function_1874b0(long player_index, vector3f const *forward);
void function_a8aa0(long unit_index);

/* sends the nearest player at the entrance to the exit, or, when the exit is
   blocked, shows them the blocked message */
// @retail 0x19cf20
void __stdcall teleporter_update(s_teleporter *teleporter)
{
	if (!teleporter->recharge_ticks)
	{
		real closest_distance = g_teleporter_entrance_radius_squared;
		s_marker_entry const &entrance = g_4e0350->marker_entries[teleporter->entrance_marker_index];
		point3f position = entrance.position;
		real entrance_angle = *(real *)entrance.unknown0c;
		long closest_player = NONE;
		s_teleporter_player_iterator iterator;
		iterator.data = g_4e8c24;
		iterator.index = NONE;
		iterator.datum_index = NONE;
		while (player_iterator_next_with_unit((long *)&iterator))
		{
			long unit_index = iterator.player->unit_index;
			point3f unit_position;
			function_b9dd0(unit_index, &unit_position);
			vector3f delta;
			vector3d_from_points3d(&position, &unit_position, &delta);
			real distance = delta.i * delta.i + delta.j * delta.j + delta.k * delta.k;
			if (((s_teleporter_object_header *)g_4e0300->data)[unit_index & 0xffff].object->parent_index == NONE &&
				closest_distance > distance)
			{
				closest_player = iterator.datum_index;
				closest_distance = distance;
			}
		}
		if (closest_player != NONE)
		{
			s_teleporter_player *player = (s_teleporter_player *)(g_4e8c24->data + (closest_player & 0xffff) * sizeof(s_teleporter_player));
			if (!(player->flags & (1 << _teleporter_player_teleported_bit)))
			{
				if (!teleporter_exit_blocked(teleporter, closest_player))
				{
					s_teleporter_object *object = ((s_teleporter_object_header *)g_4e0300->data)[player->unit_index & 0xffff].object;
					vector3f forward = object->forward;
					point3f destination;
					real exit_angle;
					marker_get_position_and_angle(teleporter->exit_marker_index, &destination, &exit_angle);
					player->has_teleported = true;
					s_event event;
					game_engine_event_initialize_inline(&event, 0, 0x16);
					event.cause_player_index = closest_player;
					event.a = closest_player;
					function_19eb90(&event);
					real angle = (real)atan2(forward.j, forward.i) + exit_angle - entrance_angle;
					forward.i = (real)cos(angle);
					forward.j = (real)sin(angle);
					forward.k = 0.0f;
					function_30bf0(&forward);
					function_b9a50(player->unit_index);
					function_b75a0(player->unit_index, &destination, &forward, g_4687b0, NULL, false);
					if (player->local_index != NONE)
						function_1874b0(player->local_index, &forward);
					s_teleporter_globals_definition *definition = (s_teleporter_globals_definition *)g_4e3b44[g_4e034c->index & 0xffff].bytes;
					real recharge_ticks = g_510c54->field_2_3 * definition->runtime->constants->recharge_seconds;
					long ticks;
					__asm
					{
						fld recharge_ticks
						fistp ticks
					}
					teleporter->recharge_ticks = ticks;
				}
				else
				{
					player = (s_teleporter_player *)(g_4e8c24->data + (closest_player & 0xffff) * sizeof(s_teleporter_player));
					if (teleporter->blocked_message_ticks > 0)
						teleporter->blocked_message_ticks--;
					else
					{
						teleporter->blocked_message_ticks = g_510c54->field_2_3 * 4;
						if (player->local_index != NONE)
							teleporter_display_blocked_message(closest_player);
						function_a8aa0(player->unit_index);
					}
					return;
				}
			}
		}
		teleporter->blocked_message_ticks = 0;
	}
	else
		teleporter->recharge_ticks--;
}

/* runs the teleporters for a tick */
// @retail 0x19cd10
void teleporters_update(void)
{
	s_record_pool *data = g_4e8c24;
	s_teleporter_player_iterator iterator;
	iterator.data = data;
	iterator.index = NONE;
	iterator.datum_index = NONE;
	while (player_iterator_next_with_unit((long *)&iterator))
		iterator.player->near_entrance = false;
	s_mp_globals *globals = g_4e9ae8;
	for (long i = 0; i < globals->teleporter_count; i++)
		teleporter_mark_players_near_entrance(&globals->teleporters[i]);
	iterator.data = data;
	iterator.index = NONE;
	iterator.datum_index = NONE;
	while (player_iterator_next_with_unit((long *)&iterator))
	{
		if (!(iterator.player->flags & (1 << _teleporter_player_near_entrance_bit)))
			iterator.player->has_teleported = false;
	}
	for (long j = 0; j < globals->teleporter_count; j++)
	{
		teleporter_update(&globals->teleporters[j]);
		globals = g_4e9ae8;
	}
}
