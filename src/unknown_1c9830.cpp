// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_1C9830.CPP: alliances between teams

LTCG dropped the last argument of function_1df6c0 (always false) and
the bool * of function_1df820.

Two team-by-team bit vectors hold the alliances: ally_bits (read by
function_1df5d0) and peace_bits (function_1df560 is its inverse). A
broken alliance keeps its ally bit but loses its peace bit until enough
time passes without incidents. */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_123b30.h"
#include <string.h>

enum
{
	k_maximum_game_teams = 16,
	k_maximum_game_allegiances = 8
};

enum
{
	_game_mode_campaign = 1,
	_game_mode_multiplayer = 2
};

struct s_type_e695f2
{
	short team_a;
	short team_b;
	short incident_threshold;
	short incident_decay_ticks;
	bool team_b_provokes;
	bool team_a_provokes;
	bool broken;
	bool changed;
	short incidents;
	short decay_timer;
	long last_incident_time;
};

struct s_game_allegiance_globals
{
	short allegiance_count;
	byte unknown02[2];
	s_type_e695f2 allegiances[k_maximum_game_allegiances];
	dword ally_bits[k_maximum_game_teams * k_maximum_game_teams / 32];
	dword peace_bits[k_maximum_game_teams * k_maximum_game_teams / 32];
};

s_game_allegiance_globals *g_4f55ec;

bool function_0bfe60(const dword *flags, long bit);
bool function_15e020(short a, short b);
void __stdcall function_1c9830(short team_a, short team_b, bool broken, bool removed);

PRIVATE void __stdcall function_1df9c0(s_type_e695f2 *volatile allegiance, bool broken, bool removed);

// @retail 0x1df460
void function_1df460(void)
{
	g_4f55ec = (s_game_allegiance_globals *)function_123d40("game allegiance globals", "game_allegiance_globals",
		sizeof(s_game_allegiance_globals));
	memset(g_4f55ec, 0, sizeof(s_game_allegiance_globals));
}

// @retail 0x1df4b0
void function_1df4b0(void)
{
	memset(g_4f55ec, 0, sizeof(s_game_allegiance_globals));
	for (short team = 0; team < k_maximum_game_teams; team++)
	{
		long bit = team * k_maximum_game_teams + team;
		g_4f55ec->peace_bits[bit >> 5] |= 1 << (bit & 31);
	}
}

// @retail 0x1df500
void function_1df500(void)
{
	s_type_e695f2 *allegiance = g_4f55ec->allegiances;

	for (short i = 0; i < g_4f55ec->allegiance_count; i++, allegiance++)
	{
		if (allegiance->decay_timer > 0 && --allegiance->decay_timer == 0)
		{
			if (--allegiance->incidents == 0)
				function_1df9c0(allegiance, false, false);
			else
				allegiance->decay_timer = allegiance->incident_decay_ticks;
		}
	}
}

// @retail 0x1df560
bool function_1df560(short team_a, short team_b)
{
	bool result = true;

	if (team_a == NONE || team_b == NONE)
		return true;

	long mode = g_4e6948->state;

	if (mode == _game_mode_campaign)
	{
		if (team_a >= 0 && team_a < k_maximum_game_teams && team_b >= 0 && team_b < k_maximum_game_teams)
		{
			long bit = team_a * k_maximum_game_teams + team_b;
			result = !function_0bfe60(g_4f55ec->peace_bits, bit);
		}
	}
	else if (mode == _game_mode_multiplayer)
	{
		result = function_15e020(team_a, team_b);
	}
	else
	{
		result = team_a != team_b;
	}
	return result;
}

// @retail 0x1df5d0
bool function_1df5d0(short team_a, short team_b)
{
	bool result = false;

	if (team_a == NONE || team_b == NONE)
		return false;

	long mode = g_4e6948->state;

	if (mode == _game_mode_campaign)
	{
		if (team_a >= 0 && team_a < k_maximum_game_teams && team_b >= 0 && team_b < k_maximum_game_teams)
		{
			long bit = team_a * k_maximum_game_teams + team_b;
			result = function_0bfe60(g_4f55ec->ally_bits, bit);
		}
	}
	else if (mode == _game_mode_multiplayer)
	{
		result = !function_15e020(team_a, team_b);
	}
	else
	{
		result = team_a == team_b;
	}
	return result;
}

/* allies whose alliance is broken */
// @retail 0x1df640
bool function_1df640(short team_a, short team_b)
{
	bool result = false;

	if (team_a == NONE || team_b == NONE || team_a == team_b)
		return false;

	if (g_4e6948->state == _game_mode_campaign &&
		team_a >= 0 && team_a < k_maximum_game_teams && team_b >= 0 && team_b < k_maximum_game_teams)
	{
		if (!function_0bfe60(g_4f55ec->ally_bits, team_a * k_maximum_game_teams + team_b) ||
			function_0bfe60(g_4f55ec->peace_bits, team_a * k_maximum_game_teams + team_b))
			return false;
		result = true;
	}
	return result;
}

