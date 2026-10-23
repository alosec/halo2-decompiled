// @flags /O2 /Ob1 /Gr
/* UNKNOWN_1E4290.CPP: an actor flag setter (lane M; called by the behaviors of
   0x1a8000..0x1affff) */

#include "unknown_11c920.h"
#include "ai_actor.h"
#include "unit_requests.h"
#include <string.h>

struct s_actor_timers_x
{
	byte field_000[0x2e8];
	long reference;
	short field_2ec;
	short field_2ee;
	bool field_2f0;
	byte field_2f1;
	short field_2f2;
	short field_2f4;
	byte field_2f6[0xa];
	long field_300;
	short field_304;
	short field_306;
};

// @retail 0x1e36d0
void function_1e36d0(long actor_index)
{
	s_actor_timers_x *actor = (s_actor_timers_x *)actor_get(actor_index);
	if (actor->reference != NONE)
	{
		actor->field_2ee--;
		if (actor->field_2ee <= 0)
		{
			actor->reference = NONE;
			actor->field_2ec = NONE;
			actor->field_2ee = NONE;
			actor->field_2f0 = false;
		}
	}
	if (actor->field_2f2 > 0)
		actor->field_2f2--;
	if (actor->field_2f4 > 0)
		actor->field_2f4--;
	if (actor->field_304 > 0)
	{
		actor->field_304--;
		if (actor->field_304 == 0)
			actor->field_300 = NONE;
	}
	if (actor->field_306 >= 0 && actor->field_306 < 32767)
		actor->field_306++;
}

// @retail 0x1e4220
void function_1e4220(long actor_index, bool enabled)
{
	s_actor_view *actor = actor_get(actor_index);
	*((bool *)actor + 0x6d0) = enabled;
	if (enabled)
		actor->flags810 |= 0x08000000;
	else
		actor->flags810 &= ~0x08000000;
}

// @retail 0x1e4260
void function_1e4260(long actor_index)
{
	s_actor_view *actor = actor_get(actor_index);
	dword flags = actor->flags810;
	actor->flags810 = flags | 2;
}

// @retail 0x1e4290
void function_1e4290(long actor_index, bool value)
{
	bool *enable = &value;
	s_actor_view *actor = actor_get(actor_index);

	if (*enable)
	{
		byte unknown229 = actor->unknown229;
		if (!unknown229)
			{
			long target_index = *(volatile long *)&actor->unknown26c;
			if (NONE != target_index)
			{
	   bool allowed = 0 >= *(byte *)&ai_object_get(target_index)->unknown34c;
	   *enable = allowed;
	   if (!(bool)allowed) return;
	  }
		}
		dword flags = actor->flags810;
		actor->flags810 = flags | 0x800;
	}
	else
	{
		actor->flags810 &= ~0x800;
	}
}

void function_25d510(long actor_index);
void function_25d580(long actor_index);

// @retail 0x1e4500
void function_1e4500(long actor_index, short mode)
{
	if (mode <= 3)
	{
		s_actor_view *actor = actor_get(actor_index);
		switch (mode)
		{
		case 0:
		case 1:
			function_25d580(actor_index);
			break;
		case 2:
			function_25d510(actor_index);
			break;
		}
		actor->unknown086 = mode;
		*(long *)actor->unknown088 = 0;
	}
}

/* a request with no arguments, written out where it is used */
static __forceinline bool unit_request_simple(long type, long unit_index)
{
	s_unit_request request;
	memset(&request, 0, sizeof(request));
	request.type = type;
	return function_e6900(unit_index, &request);
}

// @retail 0x1e4310
bool function_1e4310(long actor_index)
{
	s_actor_view *actor = actor_get(actor_index);
	bool result = false;
	if (unit_request_simple(0x16, actor->unknown018))
	{
		*(long *)((byte *)actor + 0x7c8) = g_510c54->game_time;
		result = true;
	}
	return result;
}

// @retail 0x1e1740
void function_1e1740(long actor_index, long object_index)
{
	s_actor_view *actor = actor_get(actor_index);
	const long *object_reference = &object_index;
	if (actor->unknown3cc == *object_reference)
		actor->unknown3cc = NONE;
	for (short i = 0; i <= actor->current; i++)
	{
		s_slot *slot = &actor->slots[i];
		t_slot_release callback = g_46eeb8[slot->type]->release24;
		if (callback)
			callback(actor_index, i < 4 ? &actor->slots[i] : NULL, *object_reference);
	}
}

#include "unknown_2551c0.h"

struct s_actor_object_query
{
	long definition_index;
	dword flags;
	byte field_8[0x14 - 8];
	long parent_index;
	byte field_18[0x28 - 0x18];
	dword location[2];
	byte field_30[0x70 - 0x30];
	vector3f direction;
	byte field_7c[0xaa - 0x7c];
	byte type;
	byte field_ab[0x134 - 0xab];
	dword flags134;
	byte field_138[2];
	short link_offset;
};

