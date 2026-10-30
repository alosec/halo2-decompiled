/* H2_TRACE=state: one line per frame with the first player's unit (position,
   linear velocity, look direction, head marker, body forward), so a test driver can follow the game
   without looking at the screen (a fall through the floor shows as the
   height running away). Port-only; reads the decompiled game's own data. */
#include "unknown_11c920.h"
#include "globals.h"
#include "player_queries.h"
#include "object_markers.h"
#include "object_queries.h"
#include "hs.h"
#include "hs_scheduler.h"
#include "unit_requests.h"
#include "units.h"
extern "C" int _snprintf(char *destination, __SIZE_TYPE__ capacity, const char *format, ...);

extern "C" unsigned int host_h2_trace_flags(void);
extern "C" void host_h2_log(const char *message);
void function_ba1d0(long object_index, vector3f *linear_velocity, vector3f *angular_velocity);
extern s_record_pool *g_4e030c;   /* the dynamic lights */
extern "C" unsigned int host_h2_guest_command(char *buffer, unsigned int size);
bool __stdcall function_c5460(long object_index, long ignore_index, point3f const *position, point3f *result,
	long a5, real radius, long a7);
bool function_11c470(long trigger_volume_index, point3f const *point);
bool function_11c380(long trigger_volume_index, transform4x3f *matrix);
void __stdcall function_d6bc0(long object_index);
long function_c8f60(long unit_index, short seat_index);
long function_bb760(short name_index);
long function_1070d0(short group_index);
/* game_safe_to_save (0x226190) and the checks it makes */
bool function_226190(void);
bool function_226270(void);
long function_1ca2d0(bool unknown);
bool __stdcall function_fa9a0(long *value);
bool __stdcall function_10ca00(long *value);
bool function_1778d0(void);
bool __stdcall function_cc170(long *value);
bool __stdcall function_14df40(long *value);
bool __stdcall function_14deb0(long *value);
bool __stdcall function_f7a60(long *value);
bool __stdcall function_f7ca0(long *value);
void function_107840(long device_index, long unit_index);

/* the world ray test's result (the layout retail's callers use) */
struct s_collision_result_1697c0
{
	long type;
	real t;
	point3f point;
	byte unknown14[0x24 - 0x14];
	short unknown24;
	byte unknown26[0x5c - 0x26];
};
void function_11bed0(s_location *location, point3f const *point);
bool __stdcall function_1697c0(long flags, point3f const *point, vector3f const *vector,
	long ignore_object_index, long ignore_unit_index, s_collision_result_1697c0 *result);

/* every 150 frames, each script thread: its script's name, when it wakes,
   and the script function its innermost frame evaluates (the evaluate
   procedure's address, for guest_symbolize.py) */
/* the script action test latches (g_4ed284: +4 pressed, +8 and +0xc
   tested) and the script's input locks (+0x10: bit 0 camera control off),
   logged when they change */
static void report_action_latches(unsigned long frame)
{
	static unsigned long last[4];
	if (!g_4ed284)
		return;
	unsigned long const *words = (unsigned long const *)((byte const *)(void const *)g_4ed284 + 4);
	if (words[0] == last[0] && words[1] == last[1] && words[2] == last[2] && words[3] == last[3])
		return;
	last[0] = words[0]; last[1] = words[1]; last[2] = words[2]; last[3] = words[3];
	char line[120];
	_snprintf(line, sizeof(line), "h2-latch frame %lu pressed %08lx tested %08lx %08lx control %08lx", frame, words[0], words[1],
		words[2], words[3]);
	host_h2_log(line);
}

/* every 30 frames, the other bipeds within 6 units of the player (AI
   movement around the player: who walks into whom) */