// @retail 0x1df6c0
void function_1df6c0(short team_a, short team_b, bool team_b_provokes, bool team_a_provokes,
	short incident_threshold, short incident_decay_ticks)
{
	if (g_4e6948->state != _game_mode_campaign)
		return;

	s_game_allegiance_globals *globals = g_4f55ec;
	short count = globals->allegiance_count;
	short i;

	s_type_e695f2 *allegiance = globals->allegiances;

	for (i = 0; i < globals->allegiance_count; i++, allegiance++)
	{
		if (allegiance->team_a == team_a && allegiance->team_b == team_b ||
			allegiance->team_b == team_a && allegiance->team_a == team_b)
			break;
	}

	if (i >= count && count < k_maximum_game_allegiances)
		i = count++, globals->allegiance_count = count;

	if (i < globals->allegiance_count)
	{
		allegiance = &g_4f55ec->allegiances[i];

		allegiance->team_b_provokes = team_b_provokes;
		allegiance->team_a_provokes = team_a_provokes;
		allegiance->team_a = team_a;
		allegiance->team_b = team_b;
		allegiance->incident_threshold = incident_threshold;
		allegiance->incident_decay_ticks = incident_decay_ticks;
		allegiance->incidents = 0;
		allegiance->decay_timer = 0;
		allegiance->broken = true;
		function_1df9c0(allegiance, false, false);
		allegiance->changed = false;
	}
}

// @retail 0x1df770
bool function_1df770(short volatile team_a, short volatile team_b)
{
	short first_team = team_a;
	short const *second_reference = (short const *)&team_b;
	short second_team = *second_reference;
	bool result = false;

	if (g_4e6948->state == _game_mode_campaign)
	{
		s_type_e695f2 *allegiance = g_4f55ec->allegiances;

		for (short i = 0; i < g_4f55ec->allegiance_count; i++, allegiance++)
		{
			if (allegiance->team_a == first_team && allegiance->team_b == second_team ||
				allegiance->team_b == first_team && allegiance->team_a == second_team)
			{
				function_1df9c0(allegiance, true, true);
				g_4f55ec->allegiance_count--;
				if (g_4f55ec->allegiance_count > i)
					g_4f55ec->allegiances[i] = g_4f55ec->allegiances[g_4f55ec->allegiance_count];
				result = true;
				break;
			}
		}
	}
	return result;
}

// @retail 0x1df820
bool __stdcall function_1df820(volatile short team_a, volatile short team_b, short incident_type)
{
	bool local_4 = false;
	{
		s_game_options_view *local_0 = g_4e6948;
		short local_1 = *(short const *)&team_a;
		short const *local_2 = (short const *)&team_b;
		if (*(volatile long *)&local_0->state == _game_mode_campaign)
		{

			s_game_allegiance_globals *globals = g_4f55ec;
			s_type_e695f2 *allegiance = globals->allegiances;

			for (short i = 0; i < globals->allegiance_count; i++, allegiance++)
			{

				if (allegiance->team_a == local_1 && allegiance->team_b == (*local_2) && *(volatile bool *)&allegiance->team_a_provokes ||
					allegiance->team_b == local_1 && allegiance->team_a == (*local_2) && *(volatile bool *)&allegiance->team_b_provokes)
				{
					s_game_time_globals *game_time = g_510c54;
					long local_3 = game_time->game_time;
					short delta = 0;
					real seconds = game_time->field_2_3 * 0.2f;
					long ticks;

					__asm
					{
						fld seconds
						fistp ticks
					}

					if (local_3 - allegiance->last_incident_time > ticks)
					{

						allegiance->last_incident_time = local_3;
						switch (incident_type)
						{
						case 0: delta = 1; break;
						case 1: delta = 3; break;
						case 2: delta = -1; break;
						}
						allegiance->incidents += delta;
						if (allegiance->incident_decay_ticks != NONE)
							allegiance->decay_timer = allegiance->incident_decay_ticks;
						if (allegiance->incident_threshold != NONE && allegiance->incidents >= allegiance->incident_threshold)
						{
							function_1df9c0(allegiance, true, false);
							function_1c9830(allegiance->team_a, allegiance->team_b, true, false);
							{
								local_4 = true;
								goto local_5;
							}
						}
						goto local_5;
					}
					local_1 = *(short const *)&team_a;
				}
			}
		}
		goto local_5;
	}
local_5:
	return local_4;
}

/* an incident between the teams restarts the decay of their grudge */
// @retail 0x1df950
void function_1df950(short team_a, short team_b)
{
	if (g_4e6948->state != _game_mode_campaign)
		return;

	s_type_e695f2 *allegiance = g_4f55ec->allegiances;

	for (short i = 0; i < g_4f55ec->allegiance_count; i++, allegiance++)
	{
		if (allegiance->team_a == team_a && allegiance->team_b == team_b && allegiance->team_a_provokes ||
			allegiance->team_b == team_a && allegiance->team_a == team_b && allegiance->team_b_provokes)
		{
			if (allegiance->incidents > 0 && allegiance->incident_decay_ticks != NONE)
				allegiance->decay_timer = allegiance->incident_decay_ticks;
			return;
		}
	}
}

