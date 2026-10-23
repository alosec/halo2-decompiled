// @flags /O2 /Gr
/* UNKNOWN_084A90.CPP: the player collections the simulation watcher keeps
   (lane D) */

#include "unknown_11c920.h"
#include <string.h>
#include "data_array.h"
#include "globals.h"
#include "unknown_067e10.h"

/* a game player (g_4e8c24, 0x21c bytes), as the collection reads it */
struct s_simulation_player_datum
{
	short salt;
	word flag0 : 1;
	word field_2_2 : 1;
	word unknown02 : 14;
	dword key[3];
	long time;
	s_machine_address machine;
	short machine_index;
	short controller_index;
	byte unknown1e[2];
	long unknown20;
	long value24;
	short index28;
	byte unknown2a[0xd4 - 0x2a];
	dword configuration[0x24];
	byte unknown164[0x21c - 0x164];
};

struct s_simulation_player_identity
{
	long index;
	dword key[3];
};

static inline s_simulation_player_datum *simulation_player_at_index(s_record_pool *data, long index)
{
	s_simulation_player_datum *result = 0;
	if (index != NONE && index >= 0 && index < data->high_water_index)
	{
		s_simulation_player_datum *player = (s_simulation_player_datum *)(data->data + data->size * index);
		if (player->salt)
			result = player;
	}
	return result;
}

void function_14c540(long player_index);

// @retail 0x850c0
bool simulation_player_remove_if_left(const s_simulation_player_identity *identity)
{
	s_simulation_player_datum *player;
	long index = identity->index;
	player = simulation_player_at_index(g_4e8c24, index);
	bool result = false;
	if (player)
	{
		if (memcmp(player->key, identity->key, sizeof(player->key)) == 0 && player->field_2_2)
		{
			function_14c540(data_datum_index(g_4e8c24, index));
			result = true;
		}
	}
	return result;
}

// @retail 0x84a90
void simulation_player_collection_clear(s_type_c67652 *collection)
{
	memset(collection, 0, sizeof(*collection));
	for (long i = 0; i < 16; i++)
	{
		s_simulation_owner_player *player = &collection->players[i];
		player->flag0c = false;
		player->time = NONE;
		player->unknown20 = NONE;
		player->controller_index = NONE;
	}
}

/* data_datum_iterator_next (data_iterator.cpp), which retail inlines here */
static inline bool player_iterator_next(s_data_datum_iterator *iterator)
{
	s_record_pool *data = iterator->data;
	long index = data_find_index(data, iterator->index + 1);
	byte *datum;

	if (index != NONE)
	{
		datum = data->data + data->size * index;
		iterator->index = index;
		iterator->datum_index = (*(short *)datum << 16) | index;
	}
	else
	{
		iterator->index = data->maximum_count;
		iterator->datum_index = NONE;
		datum = 0;
	}
	iterator->datum = datum;
	return datum != 0;
}

// @retail 0x84ad0
void simulation_player_collection_build(s_type_c67652 *collection)
{
	s_data_datum_iterator iterator;
	iterator.data = g_4e8c24;
	iterator.index = NONE;

	while (player_iterator_next(&iterator))
	{
		s_simulation_player_datum *datum = (s_simulation_player_datum *)iterator.datum;
		long player_index = iterator.datum_index & 0xffff;
		s_simulation_owner_player *player = &collection->players[player_index];

		collection->player_mask |= 1 << player_index;
		memcpy(player->key, datum->key, sizeof(player->key));
		if (datum->field_2_2)
		{
			player->flag0c = true;
			player->time = datum->time;
			memset(&player->machine, 0, sizeof(player->machine));
			player->unknown20 = NONE;
			player->controller_index = NONE;
		}
		else
		{
			player->flag0c = false;
			player->time = NONE;
			player->machine = datum->machine;
			player->unknown20 = datum->unknown20;
			player->controller_index = datum->controller_index;
			memcpy(player->configuration, datum->configuration, sizeof(player->configuration));
		}
	}
}

