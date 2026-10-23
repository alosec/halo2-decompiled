// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_087830.CPP: whether an update of the simulation's state is within
   one quantum of the state it would replace (lane D) */

#include "unknown_11c920.h"
#include <math.h>
#include "network_message_types.h"

/* the quantized values compared here */
struct s_simulation_quantized_state
{
	long unknown00;
	real angle04;
	real angle08;
	real value0c;
	real value10;
	real value14;
	real value18;
	byte unknown1c[0x50 - 0x1c];
	real value50;
	real value54;
};

/* a player action: takes the update's values when every one of them is within a quantum of
   the state's (the first angle also across the turn) */
// @retail 0x87830
bool function_87830(s_player_action *new_action, s_player_action *action)
{
	s_simulation_quantized_state *state = (s_simulation_quantized_state *)action;
	s_simulation_quantized_state const *update = (s_simulation_quantized_state const *)new_action;
	bool result;
	real difference = (real)fabs(state->angle04 - update->angle04);
	if ((0.00076708407f > difference || 0.00076708407f > (real)fabs(difference - 6.2831855f)) &&
		0.0015343555f > (real)fabs(state->angle08 - update->angle08) &&
		0.06666667f > (real)fabs(state->value0c - update->value0c) &&
		0.06666667f > (real)fabs(state->value10 - update->value10) &&
		0.032258064f > (real)fabs(state->value14 - update->value14) &&
		0.032258064f > (real)fabs(state->value18 - update->value18) &&
		0.06666667f > (real)fabs(state->value50 - update->value50) &&
		0.06666667f > (real)fabs(state->value54 - update->value54))
	{
		result = true;
		state->angle04 = update->angle04;
		state->angle08 = update->angle08;
		state->value0c = update->value0c;
		state->value10 = update->value10;
		state->value14 = update->value14;
		state->value18 = update->value18;
		state->value50 = update->value50;
		state->value54 = update->value54;
	}
	else
	{
		result = false;
	}
	return result;
}

#include "unknown_067e10.h"
#include "bitstream.h"
#include <string.h>

byte function_07ca70(s_bitstream *stream, void *destination);
void function_07c5a0(s_bitstream *stream, const void *source);

// @retail 0x87930
void function_87930(s_bitstream *stream, const s_simulation_player_update *update)
{
	stream_write_checked(stream, update->player_index, 4);
	function_1955d0(stream, update->key, 96);
	stream_write_checked(stream, update->type, 3);
	if (update->type == 3)
	{
		stream_write_bit(stream, update->field_2_2);
		if (!update->field_2_2)
		{
			function_1955d0(stream, &update->machine, 48);
			stream_write_checked(stream, update->controller_index, 2);
			stream_write_checked(stream, update->unknown20, 2);
		}
	}
	if (update->type == 3 || update->type == 4)
		function_07c5a0(stream, update->configuration);
	if (update->type == 1)
	{
		stream_write_checked(stream, update->other_player_index, 4);
		function_1955d0(stream, update->other_key, 96);
	}
}

// @retail 0x87ac0
bool function_87ac0(s_bitstream *stream, s_simulation_player_update *update)
{
	bool result = true;
	update->player_index = function_1959c0(stream, 4);
	function_195820(stream, update->key, 96);
	update->type = function_1959c0(stream, 3);
	if (update->player_index < 0 || update->player_index >= 16 || update->type < 0 || update->type >= 5)
		goto failed;
	if (update->type == 3)
	{
		update->field_2_2 = function_1957d0(stream);
		if (!update->field_2_2)
		{
			function_195820(stream, &update->machine, 48);
			update->controller_index = function_1959c0(stream, 2);
			update->unknown20 = function_1959c0(stream, 2);
			result = update->controller_index >= 0 && update->controller_index < 4 &&
				update->unknown20 >= 0 && update->unknown20 < 4;
		}
		else
		{
			memset(&update->machine, 0, sizeof(update->machine));
			update->controller_index = NONE;
			update->unknown20 = NONE;
		}
	}
	if (update->type == 3 || update->type == 4)
		result = result && function_07ca70(stream, update->configuration);
	if (update->type == 1)
	{
		update->other_player_index = function_1959c0(stream, 4);
		function_195820(stream, update->other_key, 96);
		if (!result || update->other_player_index < 0 || update->other_player_index >= 16 ||
			update->other_player_index == update->player_index)
			goto failed;
		result = true;
	}
	goto done;
failed:
	result = false;
done:
	return result;
}

#include "globals.h"