static void report_bipeds(unsigned long frame, long player_unit, real const *player_position)
{
	if (frame % 10 || !g_4e0300 || !g_4e0300->data)
		return;
	for (long index = 0; index < g_4e0300->maximum_count; ++index)
	{
		byte const *header = (byte const *)g_4e0300->data + index * 0xc;
		byte *object = *(byte **)(header + 8);
		if (!*(short const *)header || !object || header[3] != 0 || (index & 0xffff) == (player_unit & 0xffff))
			continue;   /* type 0: biped */
		real const *p = (real const *)(object + 0x64);
		real dx = p[0] - player_position[0], dy = p[1] - player_position[1];
		if (dx * dx + dy * dy > 36.0f)
			continue;
		/* its actor (unit+0x12c; 0x888-byte records): +0x480 don't move,
		   +0x5d0 wants to move, +0x4ac movement mode, +0x4ec the planner's
		   destination, +0x5ec the movement input */
		char actor_text[120] = "";
		long actor_index = *(long const *)(object + 0x12c);
		if (actor_index != NONE && g_4f55f0 && g_4f55f0->data)
		{
			byte const *actor = (byte const *)g_4f55f0->data + (actor_index & 0xffff) * 0x888;
			real const *destination = (real const *)(actor + 0x4ec);
			real const *throttle = (real const *)(actor + 0x5ec);
			_snprintf(actor_text, sizeof(actor_text), " actor stop %d move %d mode %d destination %ld %ld %ld input %ld %ld %ld",
				actor[0x480], actor[0x5d0], (int)*(short const *)(actor + 0x4ac),
				(long)(destination[0] * 1000.0f), (long)(destination[1] * 1000.0f), (long)(destination[2] * 1000.0f),
				(long)(throttle[0] * 1000.0f), (long)(throttle[1] * 1000.0f), (long)(throttle[2] * 1000.0f));
		}
		char line[240];
		real const *forward = (real const *)(object + 0x70);
		_snprintf(line, sizeof(line), "h2-biped frame %lu index %ld pos %ld %ld %ld fwd %ld %ld%s", frame, index,
			(long)(p[0] * 1000.0f), (long)(p[1] * 1000.0f), (long)(p[2] * 1000.0f),
			(long)(forward[0] * 1000.0f), (long)(forward[1] * 1000.0f), actor_text);
		host_h2_log(line);
	}
}

static void report_scripts(unsigned long frame)
{
	report_action_latches(frame);
	if (frame % 150 || !g_4f9384 || !g_4f9384->data || !g_4e0350 || !g_4f9394 || !g_4f9394->data)
		return;
	s_hs_scenario const *scenario = (s_hs_scenario const *)g_4e0350;
	for (long index = 0; index < g_4f9384->maximum_count; ++index)
	{
		s_hs_thread const *thread = &((s_hs_thread const *)g_4f9384->data)[index];
		if (!thread->salt || thread->script_index == NONE || thread->script_index >= scenario->script_count)
			continue;
		/* the frames from the innermost out: each one's node, as
		   "function/node" (a script call shows as s<script>) */
		char chain[120] = "";
		long length = 0;
		long depth = 0;
		for (s_hs_frame const *f = thread->frame; f && depth < 8 && length < 100; f = f->next, ++depth)
		{
			long node_index = f->expression_node_index;
			if (node_index == NONE)
				continue;
			s_hs_expression_node const *node =
				&((s_hs_expression_node const *)g_4f9394->data)[node_index & 0xffff];
			length += _snprintf(chain + length, sizeof(chain) - length, " %ld/%lx",
				(long)node->function_index, (unsigned long)(node_index & 0xffff));
		}
		char line[240];
		_snprintf(line, sizeof(line), "h2-script frame %lu thread %ld script %s sleep %ld frames%s",
			frame, index, scenario->scripts[thread->script_index].name, (long)thread->sleep_until, chain);
		host_h2_log(line);
	}
}

/* test hooks, from "guest ..." lines of the H2_CONTROL file, so a test can
   drive a level's script to its end without a human's skill:
     teleport X Y Z      the first player, on foot (the player teleport's placement);
                         "outside the bsp" off the loaded bsp, "no floor"
                         over a void (no floor within 20 units)
     place X Y Z         the same, into a switch volume too (a lift shaft
                         nobody walks across: the switch happens as the
                         player update finds the unit inside)
     probe X Y Z         whether a point is in the loaded structure bsp
     volume I            trigger volume I's box in the world now (corner,
                         forward, left, up, scale): a volume on an object's
                         node moves with it
     deathless 1|0       keep the first player's body and shields full, and
                         set its cannot-die flag (object_cannot_die's bit 14
                         of +0x10a) against one-blow kills
     kill_enemies R      kill AI bipeds of other teams within R units
     units R             the units within R units: index, type, team, body
                         vitality, position
     enter_vehicle L     the first player into the nearest free seat with
                         seat label (string id) L, 0 for any, placed beside
                         its vehicle first (the seat request the script's
                         unit_enter_vehicle, function_11ade0, makes,
                         forced)
   Port-only test aids; nothing calls them unless a driver asks. */
