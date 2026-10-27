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
extern "C" int _snprintf(char *destination, __SIZE_TYPE__ capacity, const char *format, ...);

extern "C" unsigned int host_h2_trace_flags(void);
extern "C" void host_h2_log(const char *message);
void function_ba1d0(long object_index, vector3f *linear_velocity, vector3f *angular_velocity);
extern s_record_pool *g_4e030c;   /* the dynamic lights */
extern "C" unsigned int host_h2_guest_command(char *buffer, unsigned int size);
bool __stdcall function_c5460(long object_index, long ignore_index, point3f const *position, point3f *result,
	long a5, real radius, long a7);
bool function_11c470(long trigger_volume_index, point3f const *point);
void __stdcall function_d6bc0(long object_index);

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
     teleport X Y Z      the first player, on foot (the player teleport's placement)
     deathless 1|0       keep the first player's body and shields full, and
                         set its cannot-die flag (object_cannot_die's bit 14
                         of +0x10a) against one-blow kills
     kill_enemies R      kill AI bipeds of other teams within R units
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
		if (!g_4e8c24 || !g_4e8c24->data || !g_4e0300 || !g_4e0300->data)
			continue;
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
		char line[160];
		if (command_is(command, "teleport", &rest))
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
			for (long i = 0; i < *(long const *)(scenario + 0x130) && !refused; ++i)
			{
				byte const *trigger = *(byte const **)(scenario + 0x134) + i * 0xe;
				refused = *(short const *)(trigger + 2) == g_4686c4 && function_11c470(*(short const *)trigger, &point);
			}
			/* the player teleport's own placement (c5460, which finds a clear
			   spot near the point), without its detach on failure */
			s_location location;
			function_11bed0(&location, &point);
			bool away = location.leaf_index == NONE;   /* not in the loaded structure bsp */
			bool done = !refused && !away && *(long const *)(unit + 0x14) == NONE &&
				function_c5460(player->unit_index, NONE, &point, NULL, 0, 1.0f, 0);
			_snprintf(line, sizeof(line), "h2-test teleport %s",
				refused ? "refused" : away ? "outside the bsp" : done ? "done" : "failed");
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
				function_d6bc0((long)(((dword)*(short const *)header << 16) | (dword)index));
				++killed;
			}
			_snprintf(line, sizeof(line), "h2-test killed %ld", killed);
			host_h2_log(line);
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
		real const *position = (real const *)(unit + 0x64);
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
		_snprintf(line, sizeof(line), "h2-state frame %lu unit %08lx pos %ld %ld %ld vel %ld %ld %ld look %ld %ld %ld head %ld %ld %ld fwd %ld %ld %ld ground %ld leaf %ld cluster %ld lights %ld flashlight %ld health %ld shield %ld bsp %ld",
			frame, (unsigned long)player->unit_index, MILLI(position[0]), MILLI(position[1]), MILLI(position[2]),
			MILLI(velocity.i), MILLI(velocity.j), MILLI(velocity.k), MILLI(look[0]), MILLI(look[1]), MILLI(look[2]),
			MILLI(head.x), MILLI(head.y), MILLI(head.z), MILLI(forward[0]), MILLI(forward[1]), MILLI(forward[2]),
			grounded ? MILLI(ground.point.z) : -999999L, (long)location.leaf_index, (long)location.cluster_index,
			g_4e030c ? (long)g_4e030c->actual_count : -1L,
			MILLI(*(real const *)(unit + 0x264)),   /* the flashlight's power, 0..1 */
			MILLI(*(real const *)(unit + 0xec)),   /* body vitality, 0 when dead */
			MILLI(*(real const *)(unit + 0xf0)),   /* shield vitality */
			(long)g_4686c4);   /* the structure bsp index */
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