struct s_action_codec_view
{
 dword control;
 real yaw, pitch;
 real forward, strafe, first, second;
 word flags;
 short index;
 signed char weapon, grenade;
 short slot, seat;
 byte unknown26[2];
 short target_type;
 union { short target_slot; byte target_byte; };
 long target;
 long value30;
 long value34;
 long value38;
 long player;
 byte unknown40[12];
 short flags4c;
 byte unknown4e[2];
 real value50;
 real value54;
 byte unknown58[4];
};
void player_action_initialize(s_player_action *action);
bool function_14d0a0(const s_player_action *action);

static __forceinline real action_decode_real(long value, long maximum, real low, real high)
{
 real result;
 if (value == 0)
  result = low;
 else if (value >= maximum)
  result = high;
 else
  result = ((real)(maximum - value) * low + (real)value * high) * (1.0f / (real)maximum);
 return result;
}

#pragma inline_depth(0)
static __forceinline void function_874c1(s_player_action *arg_0)
{
 player_action_initialize(arg_0);
}
#pragma inline_depth(8)
// @retail 0x874c0
byte function_874c0(s_bitstream *stream, s_player_action *action)
{
 s_action_codec_view *result = (s_action_codec_view *)action;
 function_874c1(action);
 result->control = function_1959c0(stream, 32);
 result->flags = (word)function_1959c0(stream, 9);
 result->yaw = action_decode_real(function_1959c0(stream, 13), 8191, 0.0f, 6.2831855f);
 result->pitch = action_decode_real(function_1959c0(stream, 12), 4095, -3.1415927f, 3.1415927f);
 result->forward = action_decode_real(function_1959c0(stream, 5), 30, -1.0f, 1.0f);
 result->strafe = action_decode_real(function_1959c0(stream, 5), 30, -1.0f, 1.0f);
 result->first = action_decode_real(function_1959c0(stream, 5), 31, 0.0f, 1.0f);
 result->second = action_decode_real(function_1959c0(stream, 5), 31, 0.0f, 1.0f);
 result->index = (short)(function_1959c0(stream, 5) - 1);
 result->weapon = (signed char)(function_1959c0(stream, 3) - 1);
 result->grenade = (signed char)(function_1959c0(stream, 3) - 1);
 result->slot = (short)(function_1959c0(stream, 2) - 1);
 result->seat = (short)(function_1959c0(stream, 2) - 1);
 result->target_type = (short)function_1959c0(stream, 4);
 if (result->target_type)
 {
  result->target = function_1959c0(stream, 32);
  switch (result->target_type)
  {
  case 4:
  case 5:
  case 6: result->target_slot = (short)function_1959c0(stream, 5); break;
  case 1:
  case 7: result->target_byte = (byte)function_1959c0(stream, 4); break;
  }
 }
 else
  result->target = NONE;
 if (function_1957d0(stream))
  result->value30 = function_1959c0(stream, 32);
 else
  result->value30 = NONE;
 if (function_1957d0(stream))
 {
  result->value50 = action_decode_real(function_1959c0(stream, 4), 15, 0.0f, 1.0f);
  result->value54 = action_decode_real(function_1959c0(stream, 4), 15, 0.0f, 1.0f);
  result->flags4c = (short)function_1959c0(stream, 2);
  result->value34 = function_1959c0(stream, 32);
  if (result->value34 != NONE && function_1957d0(stream))
   result->value38 = function_1959c0(stream, 5);
 }
 if (function_1957d0(stream))
 {
  long index = function_1959c0(stream, 4);
  long player = NONE;
  if (index != NONE)
   player = (*(short *)(g_4e8c24->data + g_4e8c24->size * index) << 16) | index;
  result->player = player;
 }
 return function_14d0a0(action) != false;
}

static __forceinline long action_round(real value)
{
 long result;
 __asm { fld value }
 __asm { fistp result }
 return result;
}

static __forceinline void function_86f91(s_bitstream *stream, dword value, long bits)
{
 if (bits < 32 && value >= (dword)(1 << bits))
 {
  char message[256];
  message[0] = 0;
  csprintf_256(message, "%u exceeds max value of %u", value, 1 << bits);
 }
 function_195720(stream, value, bits);
}