static bool g_deathless;

static float parse_real(char const **text)
{
	char const *p = *text;
	while (*p == ' ')
		++p;
	bool negative = *p == '-';
	if (*p == '-' || *p == '+')
		++p;
	float value = 0.0f, scale = 0.1f;
	while (*p >= '0' && *p <= '9')
		value = value * 10.0f + (float)(*p++ - '0');
	if (*p == '.')
		for (++p; *p >= '0' && *p <= '9'; ++p, scale *= 0.1f)
			value += (float)(*p - '0') * scale;
	*text = p;
	return negative ? -value : value;
}

/* the object of a scenario object name (g_4e0350 +0x48: 0x24-byte names) */
static long named_object(char const *text)
{
	while (*text == ' ')
		++text;
	long count = *(long const *)((byte const *)g_4e0350 + 0x48);
	char const *names = *(char const * const *)((byte const *)g_4e0350 + 0x4c);
	for (long name = 0; name < count; ++name)
		if (!strcmp(names + name * 0x24, text))
			return function_bb760((short)name);
	return NONE;
}

static bool command_is(char const *text, char const *name, char const **rest)
{
	long i = 0;
	for (; name[i]; ++i)
		if (text[i] != name[i])
			return false;
	if (text[i] && text[i] != ' ')
		return false;
	*rest = text + i;
	return true;
}

