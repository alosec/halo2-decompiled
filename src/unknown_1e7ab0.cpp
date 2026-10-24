// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_1E7AB0.CPP: whether a unit request can start, and whether it should
   end (built with SSE, apart from its neighbours in unknown_1e77c0.cpp) */

#include "unknown_11c920.h"
#include "globals.h"
#include <string.h>
#include <xmmintrin.h>

/* a player (g_4e8c24, 0x21c bytes): +0x28 is its local player slot */
struct s_player_request_view
{
	byte unknown00[0x28];
	short local_player_index;
	byte unknown2a[2];
	long unit_index;
	byte unknown30[0xc0 - 0x30];
	char team;
	byte unknownc1[0x21c - 0xc1];
};

struct s_time_entry
{
	long time;
	short a;
	short b;
};

struct s_request_transition
{
	short state;
	short ticks;
};

/* a local player's state in g_51e9c0 (src/player_request_state.cpp), 0x1b0 bytes:
   the four last unit requests at +0x150 */
struct s_local_player_state_view
{
	s_request_transition transitions[32];
	byte unknown080[0x150 - 0x80];
	s_time_entry requests[4];
	dword request_flags[8];
	long version;
	byte field_194;
	byte field_195;
	byte field_196;
	byte field_197;
	byte timer198;
	byte timer199;
	byte timer19a;
	byte field_19b;
	byte unknown19c[0x1b0 - 0x19c];
};

struct s_unknown_1e6a40;
extern s_unknown_1e6a40 *g_51e9c0;

struct s_entry_420
{
	long value;
	word field_04;
	word limit;
	word field_08;
	short duration;
	byte field_0c[8];
};

struct s_table_420
{
	byte field_00[0x420];
	long count;
	s_entry_420 *entries;
};

inline s_entry_420 *entry_420_get(long index)
{
	s_entry_420 *result = NULL;
	s_table_420 *table = (s_table_420 *)g_510c94;
	if (table && index >= 0 && index < table->count)
		result = &table->entries[index];
	return result;
}

bool function_1e69d0(s_time_entry *entries, long type, long age_limit);
bool function_1e7a10(long player, long index);
long function_1e7a60(long player, long index);
bool function_1e8eb0(long object_index, long type, bool any_nonzero);

#include "unknown_1248b0.h"
#include <math.h>

struct s_weapon_status_magazine
{
 bool active;
 bool idle;
 short loaded;
 short loaded_maximum;
 short unloaded;
 short total_maximum;
};

struct s_weapon_status
{
 real value;
 real heat;
 bool flag8;
 bool flag9;
 byte unknown0a[2];
 real fraction;
 bool charging;
 bool target_available;
 bool target_charging;
 bool target_ready;
 real target_fraction;
 point3f target_position;
 short magazine_count;
 s_weapon_status_magazine magazines[2];
};

void function_100520(long weapon_index, s_weapon_status *status);
point3f *function_b9dd0(long object_index, point3f *result);
short function_c8860(long unit_index);
long function_cbd50(long unit_index, short weapon_index);
bool function_f42f0(long vehicle_index);
bool function_15eaf0(void);

PRIVATE __forceinline real request_angle_delta(real angle, real previous)
{
 real delta = angle - previous;
 if (delta > 3.1415927410125732f) delta -= 6.2831854820251465f;
 if (delta < -3.1415927410125732f) delta += 6.2831854820251465f;
 return delta;
}

bool function_13cb40(void);
bool motion_sensor_enemy_vehicle_ahead(long local_player_index);
bool motion_sensor_enemy_nearby(long local_player_index);
long function_162fd0(long index);
bool voice_port_can_talk(long port);
bool function_100f00(long weapon_index);
bool function_100fd0(long weapon_index);
short function_101010(long weapon_index, short index);
struct s_object;
s_object *function_badc0(long object_index, dword type_mask);

struct s_unit_child_iterator
{
 long object_index;
 long unit_index;
 short seat_index;
 long next_index;
};
struct s_damage_object;
void function_d0590(s_unit_child_iterator *iterator, long object_index);
s_damage_object *function_d05c0(s_unit_child_iterator *iterator);