PRIVATE inline s_actor_object_query *actor_query_object(long index)
{
	return (s_actor_object_query *)ai_object_get(index);
}

PRIVATE inline long actor_query_root(long index)
{
	long result = NONE;
	while (index != NONE)
	{
		result = index;
		index = actor_query_object(index)->parent_index;
	}
	return result;
}

PRIVATE inline bool actor_query_enabled(long index)
{
	s_actor_object_query *object = actor_query_object(index);
	return !((bool)((object->flags >> 18) & 1)) && !((bool)((object->flags >> 7) & 1));
}

long function_baf80(long object_index);

// @retail 0x1e13f0
bool function_1e13f0(long actor_index)
{
	bool result = false;
	s_actor_view *actor = actor_get(actor_index);
	if (actor->unknown018 != NONE)
	{
		if (actor_query_enabled(function_baf80(actor->unknown018)))
			result = true;
	}
	else
	{
		long perception_index = *(long *)((byte *)actor + 0x1c);
		if (perception_index != NONE)
		{
			long object_index = perception_get(perception_index)->object_index;
			while (!result && object_index != NONE)
			{
				s_actor_object_query *object = actor_query_object(object_index);
				long current = object_index;
				byte *links = object->flags134 == 0 ? (byte *)object + object->link_offset : NULL;
				object_index = links ? *(long *)(links + 0xc) : NONE;
				if (actor_query_enabled(actor_query_root(current)))
					result = true;
			}
		}
	}
	return result;
}

void function_268810(long clump_index);

// @retail 0x1e11b0
bool function_1e11b0(long actor_index, bool active)
{
 s_actor_view *actor = actor_get(actor_index);
 bool result = true;
 if (actor->unknown009 != active)
 {
  if (active)
  {
   if (function_1e13f0(actor_index))
   {
    long clump_index = actor->unknown07c;
    if (clump_index != NONE && !*(bool *)(g_502420->data + (clump_index & 0xffff) * 0x50 + 0x1c))
     function_268810(clump_index);
    actor->unknown009 = true;
    ++*(short *)((byte *)g_4f55d0 + 0x36a);
   }
   else result = false;
  }
  else
  {
   actor->unknown009 = false;
   *(long *)((byte *)actor + 0x10) = g_510c54->game_time;
   --*(short *)((byte *)g_4f55d0 + 0x36a);
  }
 }
 return result;
}

// @retail 0x1e12b0
bool function_1e12b0(long actor_index, const dword *clusters)
{
 s_actor_view *actor = actor_get(actor_index);
 bool result = false;
 if (function_1e13f0(actor_index))
 {
  if (actor->unknown007)
  {
   long perception_index = *(long *)actor->unknown01c;
   if (perception_index != NONE)
   {
    long object_index = perception_get(perception_index)->object_index;
    while (!result && object_index != NONE)
    {
     s_actor_object_query *object = actor_query_object(object_index);
     long current = object_index;
     byte *links = object->flags134 == 0 ? (byte *)object + object->link_offset : NULL;
     object_index = links ? *(long *)(links + 0xc) : NONE;
     long cluster = *(short *)((byte *)actor_query_object(actor_query_root(current)) + 0x2c);
     if (cluster != NONE)
      result = (clusters[cluster >> 5] & (1 << (cluster & 31))) != 0;
    }
   }
  }
  else
  {
   long cluster = *(short *)((byte *)actor_query_object(function_baf80(actor->unknown018)) + 0x2c);
   if (cluster != NONE)
    result = (clusters[cluster >> 5] & (1 << (cluster & 31))) != 0;
  }
 }
 return result;
}

bool function_1e32e0(long actor_index, bool flag);
void __stdcall function_2628f0(long actor_index, s_reference reference);
void function_26def0(long actor_index, long owner_index);

struct s_actor_options
{
 byte field_0[0x24];
 dword flags;
 short mode;
};

// @retail 0x1e4570
void function_1e4570(long actor_index, const s_actor_options *options)
{
 const long *index_reference = &actor_index;
 s_actor_view *actor = actor_get(*index_reference);
 bool second = false;
 bool first = second;
 bool third = second;
 function_2628f0(*index_reference, g_470fa0);
 *((bool *)actor + 0x227) = false;
 if (options)
 {
  first = (bool)((options->flags >> 6) & 1);
  second = (bool)((options->flags >> 7) & 1);
  third = (bool)((options->flags >> 8) & 1);
  if (options->mode > 0)
   function_1e4500(*index_reference, options->mode - 1);
 }
 *((bool *)actor + 0x224) = third;
 *((bool *)actor + 0x223) = second;
 function_1e32e0(*index_reference, first);
 if (*((bool *)actor + 0x224) && actor->unknown26c != NONE &&
  actor->unknown018 != NONE && *(long *)((byte *)actor + 0x858) == NONE)
  function_e68c0(0x1d, actor->unknown018);
 if (*(long *)((byte *)actor + 0x3f4) != NONE)
  function_26def0(*index_reference, NONE);
}