static void run_test_commands(void)
{
	char command[128];
	while (host_h2_guest_command(command, sizeof(command)))
	{
		if (!g_4e8c24 || !g_4e8c24->data || !g_4e0300 || !g_4e0300->data || !g_4e0350)
			continue;   /* no level loaded (or one loading) */
		s_player_datum *player = NULL;
		for (long index = 0; index < g_4e8c24->maximum_count && !player; ++index)
		{
			s_player_datum *candidate = (s_player_datum *)(g_4e8c24->data + index * sizeof(s_player_datum));
			if (candidate->identifier && candidate->unit_index != NONE)
				player = candidate;
		}
		if (!player)
			continue;
		byte *unit = ((s_player_object_header *)g_4e0300->data)[player->unit_index & 0xffff].object;
		char const *rest;
		char line[240];
		bool place = command_is(command, "place", &rest);
		if (place || command_is(command, "teleport", &rest))
		{
			point3f point;
			point.x = parse_real(&rest);
			point.y = parse_real(&rest);
			point.z = parse_real(&rest);
			/* the player teleport refuses a point in one of the current
			   structure bsp's switch volumes (scenario +0x130, 14-byte
			   records: trigger volume, bsp) and then detaches the player
			   from its unit; refuse those points here instead */
			byte const *scenario = (byte const *)g_4e0350;
			bool refused = false;
			for (long i = 0; i < *(long const *)(scenario + 0x130) && !refused && !place; ++i)
			{
				byte const *trigger = *(byte const **)(scenario + 0x134) + i * 0xe;
				refused = *(short const *)(trigger + 2) == g_4686c4 && function_11c470(*(short const *)trigger, &point);
			}
			/* the player teleport's own placement (c5460, which finds a clear
			   spot near the point), without its detach on failure */
			s_location location;
			function_11bed0(&location, &point);
			bool away = location.leaf_index == NONE;   /* not in the loaded structure bsp */
			bool floorless = false;
			if (!away && !place)
			{
				/* nor over a void: a point of another bsp's room can still
				   lie in a leaf of this one, with no floor under it */
				vector3f down = {0.0f, 0.0f, -20.0f};
				s_collision_result_1697c0 floor;
				floor.unknown24 = NONE;
				floorless = !function_1697c0(0x84000d, &point, &down, player->unit_index, NONE, &floor);
			}
			/* seated: the vehicle moves instead (its bounding radius, +0x3c,
			   for the clear spot) */
			long moved = *(long const *)(unit + 0x14) == NONE ? player->unit_index : *(long const *)(unit + 0x14);
			byte const *mover = ((s_player_object_header *)g_4e0300->data)[moved & 0xffff].object;
			bool done = !refused && !away && !floorless && mover &&
				function_c5460(moved, NONE, &point, NULL, 0, moved == player->unit_index ? 1.0f : *(real const *)(mover + 0x3c), 0);
			_snprintf(line, sizeof(line), "h2-test teleport %s",
				refused ? "refused" : away ? "outside the bsp" : floorless ? "no floor" : done ? "done" : "failed");
			host_h2_log(line);
		}
		else if (command_is(command, "probe", &rest))
		{
			point3f point;
			point.x = parse_real(&rest);
			point.y = parse_real(&rest);
			point.z = parse_real(&rest);
			s_location location;
			function_11bed0(&location, &point);
			_snprintf(line, sizeof(line), "h2-test probe %s", location.leaf_index == NONE ? "outside" : "inside");
			host_h2_log(line);
		}
		else if (command_is(command, "volume", &rest))
		{
			long index = (long)parse_real(&rest);
			transform4x3f matrix;
			if (index >= 0 && index < *(long const *)((byte const *)g_4e0350 + 0x108) &&
				function_11c380(index, &matrix))
				_snprintf(line, sizeof(line),
					"h2-test volume %ld %.3f %.3f %.3f %.4f %.4f %.4f %.4f %.4f %.4f %.4f %.4f %.4f %.4f", index,
					matrix.position.x, matrix.position.y, matrix.position.z,
					matrix.forward.i, matrix.forward.j, matrix.forward.k, matrix.left.i, matrix.left.j,
					matrix.left.k, matrix.up.i, matrix.up.j, matrix.up.k, matrix.scale);
			else
				_snprintf(line, sizeof(line), "h2-test volume %ld none", index);
			host_h2_log(line);
		}
		else if (command_is(command, "deathless", &rest))
		{
			g_deathless = parse_real(&rest) != 0.0f;
			host_h2_log(g_deathless ? "h2-test deathless on" : "h2-test deathless off");
		}
		else if (command_is(command, "kill_enemies", &rest))
		{
			float radius = parse_real(&rest);
			short team = *(short const *)(unit + 0x138);
			real const *at = (real const *)(unit + 0x64);
			long killed = 0;
			for (long index = 0; index < g_4e0300->maximum_count; ++index)
			{
				byte const *header = (byte const *)g_4e0300->data + index * 0xc;
				byte *object = *(byte **)(header + 8);
				if (!*(short const *)header || !object || header[3] != 0 || *(long const *)(object + 0x12c) == NONE)
					continue;   /* AI bipeds only */
				if (*(short const *)(object + 0x138) == team || *(real const *)(object + 0xec) <= 0.0f)
					continue;
				real const *p = (real const *)(object + 0x64);
				real dx = p[0] - at[0], dy = p[1] - at[1], dz = p[2] - at[2];
				if (radius > 0.0f && dx * dx + dy * dy + dz * dz > radius * radius)
					continue;
				if (*(word const *)(object + 0x10a) & (1 << 14))
				{
					/* one that cannot die (a boss: 05b's Regret, 08b's Tartarus)
					   is beaten down to its pinned vitality instead
					   (ai_vitality_pinned, 0xdc310: body at most 0.0002) */
					*(real *)(object + 0xec) = 0.0001f;
					*(real *)(object + 0xf0) = 0.0f;
				}
				else
					function_d6bc0((long)(((dword)*(short const *)header << 16) | (dword)index));
				++killed;
			}
			_snprintf(line, sizeof(line), "h2-test killed %ld", killed);
			host_h2_log(line);
		}
		else if (command_is(command, "units", &rest))
		{
			float radius = parse_real(&rest);
			real const *at = (real const *)(unit + 0x64);
			for (long index = 0; index < g_4e0300->maximum_count; ++index)
			{
				byte const *header = (byte const *)g_4e0300->data + index * 0xc;
				byte *object = *(byte **)(header + 8);
				if (!*(short const *)header || !object || header[3] > 1)
					continue;   /* bipeds and vehicles */
				/* the bounding centre (+0x30) is in world space; the origin
				   (+0x64) of an attached unit is in its parent's (03b's Scarab
				   pilots) */
				real const *p = (real const *)(object + 0x30);
				real dx = p[0] - at[0], dy = p[1] - at[1], dz = p[2] - at[2];
				if (dx * dx + dy * dy + dz * dz > radius * radius)
					continue;
				_snprintf(line, sizeof(line), "h2-test unit %08lx type %d team %d body %.2f at %.2f %.2f %.2f parent %08lx",
					(long)(((dword)*(short const *)header << 16) | (dword)index), header[3],
					*(short const *)(object + 0x138), *(real const *)(object + 0xec), p[0], p[1], p[2],
					*(long const *)(object + 0x14));
				host_h2_log(line);
			}
			host_h2_log("h2-test units end");
		}
		else if (command_is(command, "enter_vehicle", &rest))
		{
			long label = 0;
			while (*rest == ' ')
				++rest;
			for (; *rest >= '0' && *rest <= '9'; ++rest)
				label = label * 10 + (*rest - '0');
			real const *at = (real const *)(unit + 0x64);
			long best_index = NONE, best_seat = NONE;
			real best = 1e30f;
			for (long index = 0; index < g_4e0300->maximum_count; ++index)
			{
				byte const *header = (byte const *)g_4e0300->data + index * 0xc;
				byte *object = *(byte **)(header + 8);
				if (!*(short const *)header || !object || header[3] != 1)
					continue;   /* vehicles only */
				real const *p = (real const *)(object + 0x64);
				real dx = p[0] - at[0], dy = p[1] - at[1], dz = p[2] - at[2];
				real distance = dx * dx + dy * dy + dz * dz;
				if (distance > best)
					continue;
				long vehicle_index = (long)(((dword)*(short const *)header << 16) | (dword)index);
				byte const *definition = g_4e3b44[*(long const *)object & 0xffff].bytes;
				long seat_count = *(long const *)(definition + 0x1c8);
				s_unit_seat_definition const *seats = *(s_unit_seat_definition * const *)(definition + 0x1cc);
				for (long seat = 0; seat < seat_count; ++seat)
					if ((!label || seats[seat].label == label) &&
						function_c8f60(vehicle_index, (short)seat) == NONE &&
						function_c8200(vehicle_index, (short)seat, player->unit_index))
					{
						best = distance, best_index = vehicle_index, best_seat = seat;
						break;
					}
			}
			bool requested = false;
			if (best_index != NONE && *(long const *)(unit + 0x14) == NONE)
			{
				byte const *vehicle = ((s_player_object_header *)g_4e0300->data)[best_index & 0xffff].object;
				point3f beside = *(point3f const *)(vehicle + 0x64);
				function_c5460(player->unit_index, NONE, &beside, NULL, 0, 1.0f, 0);
				s_unit_request request;
				memset(&request, 0, sizeof(request));
				request.type = 0x1c;
				request.type1c.object_index = best_index;
				request.type1c.seat_index = (short)best_seat;
				request.type1c.unknownb = true;   /* force: past function_c92c0's checks, as scripts load units */
				requested = function_e6900(player->unit_index, &request);
			}
			_snprintf(line, sizeof(line), "h2-test enter_vehicle %s", requested ? "requested" :
				best_index == NONE ? "no free seat" : "refused");
			host_h2_log(line);
		}
		else if (command_is(command, "operate", &rest))
		{
			/* a machine by its scenario object name: press the first powered control on its position group from
			   beside the control. Device +0x130 and +0x134 are the power
			   group and power, +0x13c and +0x140 the position group and
			   position (device_get_position, 0x2a4850, reads +0x140), as
			   the player's use action does (0x151c90) */
			long machine = named_object(rest);
			long pressed = 0;
			long group = NONE;
			if (machine != NONE)
				group = *(long const *)(((s_player_object_header *)g_4e0300->data)[machine & 0xffff].object + 0x13c);
			else
			{
				/* or a device group by name (scenario +0xa0: 0x28-byte records,
				   the name first), for device_group_get waits (05b's
				   gondola switches) */
				while (*rest == ' ')
					++rest;
				long count = *(long const *)((byte const *)g_4e0350 + 0xa0);
				char const *groups = *(char const * const *)((byte const *)g_4e0350 + 0xa4);
				for (long index = 0; index < count && group == NONE; ++index)
					if (!strcmp(groups + index * 0x28, rest))
						group = function_1070d0((short)index);
			}
			{
				for (long index = 0; index < g_4e0300->maximum_count && group != NONE && !pressed; ++index)
				{
					byte const *header = (byte const *)g_4e0300->data + index * 0xc;
					byte *object = *(byte **)(header + 8);
					if (!*(short const *)header || !object || header[3] != 8 || *(long const *)(object + 0x13c) != group ||
						*(real const *)(object + 0x134) <= 0.0f)
						continue;   /* powered controls of the group */
					long control = (long)(((dword)*(short const *)header << 16) | (dword)index);
					point3f beside = *(point3f const *)(object + 0x30);
					function_c5460(player->unit_index, NONE, &beside, NULL, 0, 1.0f, 0);
					function_107840(control, player->unit_index);
					++pressed;
				}
			}
			_snprintf(line, sizeof(line), "h2-test operate %s pressed %ld position %.2f", machine != NONE ? "found" : group != NONE ? "group" : "none",
				pressed, machine == NONE ? 0.0f : *(real const *)(((s_player_object_header *)g_4e0300->data)[machine & 0xffff].object + 0x140));
			host_h2_log(line);
		}
		else if (command_is(command, "exit_vehicle", &rest))
		{
			/* out of the seat (request 0x1e, as a dying unit leaves its seat
			   in 0xcaa60): a seated teleport moves the vehicle, which a place
			   on foot (08b's boss platform) cannot use */
			bool requested = false;
			if (*(long const *)(unit + 0x14) != NONE)
			{
				s_unit_request request;
				memset(&request, 0, sizeof(request));
				request.type = 0x1e;
				requested = function_e6900(player->unit_index, &request);
			}
			host_h2_log(requested ? "h2-test exit_vehicle requested" : "h2-test exit_vehicle none");
		}
		else if (command_is(command, "safe_to_save", &rest))
		{
			/* which of game_safe_to_save's checks says no (08b's Scarab
			   loop waits on it): bit per check, in 0x226190's order */
			long value = NONE;
			dword failing = 0;
			if (function_226270()) failing |= 1 << 0;
			if (function_1ca2d0(false) != NONE) failing |= 1 << 1;
			if (function_fa9a0(&value)) failing |= 1 << 2;
			if (function_10ca00(&value)) failing |= 1 << 3;
			if (function_1778d0()) failing |= 1 << 4;
			if (function_cc170(&value)) failing |= 1 << 5;
			if (function_14df40(&value)) failing |= 1 << 6;
			if (function_14deb0(&value)) failing |= 1 << 7;
			if (((byte const *)g_4e8c20)[5]) failing |= 1 << 8;
			if (function_f7a60(&value)) failing |= 1 << 10;
			if (function_f7ca0(&value)) failing |= 1 << 11;
			_snprintf(line, sizeof(line), "h2-test safe_to_save %d failing %03lx object %08lx", function_226190() ? 1 : 0,
				(unsigned long)failing, value);
			host_h2_log(line);
		}
		else if (command_is(command, "goto_object", &rest))
		{
			/* beside an object by its scenario object name (its bounding
			   centre, +0x30), for objects_distance_to_object waits (07a's
			   mausoleum holograms) */
			long object_index = named_object(rest);
			bool done = false;
			if (object_index != NONE)
			{
				point3f point = *(point3f const *)(((s_player_object_header *)g_4e0300->data)[object_index & 0xffff].object + 0x30);
				done = function_c5460(player->unit_index, NONE, &point, NULL, 0, 1.0f, 0);
			}
			host_h2_log(object_index == NONE ? "h2-test goto_object none" : done ? "h2-test goto_object done" : "h2-test goto_object failed");
		}
		else if (command_is(command, "goto_unit", &rest))
		{
			/* beside a unit by the datum index "units" lists, for waits on
			   the distance to a command script's own actor (03b's Scarab
			   pilots wait for the player within 7 and then 3 units) */
			dword datum = 0;
			while (*rest == ' ')
				++rest;
			for (; (*rest >= '0' && *rest <= '9') || (*rest >= 'a' && *rest <= 'f'); ++rest)
				datum = datum * 16 + (dword)(*rest <= '9' ? *rest - '0' : *rest - 'a' + 10);
			byte const *header = (byte const *)g_4e0300->data + (datum & 0xffff) * 0xc;
			byte const *object = (long)(datum & 0xffff) < g_4e0300->maximum_count &&
				*(word const *)header == (word)(datum >> 16) ? *(byte * const *)(header + 8) : NULL;
			bool done = false;
			if (object)
			{
				point3f point = *(point3f const *)(object + 0x30);
				done = function_c5460(player->unit_index, NONE, &point, NULL, 0, 1.0f, 0);
			}
			host_h2_log(!object ? "h2-test goto_unit none" : done ? "h2-test goto_unit done" : "h2-test goto_unit failed");
		}
		else if (command_is(command, "kill_player", &rest))
		{
			/* clears cannot-take-damage (+0x10a bit 7) and cannot-die (bit
			   14) first: cinematics and scripts set them on the player */
			word flags = *(word *)(unit + 0x10a);
			*(word *)(unit + 0x10a) &= (word)~((1 << 7) | (1 << 14));
			function_d6bc0(player->unit_index);
			_snprintf(line, sizeof(line), "h2-test kill_player flags %04x body %.2f", flags, *(real const *)(unit + 0xec));
			host_h2_log(line);
		}
		else if (command_is(command, "destroy", &rest))
		{
			/* an object by its scenario object name (04b's cables), unless a
			   script made it unable to take damage (+0x10a bit 7, 0xd87e0) */
			long object_index = named_object(rest);
			byte const *object = object_index == NONE ? NULL :
				((s_player_object_header *)g_4e0300->data)[object_index & 0xffff].object;
			bool invulnerable = object && (*(word const *)(object + 0x10a) & 0x80);
			if (object && !invulnerable)
				function_d6bc0(object_index);
			host_h2_log(!object ? "h2-test destroy none" : invulnerable ? "h2-test destroy invulnerable" : "h2-test destroy done");
		}
	}
	if (g_deathless && g_4e8c24 && g_4e8c24->data && g_4e0300 && g_4e0300->data)
		for (long index = 0; index < g_4e8c24->maximum_count; ++index)
		{
			s_player_datum *player = (s_player_datum *)(g_4e8c24->data + index * sizeof(s_player_datum));
			if (!player->identifier || player->unit_index == NONE)
				continue;
			byte *unit = ((s_player_object_header *)g_4e0300->data)[player->unit_index & 0xffff].object;
			if (unit && *(real *)(unit + 0xec) > 0.0f)
			{
				*(word *)(unit + 0x10a) |= 1 << 14;
				/* full shields are not depleted: clear shield_depleted (bit 3,
				   set by 0xd6a70), which recharging would have cleared; left
				   set, game_safe_to_save (0x14deb0) never passes (08b's Scarab) */
				*(word *)(unit + 0x10a) &= (word)~(1 << 3);
				*(real *)(unit + 0xec) = 1.0f;
				*(real *)(unit + 0xf0) = 1.0f;
			}
		}
}