// @retail 0x1dfaa0
PRIVATE void function_1dfaa0(dword *vector, long bit, bool value)
{
	if (value)
		vector[bit >> 5] |= 1 << (bit & 31);
	else
		vector[bit >> 5] &= ~(1 << (bit & 31));
}

/* All 213 bytes match with the standard convention. Without this marker,
   the same body takes the pointer in a register and returns ret 8.
   Retail has no absolute reference to this address; its four callers
   (0x1df500, 0x1df6c0, 0x1df770, 0x1df820) push all three arguments.
   Direct pointer copies, parameter references and a force-inline helper
   all used ret 8. Keeping the earlier guard held ret 12 but gave 217 bytes. */
// @retail 0x1df9c0 standard
PRIVATE void __stdcall function_1df9c0(s_type_e695f2 *volatile allegiance, bool broken, bool removed)
{
	s_type_e695f2 *local_0 = *(s_type_e695f2 *const *)&allegiance;
	if (!removed && local_0->broken == broken)
		return;

	local_0->broken = broken;
	if (local_0->team_a < k_maximum_game_teams && local_0->team_b < k_maximum_game_teams)
	{
		function_1dfaa0(g_4f55ec->ally_bits, local_0->team_a * k_maximum_game_teams + local_0->team_b, !removed);
		function_1dfaa0(g_4f55ec->ally_bits, local_0->team_b * k_maximum_game_teams + local_0->team_a, !removed);
		function_1dfaa0(g_4f55ec->peace_bits, local_0->team_a * k_maximum_game_teams + local_0->team_b, !broken);
		function_1dfaa0(g_4f55ec->peace_bits, local_0->team_b * k_maximum_game_teams + local_0->team_a, !broken);
	}
	local_0->changed = true;
	function_1c9830(local_0->team_a, local_0->team_b, broken, removed);
}

#include "data_array.h"
#include "unknown_1fb7e0.h"

struct s_1c9830
{
 byte field_0[0x12];
 short field_12;
 long field_14;
 byte field_18[0x50 - 0x18];
};
struct s_1c9831
{
 byte field_0[8];
 long field_8;
 byte field_c[8];
 long field_14;
 long field_18;
 byte field_1c[4];
 short field_20;
 byte field_22;
 bool field_23;
 bool field_24;
 byte field_25[0x3c - 0x25];
 bool field_3c;
 byte field_3d[0xc4 - 0x3d];
};
struct s_1c9832
{
 long field_0;
 long field_4;
 byte field_8[0x14 - 8];
 long field_14;
 byte field_18[0x34 - 0x18];
 long field_34;
 long field_38;
};
struct s_1c9833
{
 byte field_0[0x70];
 byte field_70[0x124 - 0x70];
};
struct s_1c9834
{
 s_1c9830 *field_0;
 s_record_pool_iterator field_4;
};
real __stdcall function_265d30(long actor_index, long prop_index);

// @retail 0x1c9830
void __stdcall function_1c9830(short arg_0, short arg_1, bool arg_2, bool arg_3)
{
 if (g_4f55d0->active)
 {
  s_1c9834 local_0;
  local_0.field_4.data = g_502420;
  local_0.field_4.index = NONE;
  while (g_4f55d0->active)
  {
   local_0.field_0 = (s_1c9830 *)data_iterator_next_calling(&local_0.field_4);
   if (!local_0.field_0)
    break;
   short local_1;
   if (local_0.field_0->field_12 == arg_0)
    local_1 = arg_1;
   else if (local_0.field_0->field_12 == arg_1)
    local_1 = arg_0;
   else
    continue;
   if (local_1 == NONE)
    continue;
   long local_2 = ((s_1c9830 *)g_502420->data)[local_0.field_4.datum_index & 0xffff].field_14;
   while (local_2 != NONE)
   {
    s_1c9831 *local_3 = &((s_1c9831 *)g_50241c->data)[local_2 & 0xffff];
    long local_4 = local_2;
    local_2 = local_3->field_14;
    if (local_3->field_20 != local_1)
     continue;
    if (!arg_3)
     local_3->field_24 = true;
    local_3->field_23 = arg_2;
    local_3->field_3c = true;
    if (!arg_2 && !arg_3)
     function_20ba60(0xc4, local_3->field_8, NONE, NONE, NONE, NULL);
    if (local_3->field_23)
    {
     long local_5 = ((s_1c9831 *)g_50241c->data)[local_4 & 0xffff].field_18;
     while (local_5 != NONE)
     {
      s_1c9832 *local_6 = &((s_1c9832 *)g_502418->data)[local_5 & 0xffff];
      long local_7 = local_5;
      local_5 = local_6->field_34;
      s_1c9833 *local_8 = local_6->field_14 == NONE ? NULL :
       &((s_1c9833 *)g_502414->data)[local_6->field_14 & 0xffff];
      byte *local_9 = local_8 ? local_8->field_70 : NULL;
      if (local_9 && local_3->field_23)
       *(real *)(local_9 + 0x3c) = function_265d30(local_6->field_4, local_7);
     }
    }
   }
  }
 }
}
