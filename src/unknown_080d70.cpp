#include "unknown_4cf790.h"
#include "unknown_11c920.h"
#include "unknown_2312b4.h"

// @flags /O2 /Oi /arch:SSE /Gr

struct s_player_configuration_cache_entry
{
	s_recent_player player;
	short unknown58;
	short next;
	short previous_other;
	short next_other;
	dword flags;
	union
	{
		byte unknown64[4];
		dword field_64;
	};
};


struct s_cache_property
{
	long unknown00;
	long type;
	long unknown08;
	long unknown0c;
};

struct s_cache_property_record
{
	byte identity[12];
	long type;
	long count;
	s_cache_property *properties;
};

long __stdcall function_7fc80(const void *identity, long *position);

// @retail 0x80d70
void function_80d70(s_cache_property_record *records, long count)
{
	long position;
	for (long i = 0; i < count; i++)
	{
		s_cache_property_record *record = &records[i];
		if (record->type == 0x61 && record->count == 4)
		{
			long index = function_7fc80(record->identity, &position);
			if (index != NONE)
			{
				s_cache_property *properties = record->properties;
				long valid = 0;
				if (properties[0].type == 4)
					valid++;
				if (properties[1].type == 1)
					valid++;
				if (properties[2].type == 1)
					valid++;
				if (properties[3].type == 1)
					valid++;
				if (valid == 4)
    {
     s_player_configuration_cache_entry *local_0 = g_4cf98c + index;
     local_0->flags &= ~1;
    }
			}
		}
	}
}
