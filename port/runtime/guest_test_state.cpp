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

extern "C" void h2_port_report_state(void)
{
	static bool enabled = (host_h2_trace_flags() & 2) != 0;
	static unsigned long frame;
	++frame;
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
		_snprintf(line, sizeof(line), "h2-state frame %lu unit %08lx pos %ld %ld %ld vel %ld %ld %ld look %ld %ld %ld head %ld %ld %ld fwd %ld %ld %ld ground %ld leaf %ld cluster %ld lights %ld flashlight %ld health %ld",
			frame, (unsigned long)player->unit_index, MILLI(position[0]), MILLI(position[1]), MILLI(position[2]),
			MILLI(velocity.i), MILLI(velocity.j), MILLI(velocity.k), MILLI(look[0]), MILLI(look[1]), MILLI(look[2]),
			MILLI(head.x), MILLI(head.y), MILLI(head.z), MILLI(forward[0]), MILLI(forward[1]), MILLI(forward[2]),
			grounded ? MILLI(ground.point.z) : -999999L, (long)location.leaf_index, (long)location.cluster_index,
			g_4e030c ? (long)g_4e030c->actual_count : -1L,
			MILLI(*(real const *)(unit + 0x264)),   /* the flashlight's power, 0..1 */
			MILLI(*(real const *)(unit + 0xec)));   /* body vitality, 0 when dead */
		host_h2_log(line);
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