extern "C" void h2_port_report_state(void)
{
	static bool enabled = (host_h2_trace_flags() & 2) != 0;
	static unsigned long frame;
	++frame;
	run_test_commands();
	if (!enabled)
		return;
	report_scripts(frame);
	if (!g_4e8c24 || !g_4e8c24->data || !g_4e0300 || !g_4e0300->data)
	{
		if (frame % 30 == 0)
		{
			char line[64];
			_snprintf(line, sizeof(line), "h2-state frame %lu none", frame);
			host_h2_log(line);
		}
		return;
	}
	bool found = false;
	for (long index = 0; index < g_4e8c24->maximum_count && !found; ++index)
	{
		s_player_datum *player = (s_player_datum *)(g_4e8c24->data + index * sizeof(s_player_datum));
		if (!player->identifier || player->unit_index == NONE)
			continue;
		byte *unit = ((s_player_object_header *)g_4e0300->data)[player->unit_index & 0xffff].object;
		if (!unit)
			continue;
		/* seated, a unit's position is its seat's (parent relative): the
		   bounding centre (+0x30, which the volume tests use) is in the world */
		real const *position = (real const *)(unit + (*(long const *)(unit + 0x14) == NONE ? 0x64 : 0x30));
		real const *look = (real const *)(unit + 0x18c);
		real const *forward = (real const *)(unit + 0x70);
		vector3f velocity = {0.0f, 0.0f, 0.0f};
		function_ba1d0(player->unit_index, &velocity, 0);
		/* the head marker: where the game's "is the player looking at" checks
		   (function_cc2b0) measure from */
		s_object_marker marker;
		point3f head = {position[0], position[1], position[2]};
		if (function_b8d30(player->unit_index, 0x4000095, &marker, 1, false))
			head = marker.matrix.position;
		/* the ground under the unit: a ray 50 units down from half a unit up,
		   with the flags bipeds test the ground with; a fall through the floor
		   shows as the unit below the ground the ray still finds */
		point3f ray_start = {position[0], position[1], position[2] + 0.5f};
		vector3f ray = {0.0f, 0.0f, -50.0f};
		s_collision_result_1697c0 ground;
		ground.unknown24 = NONE;
		bool grounded = function_1697c0(0x84000d, &ray_start, &ray, player->unit_index, NONE, &ground);
		/* the structure bsp leaf the position is in (NONE: outside the world) */
		s_location location;
		function_11bed0(&location, (point3f const *)position);
		char line[320];
		/* thousandths: the guest's formatter is integer-only */
		#define MILLI(value) (long)((value) * 1000.0f)
		_snprintf(line, sizeof(line), "h2-state frame %lu unit %08lx pos %ld %ld %ld vel %ld %ld %ld look %ld %ld %ld head %ld %ld %ld fwd %ld %ld %ld ground %ld leaf %ld cluster %ld lights %ld flashlight %ld health %ld shield %ld bsp %ld shield_recent %ld",
			frame, (unsigned long)player->unit_index, MILLI(position[0]), MILLI(position[1]), MILLI(position[2]),
			MILLI(velocity.i), MILLI(velocity.j), MILLI(velocity.k), MILLI(look[0]), MILLI(look[1]), MILLI(look[2]),
			MILLI(head.x), MILLI(head.y), MILLI(head.z), MILLI(forward[0]), MILLI(forward[1]), MILLI(forward[2]),
			grounded ? MILLI(ground.point.z) : -999999L, (long)location.leaf_index, (long)location.cluster_index,
			g_4e030c ? (long)g_4e030c->actual_count : -1L,
			MILLI(*(real const *)(unit + 0x264)),   /* the flashlight's power, 0..1 */
			MILLI(*(real const *)(unit + 0xec)),   /* body vitality, 0 when dead */
			MILLI(*(real const *)(unit + 0xf0)),   /* shield vitality */
			(long)g_4686c4,   /* the structure bsp index */
			MILLI(*(real const *)(unit + 0xf4)));   /* recent shield damage: the shield effect draws while above 0 */
		host_h2_log(line);
		report_bipeds(frame, player->unit_index, position);
		/* in a vehicle the unit's position is its seat's, relative to the
		   vehicle: also report the root object's (world) position */
		#define OBJECT(index) (((s_player_object_header *)g_4e0300->data)[(index) & 0xffff].object)
		long root = player->unit_index;
		while (*(long const *)(OBJECT(root) + 0x14) != NONE)   /* parent object */
			root = *(long const *)(OBJECT(root) + 0x14);
		if (root != player->unit_index)
		{
			real const *origin = (real const *)(OBJECT(root) + 0x64);
			_snprintf(line, sizeof(line), "h2-state frame %lu vehicle %08lx pos %ld %ld %ld",
				frame, (unsigned long)root, MILLI(origin[0]), MILLI(origin[1]), MILLI(origin[2]));
			host_h2_log(line);
		}
		#undef OBJECT
		#undef MILLI
		found = true;
	}
	if (!found && frame % 30 == 0)
	{
		char line[64];
		_snprintf(line, sizeof(line), "h2-state frame %lu none", frame);
		host_h2_log(line);
	}
}


/* the canary of -fstack-protector-all (H2_STACK_PROTECT=1 builds): a
   function whose frame was overrun calls __stack_chk_fail on return */
extern "C" unsigned long __stack_chk_guard = 0x5a17c0deUL;
extern "C" __attribute__((noreturn)) void __stack_chk_fail(void)
{
	char line[96];
	_snprintf(line, sizeof(line), "h2-state stack overrun in the function at 0x%08lx",
		(unsigned long)__builtin_return_address(0));
	host_h2_log(line);
	__builtin_trap();
}
