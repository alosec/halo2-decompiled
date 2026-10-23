// @flags /O2 /GL-
// Havok (third-party physics library): removing a pair of collidables from a
// cache of pairs: from the sorted pairs still being merged, or else from the
// unsorted pairs of either collidable's island, or from the agents of an
// island, ending each agent found.

class island_316110;
class cache_316110;

// a collidable: its id orders the pairs
class collidable_316110
{
public:
	long field_0;
	unsigned long id;
};

class owner_316110
{
public:
	char unknown0[0x44];
	island_316110 *island;
};

class base_316110
{
public:
	char unknown0[0x10];
};

// the entity a collidable belongs to
class entity_316110 : public base_316110, public collidable_316110
{
public:
	char unknown18[0x20 - 0x18];
	owner_316110 *owner;
};

class agent_owner_316110
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void agent_ended() {}
};

class agent_316110
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void slot_4() {}
	virtual void end() {}

	long field_4;
	agent_owner_316110 *owner;
};

// a pair of collidables
struct pair_316110
{
	collidable_316110 *first;
	collidable_316110 *second;
};

// a pair with the agent that handles it
struct record_316110
{
	collidable_316110 *first;
	collidable_316110 *second;
	agent_316110 *agent;
	long field_c;
};

struct pairs_316110
{
	pair_316110 *data;
	int size;
};

struct records_316110
{
	record_316110 *data;
	int size;
};

// the pairs an island keeps: its agents' records, its unsorted pairs, and
// the sorted pairs being merged
class cache_316110
{
public:
	char unknown0[0xc];
	bool merging;
	char unknownd[0x4c - 0xd];
	records_316110 records;
	char unknown54[0x58 - 0x54];
	pairs_316110 pairs;
	char unknown60[0x80 - 0x60];
	record_316110 *next;
	record_316110 *end;
	record_316110 *written;

	void function_316110(pair_316110 const *pair);
};

class island_head_316110
{
public:
	char unknown0[0x1c];
};

class island_316110 : public island_head_316110, public cache_316110
{
};

// whether a record comes before a pair in the order of their ids
inline bool before_316110(record_316110 const *record, pair_316110 const *pair)
{
	if (record->first->id < pair->first->id)
	{
		return true;
	}
	if (record->first->id != pair->first->id)
	{
		return false;
	}
	return record->second->id < pair->second->id;
}

// the island of a collidable's entity
inline island_316110 *island_of_316110(collidable_316110 *collidable)
{
	return static_cast<entity_316110 *>(collidable)->owner->island;
}

// ends an agent, and tells its owner
inline void end_agent_316110(agent_316110 *agent)
{
	if (agent)
	{
		agent_owner_316110 *owner = agent->owner;
		agent->end();
		owner->agent_ended();
	}
}

// score 167 (was 185): the second collidable is now taken up front, so the
// pair dies in edx as retail's does; retail still reloads the pair from its
// slot inside the loop and keeps the array pointer in esi, ours keeps the
// second in the slot and the pointer in edx, and the two virtual calls swap
// their vtable registers
// @retail 0x316110
void cache_316110::function_316110(pair_316110 const *pair)
{
	record_316110 *record = next;
	record_316110 *const last = end;
	merging = true;
	while (record < end && before_316110(record, pair))
	{
		record++;
	}
	if (record != last && record->first->id == pair->first->id && record->second->id == pair->second->id)
	{
		while (next < record)
		{
			*written = *next;
			written++;
			next++;
		}
		end_agent_316110(next->agent);
		next++;
		return;
	}
	collidable_316110 *first = pair->first;
	collidable_316110 *second = pair->second;
	pairs_316110 *pairs = &island_of_316110(first)->pairs;
	int i;
	for (i = 0; i < pairs->size; i++)
	{
		if (pairs->data[i].first->id == first->id && pairs->data[i].second->id == second->id)
		{
			goto remove;
		}
	}
	{
		pairs = &island_of_316110(second)->pairs;
		for (i = 0; i < pairs->size; i++)
		{
			if (pairs->data[i].first->id == first->id && pairs->data[i].second->id == second->id)
			{
				goto remove;
			}
		}
		island_316110 *island = island_of_316110(first);
		if (island == static_cast<island_316110 *>(this))
		{
			island = island_of_316110(second);
		}
		records_316110 *records = &island->records;
		for (int j = 0; j < records->size; j++)
		{
			if (records->data[j].first->id == first->id && records->data[j].second->id == second->id)
			{
				end_agent_316110(records->data[j].agent);
				records->size--;
				for (int k = j; k < records->size; k++)
				{
					records->data[k] = records->data[k + 1];
				}
				return;
			}
		}
		return;
	}
remove:
	pairs->size--;
	pairs->data[i] = pairs->data[pairs->size];
}
