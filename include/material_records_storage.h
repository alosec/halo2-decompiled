#ifndef MATERIAL_RECORDS_STORAGE_H
#define MATERIAL_RECORDS_STORAGE_H

#include <stddef.h>
#include "unknown_11c920.h"
#include "index_cache_storage.h"




struct s_2cb30_state
{
	byte unknown00[0x2a78];
	long active;
};

struct s_2cb30_entry
{
	dword values[9];
};

/* the record positions follow the 850 records in one object: a copy into
   the records may write them, so they are stored before and reloaded after */
struct s_2cb30_globals
{
	long current;
	long previous;
	s_2cb30_state *state;
	union
	{
		byte unknown0c[0x9b0 - 0xc];
		struct
		{
			byte weights[850];
			byte material_indices[850];
			long materials[192];
		};
	};
	long count;
	s_2cb30_entry entries[32];
	long entry_count;
};

struct s_material_records_storage
{
    s_44940_entry field_0[850];
    s_2cb30_globals positions;
};

extern s_material_records_storage g_material_records_storage;
#define g_4ba138 (g_material_records_storage.field_0)
#define g_4c0b78 (g_material_records_storage.positions)
typedef char assert_material_records_offset_0[(offsetof(s_material_records_storage, field_0) == 0x0) ? 1 : -1];
typedef char assert_material_records_offset_6a40[(offsetof(s_material_records_storage, positions) == 0x6a40) ? 1 : -1];

typedef char assert_material_records_size[(sizeof(s_material_records_storage) == 0x6a40 + 0xe38) ? 1 : -1];

#endif