// @retail 0x84c90
void simulation_player_collection_apply_update(s_type_c67652 *collection, const s_simulation_player_update *update)
{
	s_simulation_owner_player *player = &collection->players[update->player_index];

	switch (update->type)
	{
	case 0:
		player->flag0c = true;
		player->time = g_510c54->game_time;
		memset(&player->machine, 0, sizeof(player->machine));
		player->controller_index = NONE;
		player->unknown20 = NONE;
		break;
	case 1:
	{
		long other_index = update->other_player_index;
		s_simulation_owner_player *other = &collection->players[other_index];
		bool player_present = (collection->player_mask & (1 << update->player_index)) != 0;
		bool player_left = player->flag0c;
		bool other_left = other->flag0c;
		long player_time = player->time;
		long other_time = other->time;

		if (collection->player_mask & (1 << other_index))
			collection->player_mask |= 1 << update->player_index;
		else
			collection->player_mask &= ~(1 << update->player_index);
		memcpy(player->key, update->other_key, sizeof(player->key));
		player->flag0c = other_left;
		player->time = other_time;

		if (player_present)
			collection->player_mask |= 1 << update->other_player_index;
		else
			collection->player_mask &= ~(1 << update->other_player_index);
		memcpy(other->key, update->key, sizeof(other->key));
		other->flag0c = player_left;
		other->time = player_time;
		break;
	}
	case 2:
		collection->player_mask &= ~(1 << update->player_index);
		memset(player->key, 0, sizeof(player->key));
		player->flag0c = false;
		player->time = NONE;
		break;
	case 3:
		if (collection->player_mask & (1 << update->player_index))
		{
			player->flag0c = false;
			player->time = NONE;
		}
		else
		{
			collection->player_mask |= 1 << update->player_index;
			memcpy(player->key, update->key, sizeof(player->key));
			if (update->field_2_2)
			{
				player->flag0c = true;
				player->time = g_510c54->game_time;
			}
		}
		player->machine = update->machine;
		player->controller_index = update->controller_index;
		player->unknown20 = update->unknown20;
	case 4:
		memcpy(player->configuration, update->configuration, sizeof(player->configuration));
		break;
	default:
		__assume(0);
	}
}

/* swaps two players' slots */
// @retail 0x84e90
void simulation_player_collection_swap(s_type_c67652 *collection, long player_index, long other_index, s_simulation_player_update *update)
{
	update->type = 1;
	update->player_index = player_index;
	if (collection->player_mask & (1 << player_index))
		memcpy(update->key, collection->players[player_index].key, sizeof(update->key));
	else
		memset(update->key, 0, sizeof(update->key));
	update->other_player_index = other_index;
	if (collection->player_mask & (1 << other_index))
		memcpy(update->other_key, collection->players[other_index].key, sizeof(update->other_key));
	else
		memset(update->other_key, 0, sizeof(update->other_key));
	simulation_player_collection_apply_update(collection, update);
}

// @retail 0x84be0
dword function_84be0(const s_type_c67652 *collection)
{
	dword mask = 0;
	for (long i = 0; i < 16; i++)
	{
		if (collection->player_mask & (1 << i))
		{
			if (!collection->players[i].flag0c)
				mask |= 1 << i;
			else
				mask &= ~(1 << i);
		}
	}
	return mask;
}

void function_152df0(long player_index);

// @retail 0x84f30
bool function_84f30(const s_simulation_player_identity *identity)
{
	s_simulation_player_datum *player;
	long index = identity->index;
	player = simulation_player_at_index(g_4e8c24, index);
	bool result = false;
	if (player)
	{
		if (memcmp(player->key, identity->key, sizeof(player->key)) == 0 && !player->field_2_2)
		{
			function_152df0(data_datum_index(g_4e8c24, index));
			result = true;
		}
	}
	return result;
}


void function_14c320(long first, long second);

// @retail 0x84fb0
bool function_84fb0(const s_simulation_player_update *update)
{
 long first_index = update->player_index;
 s_simulation_player_datum *first = simulation_player_at_index(g_4e8c24, first_index);
 long second_index = update->other_player_index;
 s_simulation_player_datum *second = simulation_player_at_index(g_4e8c24, second_index);
 bool result = false;
 if ((first || second) && (!first || (*(byte *)((byte *)first + 2) & 2)) &&
  (!second || (*(byte *)((byte *)second + 2) & 2)))
 {
  t_player_key first_key;
  t_player_key second_key;
  if (first)
   memcpy(first_key, first->key, sizeof(first_key));
  else
   memset(first_key, 0, sizeof(first_key));
  if (second)
   memcpy(second_key, second->key, sizeof(second_key));
  else
   memset(second_key, 0, sizeof(second_key));
  if (!memcmp(first_key, update->key, sizeof(first_key)) &&
   !memcmp(second_key, update->other_key, sizeof(second_key)))
  {
   function_14c320(first_index, second_index);
   result = true;
  }
 }
 return result;
}

void __stdcall function_14be90(long player_index, const dword *configuration);