PRIVATE inline byte *request_unit(long object_index, byte *headers)
{
 return *(byte **)(headers + (object_index & 0xffff) * 12 + 8);
}

PRIVATE inline long request_weapon(s_player_request_view *player, long slot, byte *headers)
{
 long unit_index = player->unit_index;
 byte *unit = request_unit(unit_index, headers);
 return function_cbd50(unit_index, *(signed char *)(unit + 0x212 + slot));
}

// @retail 0x1e7ab0
bool function_1e7ab0(long local_index, long type)
{
 (void)&local_index;
 s_local_player_state_view *state = &((s_local_player_state_view *)g_51e9c0)[local_index];
 byte *data = (byte *)state;
 volatile bool result = false;
 long player_index = local_index == NONE ? NONE : g_4e8c20->entries[local_index];
 s_entry_420 *entry = entry_420_get(type);
 if (player_index != NONE && entry &&
  (!(*((byte *)entry + 0x10) & 1) || g_4e6948->state != 2))
 {
  s_player_request_view *volatile player = (s_player_request_view *)(g_4e8c24->data + (player_index & 0xffff) * 0x21c);
  long unit_index = player->unit_index;
  if (unit_index != NONE)
  {
   byte *unit = request_unit(unit_index, g_4e0300->data);
   switch (type)
   {
   case 10:
    if (!function_13cb40() && *(short *)(unit + 0x1fc) != NONE && *(long *)(unit + 0x14) != NONE && function_1e8eb0(unit_index, NONE, true))
    { result = true; if (*(long *)(data + 0x80) != 10) data[0x1a0] = false; }
    break;
   case 11:
    if (!function_13cb40() && *(short *)(unit + 0x1fc) != NONE && *(long *)(unit + 0x14) != NONE && function_1e8eb0(unit_index, NONE, true))
    { result = true; if (*(long *)(data + 0x80) != 11) data[0x1a0] = false; }
    break;
   case 12:
    if (!function_13cb40() && *(long *)(unit + 0x14) == NONE)
    { result = true; if (*(long *)(data + 0x80) != 12) data[0x1a0] = false; }
    break;
   case 13:
    if (!function_13cb40() && *(long *)(unit + 0x14) == NONE)
    { result = true; if (*(long *)(data + 0x80) != 13) data[0x1a0] = false; }
    break;
   case 14:
    if (function_1e7a60(local_index, 16) == 0 && function_c8860(unit_index) != NONE) result = true;
    break;
   case 2:
    if (!function_13cb40() && function_1e7a10(local_index, 12) && *(long *)(unit + 0x14) == NONE)
    { result = true; if (*(long *)(data + 0x80) != 2) data[0x1a0] = false; }
    break;
   case 4:
    if (!function_13cb40())
    {
     long parent = *(long *)(unit + 0x14);
     short seat = *(short *)(unit + 0x1fc);
     if (parent != NONE && seat != NONE)
     {
      byte *definition = g_4e3b44[*(long *)request_unit(parent, g_4e0300->data) & 0xffff].bytes;
      byte *seats = *(byte **)(definition + 0x1cc);
      dword flags = *(dword *)(seats + seat * 0xb0);
      if (!((bool)((flags >> 2) & 1)) && !((bool)((flags >> 3) & 1)) && !((bool)((flags >> 11) & 1)))
      {
       s_unit_child_iterator iterator;
       function_d0590(&iterator, parent);
       volatile bool blocked = false;
       while (function_d05c0(&iterator))
       {
        if (iterator.seat_index != NONE && iterator.unit_index != unit_index &&
         ((bool)((*(dword *)(seats + iterator.seat_index * 0xb0) >> 2) & 1)))
        { blocked = true; break; }
       }
       if (!blocked) result = true;
      }
     }
    }
    break;
   case 15:
    if (request_weapon(player, false, g_4e0300->data) != NONE && !motion_sensor_enemy_nearby(local_index)) result = true;
    break;
   case 18:
    if (request_weapon(player, false, g_4e0300->data) != NONE && !motion_sensor_enemy_nearby(local_index) && function_1e7a10(local_index, 15)) result = true;
    break;
   case 0: result = (state->field_194 & 1) != 0; break;
   case 1: result = ((state->field_194 >> 1) & 1) != 0; break;
   case 31: result = ((state->field_194 >> 2) & 1) != 0; break;
   case 17: if (state->field_19b > 0 && *(short *)(unit + 0x1fc) == NONE) result = true; break;
   case 30:
    if (g_4e6948->state == 2 && function_15eaf0() && function_162fd0(local_index) != NONE)
    {
     long gamepad = *(long *)((byte *)player + 0x24);
     if (gamepad != NONE && voice_port_can_talk(gamepad)) result = true;
    }
    break;
   case 16:
    if (!motion_sensor_enemy_nearby(local_index) && request_weapon(player, false, g_4e0300->data) != NONE &&
     request_weapon(player, true, g_4e0300->data) == NONE && function_100fd0(request_weapon(player, false, g_4e0300->data)) &&
     function_101010(request_weapon(player, false, g_4e0300->data), 0) != NONE)
    { result = true; if (*(long *)(data + 0x80) != 16) data[0x1a0] = false; }
    break;
   case 20:
    if (motion_sensor_enemy_vehicle_ahead(local_index) && *(short *)(unit + 0x1fc) == NONE) result = true;
    break;
   case 23:
    if (!motion_sensor_enemy_nearby(local_index) && request_weapon(player, false, g_4e0300->data) != NONE)
    {
     s_weapon_status status;
     function_100520(request_weapon(player, false, g_4e0300->data), &status);
     if (status.magazine_count > 0 && status.magazines[0].loaded_maximum != 0 &&
      status.magazines[0].loaded * 2 <= status.magazines[0].loaded_maximum &&
      status.magazines[0].total_maximum != 0 && status.magazines[0].unloaded > 0) result = true;
    }
    break;
   case 9:
    if (request_weapon(player, true, g_4e0300->data) != NONE && function_100f00(request_weapon(player, true, g_4e0300->data))) result = true;
    break;
   case 25: if (function_1e8eb0(unit_index, 3, false)) result = true; break;
   case 28: if (function_1e8eb0(unit_index, 3, false)) result = true; break;
   case 24: if (function_1e8eb0(unit_index, 1, false)) result = true; break;
   case 26: if (function_1e8eb0(unit_index, 4, false)) result = true; break;
   case 27: if (function_1e8eb0(unit_index, 4, false)) result = true; break;
   case 29: if (function_1e8eb0(unit_index, 4, false)) result = true; break;
   case 22:
    if (*(real *)(unit + 0xf0) < 0.25f || (*(long *)(data + 0x80) == 22 && function_1e7a10(local_index, 21))) result = true;
    break;
   case 21:
    if ((bool)(((dword)*(word *)(unit + 0x10a) >> 9) & 1))
     if (*(real *)(unit + 0xf0) < 0.25f || *(long *)(data + 0x80) == 21) result = true;
    break;
   case 5:
    if (*(word *)(data + 0x140) == 3 && *(word *)(data + 0x148) >= 3) result = true;
    break;
   case 6:
    if (*(word *)(data + 0x140) == 5 && *(word *)(data + 0x148) >= 3)
    {
     byte *target = (byte *)function_badc0(*(long *)(data + 0x144), 2);
     if (target && *(short *)(g_4e3b44[*(long *)target & 0xffff].bytes + 0x21a) != 0) result = true;
    }
    break;
   case 7:
    if (*(word *)(data + 0x140) == 1 && (data[0x14c] & 3) && *(word *)(data + 0x148) >= 3) result = true;
    break;
   case 8:
    if (*(word *)(data + 0x140) == 1 && (data[0x14c] & 12) && state->field_197 >= 3) result = true;
    break;
   case 3: break;
   case 19: break;

   }
  }
 }
 return result;
}
// @retail 0x1e83c0
bool function_1e83c0(long local_index, long type)
{
 s_local_player_state_view *state = &((s_local_player_state_view *)g_51e9c0)[local_index];
 byte *data = (byte *)state;
 bool result = false;
 long player_index = local_index == NONE ? NONE : g_4e8c20->entries[local_index];
 if (player_index != NONE)
 {
  s_player_request_view *player = (s_player_request_view *)(g_4e8c24->data + (player_index & 0xffff) * 0x21c);
  long unit_index = player->unit_index;
  if (unit_index != NONE)
  {
   byte *unit = *(byte **)(g_4e0300->data + (unit_index & 0xffff) * 12 + 8);
   switch (type)
   {
   case 10:
    if (*(long *)(data + 0x80) == 10)
    {
     if (!data[0x1a0])
     {
      *(point2f *)(data + 0x1a4) = *(point2f *)(data + 0x8c);
      data[0x1a0] = true;
     }
     else
     {
      real yaw = *(real *)(data + 0x8c) - *(real *)(data + 0x1a4);
      real pitch = *(real *)(data + 0x90) - *(real *)(data + 0x1a8);
      if (yaw > 3.1415927410125732f) yaw -= 6.2831854820251465f;
      if (yaw < -3.1415927410125732f) yaw += 6.2831854820251465f;
      if (fabs(yaw) > 0.1745329201221466f || fabs(pitch) > 0.1745329201221466f) result = true;
     }
    }
    break;
   case 11:
    if (*(long *)(data + 0x80) == 11)
    {
     if (!data[0x1a0])
     {
      function_b9dd0(unit_index, (point3f *)(data + 0x1a4));
      data[0x1a0] = true;
     }
     else
     {
      point3f point;
      function_b9dd0(unit_index, &point);
      real x = *(real *)(data + 0x1a4) - point.x;
      real y = *(real *)(data + 0x1a8) - point.y;
      real z = *(real *)(data + 0x1ac) - point.z;
      if (z * z + y * y + x * x > 0.25f) result = true;
     }
    }
    break;
   case 12:
    if (*(long *)(data + 0x80) == 12)
    {
     if (!data[0x1a0])
     {
      *(point2f *)(data + 0x1a4) = *(point2f *)(data + 0x8c);
      data[0x1a0] = true;
     }
     else
     {
      real yaw = *(real *)(data + 0x8c) - *(real *)(data + 0x1a4);
      real pitch = *(real *)(data + 0x90) - *(real *)(data + 0x1a8);
      if (yaw > 3.1415927410125732f) yaw -= 6.2831854820251465f;
      if (yaw < -3.1415927410125732f) yaw += 6.2831854820251465f;
      if (fabs(yaw) > 0.1745329201221466f || fabs(pitch) > 0.1745329201221466f) result = true;
     }
    }
    break;
   case 13:
    if (*(long *)(data + 0x80) == 13)
    {
     if (!data[0x1a0])
     {
      *(point3f *)(data + 0x1a4) = *(point3f *)(unit + 0x64);
      data[0x1a0] = true;
     }
     else
     {
      point3f point = *(point3f *)(unit + 0x64);
      real x = *(real *)(data + 0x1a4) - point.x;
      real y = *(real *)(data + 0x1a8) - point.y;
      real z = *(real *)(data + 0x1ac) - point.z;
      if (z * z + y * y + x * x > 0.25f) result = true;
     }
    }
    break;
   case 14: if (function_c8860(unit_index) == NONE) result = true; break;
   case 2:
    if (*(long *)(data + 0x80) == 2)
    {
     if (!data[0x1a0])
     {
      data[0x1ac] = false; data[0x1ad] = false;
      *(real *)(data + 0x1a4) = *(real *)(data + 0x8c);
      data[0x1a0] = true;
     }
     else
     {
      real delta = request_angle_delta(*(real *)(data + 0x8c), *(real *)(data + 0x1a4));
      if (delta < -0.3490658402442932f) data[0x1ac] = true;
      if (delta > 0.3490658402442932f) data[0x1ad] = true;
      if (data[0x1ad] && data[0x1ac]) result = true;
     }
    }
    break;
   case 4: if (function_1e69d0(state->requests, 0x1d, g_510c54->field_2_3)) result = true; break;
   case 15: if (state->timer19a > 0) { result = true; state->timer19a = 0; } break;
   case 18: if (state->timer199 > 0) { result = true; state->timer199 = 0; } break;
   case 0:
    if (data[0x88] & 4) { result = true; state->field_194 &= ~1; }
    break;
   case 1:
    if (data[0x88] & 1) { result = true; state->field_194 &= ~2; }
    break;
   case 31: if (data[0x88] & 4) { result = true; state->field_194 &= ~4; } break;   case 17: if (state->timer198 > 0) { result = true; state->timer198 = 0; } break;
   case 30:
    if (g_4e6948->state == 2 && function_15eaf0())
    {
     if (*(long *)((byte *)player + 0x24) != NONE)
     {
      const s_type_ff3a2a *input = function_1249a0(*(short *)((byte *)player + 0x24));
      if (input && (((const byte *)input)[0x15] > 0 || ((const byte *)input)[0x18] > 0)) result = true;
     }
    }
    break;
   case 16:
    if (*(long *)(data + 0x80) == 16)
    {
     if (!data[0x1a0]) { data[0x1a0] = true; *(long *)(data + 0x1a4) = 0; }
     else
     {
      if (function_c8860(unit_index) != NONE) ++*(long *)(data + 0x1a4);
      else *(long *)(data + 0x1a4) = 0;
      if (*(long *)(data + 0x1a4) > g_510c54->field_2_3 / 2) result = true;
     }
    }
    break;
   case 20: if (function_1e69d0(state->requests, 0x1f, g_510c54->field_2_3)) result = true; break;
   case 23:
    if (function_cbd50(player->unit_index, *(signed char *)(*(byte **)(g_4e0300->data + (player->unit_index & 0xffff) * 12 + 8) + 0x212)) != NONE)
    {
     s_weapon_status status;
     byte *current_unit = *(byte **)(g_4e0300->data + (player->unit_index & 0xffff) * 12 + 8);
     function_100520(function_cbd50(player->unit_index, *(signed char *)(current_unit + 0x212)), &status);
     if (status.magazine_count > 0 && status.magazines[0].loaded_maximum != 0 &&
      status.magazines[0].loaded < status.magazines[0].loaded_maximum && (*(dword *)(unit + 0x148) & 0x10000000)) result = true;
    }
    break;
   case 9: if (*(dword *)(data + 0x88) & 0x200000) result = true; break;
   case 25: if (*(dword *)(data + 0x88) & 0x10000) result = true; break;
   case 29:
    if (function_1e8eb0(unit_index, 4, false) && function_f42f0(*(long *)(unit + 0x14))) result = true;
    break;
   case 27: if (*(dword *)(data + 0x88) & 0x20000000) result = true; break;
   case 24: if (*(dword *)(data + 0x88) & 0x4000) result = true; break;
   case 26: if (*(dword *)(data + 0x88) & 0x800) result = true; break;
   case 28: if (*(dword *)(data + 0x88) & 0x800) result = true; break;
   case 21: if (*(real *)(unit + 0xf0) >= 0.9999f) result = true; break;
   case 22: if (*(real *)(unit + 0xf0) >= 0.9999f) result = true; break;
   case 5: if (state->field_195 > 0) result = true; break;
   case 6: if (function_1e69d0(state->requests, 0x1c, g_510c54->field_2_3)) result = true; break;
   case 7:
    if (*(word *)(data + 0x140) == 1 && ((*(word *)(data + 0x14c) & 1) || (*(word *)(data + 0x14c) & 2)) &&
     *(word *)(data + 0x148) >= 3 && (data[0xa4] & 2)) result = true;
    break;
   case 8:
    if (*(word *)(data + 0x140) == 1 && (data[0x14c] & 12) && (data[0xa4] & 0x20)) result = true;
    break;
   case 3: break;
   case 19: break;

   }
  }
 }
 return result;
}