// @retail 0x86f90
void function_86f90(s_bitstream *stream, s_player_action *action)
{
 s_action_codec_view *input = (s_action_codec_view *)action;
 function_195720(stream, input->control, 32);
 stream_write_checked(stream, input->flags, 9);
 function_195720(stream, action_round(input->yaw * 1303.6380615234375f), 13);
 function_195720(stream, action_round((input->pitch - -3.1415927f) * 651.739501953125f), 12);
 function_195720(stream, action_round((input->forward - -1.0f) * 15.0f), 5);
 function_195720(stream, action_round((input->strafe - -1.0f) * 15.0f), 5);
 function_195720(stream, action_round(input->first * 31.0f), 5);
 function_195720(stream, action_round(input->second * 31.0f), 5);
 stream_write_checked(stream, input->index + 1, 5);
 stream_write_checked(stream, input->weapon + 1, 3);
 stream_write_checked(stream, input->grenade + 1, 3);
 stream_write_checked(stream, input->slot + 1, 2);
 stream_write_checked(stream, input->seat + 1, 2);
 function_86f91(stream, input->target_type, 4);
 if (input->target_type)
 {
  function_195720(stream, input->target, 32);
  switch (input->target_type)
  {
  case 4:
  case 5:
  case 6: function_86f91(stream, input->target_slot, 5); break;
  case 1:
  case 7: function_86f91(stream, input->target_byte, 4); break;
  }
 }
 stream_write_bit(stream, input->value30 != NONE);
 if (input->value30 != NONE)
  function_195720(stream, input->value30, 32);
 if (input->value50 > 0.0f || input->value54 > 0.0f || (input->flags4c & 1) || input->value34 != NONE)
 {
  stream_write_bit(stream, true);
  real first = input->value50 > 0.0f ? input->value50 : 0.0f;
  real second = input->value54 > 0.0f ? input->value54 : 0.0f;
  function_195720(stream, action_round(first * 15.0f), 4);
  function_195720(stream, action_round(second * 15.0f), 4);
  stream_write_checked(stream, input->flags4c, 2);
  function_195720(stream, input->value34, 32);
  if (input->value34 != NONE)
  {
   stream_write_bit(stream, input->value38 != NONE);
   if (input->value38 != NONE)
    stream_write_checked(stream, input->value38, 5);
  }
 }
 else
  stream_write_bit(stream, false);
 stream_write_bit(stream, input->player != NONE);
 if (input->player != NONE)
  stream_write_checked(stream, input->player & 0xffff, 4);
}


struct s_simulation_machine_list
{
 dword mask;
 byte addresses[16][6];
};
void simulation_write_machines(s_bitstream *stream, const s_simulation_machine_list *machines);
bool simulation_read_machines(s_bitstream *stream, s_simulation_machine_list *machines);

// @retail 0x87d00
void function_87d00(s_bitstream *stream, void *message)
{
 byte *block = (byte *)message;
 stream_write_checked(stream, *(dword *)block, 32);
 stream_write_checked(stream, *(dword *)(block + 8), 16);
 for (long i = 0; i < 16; i++)
  if (*(dword *)(block + 8) & (1 << i))
   function_86f90(stream, (s_player_action *)(block + 0xc + i * 0x5c));
 stream_write_bit(stream, *(bool *)(block + 0xdd0));
 if (*(bool *)(block + 0xdd0))
  simulation_write_machines(stream, (s_simulation_machine_list *)(block + 0xdd4));
 stream_write_checked(stream, *(dword *)(block + 0xe38), 5);
 for (long j = 0; j < *(long *)(block + 0xe38); j++)
  function_87930(stream, (s_simulation_player_update *)(block + 0xe3c + j * 0xc8));
 stream_write_bit(stream, *(bool *)(block + 0x403c));
 stream_write_checked(stream, *(dword *)(block + 0x4040), 32);
 stream_write_checked(stream, *(dword *)(block + 0x4044), 32);
}

// @retail 0x87e90
bool function_87e90(s_bitstream *stream, void *message)
{
 byte *block = (byte *)message;
 bool result = true;
 *(dword *)block = function_1959c0(stream, 32);
 *(dword *)(block + 8) = function_1959c0(stream, 16);
 for (long i = 0; i < 16; i++)
  if (*(dword *)(block + 8) & (1 << i))
   if (result && function_874c0(stream, (s_player_action *)(block + 0xc + i * 0x5c))) result = true; else result = false;
 *(bool *)(block + 0xdd0) = stream_read_bit(stream);
 if (*(bool *)(block + 0xdd0))
  if (result && simulation_read_machines(stream, (s_simulation_machine_list *)(block + 0xdd4))) result = true; else result = false;
 *(long *)(block + 0xe38) = function_1959c0(stream, 5);
 if (*(long *)(block + 0xe38) >= 0 && *(long *)(block + 0xe38) <= 64)
 {
  for (long j = 0; j < *(long *)(block + 0xe38); j++)
   if (result && function_87ac0(stream, (s_simulation_player_update *)(block + 0xe3c + j * 0xc8))) result = true; else result = false;
 }
 else
  result = false;
 *(bool *)(block + 0x403c) = stream_read_bit(stream);
 *(dword *)(block + 0x4040) = function_1959c0(stream, 32);
 *(dword *)(block + 0x4044) = function_1959c0(stream, 32);
 result = result && !stream_overflowed(stream) && *(long *)(block + 0x4040) >= 0 && *(long *)block >= 0;
 return result;
}