// @retail 0x854c0
bool __stdcall function_854c0(const s_simulation_player_update *update)
{
 const s_simulation_player_update *const *update_reference = &update;
 update = *update_reference;
 s_record_pool *data = g_4e8c24;
 long index = update->player_index;
 s_simulation_player_datum *player = simulation_player_at_index(data, index);
 bool result = false;
 if (player && !memcmp(player->key, update->key, sizeof(player->key)))
 {
  function_14be90(data_datum_index(data, index), update->configuration);
  result = true;
 }
 return result;
}

#pragma pack(push, 1)
struct s_player_creation_record
{
 bool active;
 bool field_2_2;
 short controller_index;
 long value04;
 s_machine_address machine;
 dword key[3];
 byte unknown1a[2];
 dword configuration[0x24];
 byte unknownac[0xe4 - 0xac];
};
#pragma pack(pop)

struct s_player_machine_table
{
 byte unknown00[0x2c];
 dword mask;
 s_machine_address machines[16];
};

long function_14bc00(long player_index, const s_player_creation_record *record);
void __stdcall function_14bf80(long player_index, const s_player_creation_record *record);

/* the team a new player's record keeps: pinned to the eight teams in a team
   game, the player's own index otherwise */
static inline void creation_record_set_team(char *team, const s_simulation_player_update *update)
{
 char value = *team;
 if (value == NONE)
  return;
 if (g_4e6948->flags184.bit0)
 {
  if (value < 0)
  {
   *team = 0;
   return;
  }
  if (value > 7)
  {
   *team = 7;
   return;
  }
 }
 else
  value = (char)update->player_index;
 *team = value;
}

// @retail 0x85140
bool __stdcall function_85140(const s_simulation_player_update *update)
{
 const s_simulation_player_update *const *update_reference = &update;
 update = *update_reference;
 s_record_pool *data = g_4e8c24;
 long index = update->player_index;
 s_simulation_player_datum *existing = simulation_player_at_index(data, index);
 volatile bool result = false;
 long machine_index = NONE;
 if (!update->field_2_2 &&
  (update->controller_index < 0 || update->controller_index >= 4 ||
   update->unknown20 < 0 || update->unknown20 >= 4))
  return result;
 if (!update->field_2_2)
 {
  const s_player_machine_table *table = (const s_player_machine_table *)g_4e8c20;
  dword mask = table->mask;
  s_machine_address machines[16];
  memcpy(machines, table->machines, sizeof(machines));
  for (long i = 0; i < 16; i++)
   if ((mask & (1 << i)) && !memcmp(&update->machine, &machines[i], sizeof(s_machine_address)))
   {
    machine_index = i;
    break;
   }
  if (machine_index == NONE)
   return result;
 }
 s_record_pool_iterator iterator;
 s_simulation_player_datum *player;
 iterator.data = g_4e8c24;
 iterator.index = NONE;
 while ((player = (s_simulation_player_datum *)data_iterator_next_inlined(&iterator)) != 0)
 {
  if ((short)iterator.datum_index != index &&
   !memcmp(player->key, update->key, sizeof(player->key)))
   return result;
 }
 if (!update->field_2_2)
 {
  iterator.data = g_4e8c24;
  iterator.index = NONE;
  while ((player = (s_simulation_player_datum *)data_iterator_next_inlined(&iterator)) != 0)
  {
   if (player->machine_index == machine_index && (short)iterator.datum_index != index &&
    (player->index28 == update->controller_index || player->value24 == update->unknown20))
    return result;
  }
 }
 s_player_creation_record record;
 memset(&record, 0, sizeof(record));
 bool special = update->field_2_2;
 record.active = true;
 record.field_2_2 = special;
 if (!special)
 {
  record.machine = update->machine;
  record.controller_index = (short)update->controller_index;
  record.value04 = update->unknown20;
 }
 else
 {
  record.controller_index = NONE;
  record.value04 = NONE;
 }
 memcpy(record.key, update->key, sizeof(record.key));
 memcpy(record.configuration, update->configuration, sizeof(record.configuration));
 creation_record_set_team((char *)record.configuration + 0x7c, update);
 if (!existing)
 {
  if (function_14bc00(index, &record) != NONE)
   result = true;
 }
 else if (!memcmp(existing->key, record.key, sizeof(record.key)) && existing->field_2_2)
 {
  long datum = NONE;
  if (index != NONE)
   datum = (((short *)(data->data + data->size * index))[0] << 16) | index;
  function_14bf80(datum, &record);
  result = true;
 }
 return result;
}
