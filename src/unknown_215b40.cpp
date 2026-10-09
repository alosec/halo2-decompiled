#include "unknown_11c920.h"
#include "unknown_19d220.h"
#include <string.h>
#ifdef H2_NATIVE_PORT
#include <wchar.h>
#endif

// @flags /O2 /Ob1 /Gr

class c_location_record
{
public:
	c_location_record();
	char path[0x14];
	word name[0x16];
};

struct s_location_record_table
{
	long count;
	c_location_record entries[0x1000];
};

extern void *g_51ea14;
bool function_216800(void *location, long file_index);

// @retail 0x215b40
c_location_record::c_location_record()
{
	path[0] = 0;
	name[0] = 0;
}

// @retail 0x216060
bool function_216060(long file_index, word const *name)
{
	bool result = false;
	struct
	{
		char path[0x14];
		word name[0x16];
	} location;
	location.path[0] = 0;
	location.name[0] = 0;
	if (function_216800(&location, file_index) && name && name[0])
	{
		if (wcsncmp(location.name, name, 0x80))
			result = true;
	}
	return result;
}

// @retail 0x216990
bool function_216990(long unit, c_location_record const *record, long *index)
{
	bool result = false;
	if (g_51ea14)
	{
		s_location_record_table *table = (s_location_record_table *)((byte *)g_51ea14 + 0xbef8) + unit;
		if (table->count != 0x1000)
		{
			long slot = table->count++;
			table->entries[slot] = *record;
			*index = table->count - 1;
			result = true;
		}
	}
	return result;
}

// @retail 0x2169e0
void function_2169e0(long file_index)
{
	if (g_51ea14)
	{
		long unit = (file_index >> 4) & 0xf;
		long index = (file_index >> 8) & 0x1fff;
		s_location_record_table *table = (s_location_record_table *)((byte *)g_51ea14 + 0xbef8) + unit;
		long count = table->count;
		long bounded_index = index < 0 ? 0 : index > count - 1 ? count - 1 : index;
		if (bounded_index == index)
		{
			if (index < count - 1)
			{
				c_location_record *record = &table->entries[index];
				memmove(record, record + 1, (count - index - 1) * sizeof(c_location_record));
			}
			table->count--;
		}
	}
}

// @retail 0x217300
long function_217300(long file_index, long removed_index)
{
	if (!((bool)(((dword)file_index >> 21) & 1)))
	{
		void *storage = g_51ea14;
		if (storage)
		{
			long unit = (file_index >> 4) & 0xf;
			long removed_unit = (removed_index >> 4) & 0xf;
			if (removed_unit == unit)
			{
				long index = (file_index >> 8) & 0x1fff;
				long removed = (removed_index >> 8) & 0x1fff;
				if (index > removed)
				{
					long generation = *(long *)((byte *)storage + 0x4befc);
					file_index = (file_index & 0xf) | ((unit & 0xf) << 4) |
						(((index - 1) & 0x1fff) << 8) | ((generation & 0x1ff) << 22);
				}
			}
		}
	}
	return file_index;
}

struct s_cached_location_entry
{
	long type;
	byte data[0x1e0];
};

struct s_cached_location_table
{
	long count;
	s_cached_location_entry entries[1];
};

PRIVATE __forceinline word *copy_location_name(word *destination, word const *source)
{
	wcsncpy(destination, source, 0x7f);
	destination[0x7f] = 0;
	return destination;
}

// @retail 0x215b50
word *function_215b50(long file_index, word *name)
{
	word *result = name;
	s_cached_location_table *files = (s_cached_location_table *)g_51ea14;
	name[0] = 0;
	if (files && (bool)(((dword)file_index >> 21) & 1))
	{
		long type = file_index & 0xf;
		long index = (file_index >> 8) & 0x1fff;
		long bounded_index = index < 0 ? 0 : index > files->count - 1 ? files->count - 1 : index;
		if (bounded_index == index)
		{
			s_cached_location_entry *entry = &files->entries[index];
			if (type == 0)
			{
				return copy_location_name(result, (word *)(entry->data + 8));
			}
			else
			{
				long bounded_type = type < 1 ? 1 : type > 9 ? 9 : type;
				if (bounded_type == type)
				{
					return copy_location_name(result, (word *)(entry->data + 4));
				}
			}
		}
	}
	else
	{
		struct
		{
			char path[0x14];
			word name[0x16];
		} location;
		location.path[0] = 0;
		location.name[0] = 0;
		if (function_216800(&location, file_index))
		{
			return copy_location_name(result, location.name);
		}
	}
	return result;
}

struct s_player_profile;
void function_1a0750(s_player_profile *profile);
const word *function_217a20(long language);
long g_55c280;

// @retail 0x216760
void function_216760(void)
{
	if (g_51ea14 && ((s_cached_location_table *)g_51ea14)->count != 0x65)
	{
		s_cached_location_entry entry;
		word name[0x100];
		entry.type = 0;
		name[0] = 0;
		word const *default_name = function_217a20(g_55c280);
		word const *const *name_reference = &default_name;
		wcsncpy(name, *name_reference, 0xff);
		name[0xff] = 0;
		function_1a0750((s_player_profile *)entry.data);
		wcsncpy((word *)(entry.data + 8), name, 0x1f);
		((word *)(entry.data + 8))[0x1f] = 0;
		s_cached_location_table *files = (s_cached_location_table *)g_51ea14;
		long index = files->count++;
		files->entries[index] = entry;
	}
}

long saved_game_file_type_from_variant(s_game_variant *variant);

// @retail 0x217080
void __stdcall function_217080(s_game_variant const *variant)
{
	s_game_variant const *const *variant_reference = &variant;
	if (g_51ea14 && ((s_cached_location_table *)g_51ea14)->count != 0x65)
	{
		union
		{
			s_cached_location_entry entry;
			unsigned __int64 words[0x1e8 / 8];
		} value;
		*(s_game_variant *)value.entry.data = **variant_reference;
		value.entry.type = saved_game_file_type_from_variant((s_game_variant *)value.entry.data);
		s_cached_location_table *files = (s_cached_location_table *)g_51ea14;
		s_cached_location_entry *destination = &files->entries[files->count++];
		*destination = value.entry;
	}
}

typedef bool (__stdcall *t_location_compare)(long, long, void const *);
void sort_4byte(long *elements, unsigned long count, void *unused, t_location_compare compare, void const *context);
bool function_1249f0(long memory_unit, char *drive_letter);

// @retail 0x2163d0
bool __stdcall function_2163d0(long first, long second, void const *context)
{
	bool result;
	dword first_cached = (dword)first >> 21;
	dword second_cached = (dword)second >> 21;
	if (!((first_cached ^ second_cached) & 1))
		result = first > second;
	else
		result = (bool)((byte)first_cached & 1);
	return result;
}

// @retail 0x215900
void __stdcall function_215900(long controller, long type, word *capacity, long *indices, long include_cached)
{
	(void)&controller;
	(void)&type;
	(void)&capacity;
	(void)&indices;
	(void)&include_cached;
	long count = 0;
	if (g_51ea14)
	{
		long units = controller == NONE || controller == 0xff ? 1 : 3;
		long unit = 0;
		for (long pass = 0; pass < units && count < *capacity; pass++)
		{
			char drive_letter;
			if (function_1249f0(unit, &drive_letter))
			{
				s_location_record_table *table = (s_location_record_table *)((byte *)g_51ea14 + 0xbef8) + unit;
				long index = 0;
				do
				{
					if (index >= table->count)
						break;
					struct
					{
						char path[0x14];
						word name[0x12];
						long type;
						byte unknown3c[4];
					} entry;
					memcpy(&entry, &table->entries[index], sizeof(entry));
					long old_index = index++;
					if (entry.type == type)
					{
						long generation = *(long *)((byte *)g_51ea14 + 0x4befc);
						indices[count++] = (type & 0xf) | ((unit & 0xf) << 4) |
							((old_index & 0x1fff) << 8) | ((generation & 0x1ff) << 22);
					}
				} while (count < *capacity);
			}
			if (controller == 0xff)
				unit++;
			else
			{
				switch (controller)
				{
				case NONE: unit++; break;
				case 0: unit = pass + 1; break;
				case 1: unit = pass + 3; break;
				case 2: unit = pass + 5; break;
				case 3: unit = pass + 7; break;
				default: __assume(0); break;
				}
			}
		}
		if ((byte)include_cached)
		{
			s_cached_location_table *files = (s_cached_location_table *)g_51ea14;
			long cached_count = files->count;
			for (long index = 0; count < *capacity && index < cached_count; index++)
			{
				if (files->entries[index].type == type)
				{
					long generation = *(long *)((byte *)files + 0x4befc);
					indices[count++] = (type & 0xf) | ((index & 0x1fff) << 8) |
						(1 << 21) | ((generation & 0x1ff) << 22);
				}
			}
			sort_4byte(indices, count, &cached_count, function_2163d0, NULL);
		}
	}
	*capacity = (word)count;
}

bool function_2161d0(long file_index, void *buffer, long size);
bool function_19d650(s_game_variant *variant);

// @retail 0x212bc0
bool function_212bc0(long file_index, s_game_variant *variant)
{
	bool result = false;
	if (file_index != NONE && function_2161d0(file_index, variant, sizeof(*variant)))
	{
		union
		{
			s_game_variant value;
			unsigned __int64 words[0x130 / 8];
		} copy;
		copy.value = *variant;
		result = function_19d650(&copy.value);
	}
	return result;
}

// @retail 0x212c20
bool __stdcall function_212c20(word const *name, s_game_variant *variant, long *file_index)
{
	(void)&name;
	(void)&variant;
	(void)&file_index;
	bool result = false;
	if (file_index)
		*file_index = NONE;
	if (name[0])
	{
		for (long type = 1; type <= 9 && !result; type++)
		{
			long indices[0x1000];
			long capacity = 0x1000;
			word candidate[0x100];
			function_215900(0xff, type, (word *)&capacity, indices, true);
			for (long index = 0; index < (word)capacity; index++)
			{
				if (!_wcsicmp(function_215b50(indices[index], candidate), name))
				{
					result = true;
					if (variant)
						result = function_212bc0(indices[index], variant);
					if (result && file_index)
						*file_index = indices[index];
					break;
				}
			}
		}
	}
	else if (variant)
	{
		function_19d220(variant, 0);
		result = true;
	}
	return result;
}


struct s_variant_preset_212f40 { word const *name; long index; long type; };
PRIVATE s_variant_preset_212f40 const g_445958[28] =
{
    { L"Slayer Duel", 0, 0 },
    { L"Rumble Slayer", 1, 0 },
    { L"Team Slayer", 2, 0 },
    { L"Rumble Rockets", 3, 0 },
    { L"Team Rockets", 4, 0 },
    { L"Rumble Shotguns", 5, 0 },
    { L"Pistols", 6, 0 },
    { L"Rifle Duel", 7, 0 },
    { L"Brutes", 8, 0 },
    { L"Sword Duel", 9, 0 },
    { L"Snipers", 10, 0 },
    { L"Elimination", 11, 0 },
    { L"Multi Flag CTF", 12, 4 },
    { L"Classic CTF", 13, 4 },
    { L"Shotgun CTF", 14, 4 },
    { L"1 Flag CTF", 15, 4 },
    { L"1 Flag CTF Fast", 16, 4 },
    { L"Multi Bomb", 17, 5 },
    { L"Assault", 18, 5 },
    { L"Rapid Assault", 19, 5 },
    { L"Major Assault", 20, 5 },
    { L"Minor Assault", 21, 5 },
    { L"Crazy King", 22, 3 },
    { L"Team Crazy King", 23, 3 },
    { L"Oddball", 24, 1 },
    { L"Teamball", 25, 1 },
    { L"Three Plots", 26, 6 },
    { L"Land Grab", 27, 6 },
};

// @retail 0x212f40
void function_212f40(long preset, s_game_variant *variant)
{
    s_variant_preset_212f40 const *entry = &g_445958[preset];
    function_19d220(variant, entry->type);
    wcsncpy(variant->name, entry->name, 31);
    variant->name[31] = 0;
    variant->unknown78 = 16;
    variant->unknown74 = 16;
    switch (preset)
    {
    case 0: variant->unknown50 = 10; break;
    case 1: variant->unknown54 = 360; break;
    case 2: variant->flags48 |= 1; break;
    case 3:
    case 4:
        variant->teams_enabled = preset == 4;
        variant->unknown54 = 360;
        variant->unknownd4 = 2;
        variant->motion_sensor_enabled = false;
        variant->unknown84 = 0;
        variant->unknownd6 = 8;
        variant->unknownd7 = 1;
        break;
    case 5:
        variant->unknowncf = 4; variant->unknownd0 = 4; variant->unknownd1 = 4;
        variant->unknown54 = 360; variant->unknownd4 = 3; variant->unknownd6 = 9; variant->unknownd7 = 1;
        variant->unknownd2 = 6; variant->unknownd3 = 6;
        break;
    case 6: variant->unknownd4 = 14; variant->unknownd6 = 4; variant->unknownd7 = 1; break;
    case 7:
        variant->unknown50 = 10; variant->unknownd4 = 11; variant->unknownd6 = 13; variant->unknownd7 = 4;
        variant->unknowncf = 4; variant->unknownd0 = 4; variant->unknownd1 = 4;
        break;
    case 8: variant->unknownd4 = 5; variant->unknownd6 = 11; variant->unknownd7 = 1; break;
    case 9:
        variant->unknownd4 = 4; variant->unknowncf = 4; variant->unknownd0 = 4; variant->unknownd1 = 4;
        variant->unknown50 = 10; variant->unknownd6 = 17; variant->unknownd7 = 1; variant->unknown88 = 2;
        variant->unknowncd = 7; variant->unknownce = 7; variant->unknownd2 = 6; variant->unknownd3 = 6;
        break;
    case 10:
        variant->unknownd4 = 12; variant->flags48 &= ~2; variant->unknownd7 = 4;
        variant->unknowncf = 4; variant->unknownd0 = 4; variant->unknownd1 = 4;
        variant->unknownd6 = 10; variant->unknowncd = 7; variant->unknownce = 7;
        variant->unknownd2 = 6; variant->unknownd3 = 6;
        break;
    case 11: variant->unknown50 = 0; variant->unknown4c = 5; variant->unknown7c = 1; break;
    case 12: variant->unknown54 = 720; break;
    case 13: variant->engine_flags |= 12; break;
    case 14:
        variant->engine_flags |= 12; variant->unknownd4 = 3; variant->unknownd6 = 9; variant->unknownd7 = 1;
        variant->unknowncf = 4; variant->unknownd1 = 4;
        break;
    case 15:
        variant->unknown4c = 2; variant->unknown54 = 180; variant->unknown50 = 1; variant->flag.unknown104 = 1;
        break;
    case 16:
        variant->unknown4c = 2; variant->unknown54 = 120; variant->unknown80 = 5; variant->unknown50 = 1;
        variant->unknown84 = 5; variant->flag.unknown104 = 1; variant->flag.unknownf4 = 15;
        break;
    case 17: variant->unknown54 = 720; break;
    case 18:
        variant->unknown4c = 2; variant->unknown50 = 1; variant->unknown54 = 180; variant->flag.unknown104 = 1;
        break;
    case 19:
        variant->unknown4c = 2; variant->unknown80 = 5; variant->unknown54 = 120; variant->unknown50 = 1;
        variant->unknown84 = 5; variant->flag.unknown104 = 1; variant->flag.unknownf4 = 20;
        break;
    case 20: variant->flag.unknown104 = 2; variant->flag.unknown108 = 10; variant->unknown54 = 720; break;
    case 21: variant->flag.unknown104 = 2; variant->flag.unknown108 = 10; break;
    case 22: variant->unknowna8 = 0; variant->unknown54 = 360; break;
    case 23:
        variant->flags48 |= 1; variant->unknowna8 = 0; variant->unknown80 = 10;
        variant->engine_flags |= 1; variant->unknown50 = 60;
        break;
    case 24: variant->unknown54 = 360; break;
    case 25: variant->flags48 |= 1; variant->unknowna8 = 0; variant->unknown80 = 10; break;
    case 26:
        variant->flags48 |= 1; variant->unknowna8 = 0; variant->unknown84 = 5;
        variant->territories.unknownf2 = 3; variant->unknown50 = 180;
        break;
    case 27:
        variant->flags48 |= 1; variant->unknown54 = 720; variant->territories.unknownf0 = 5; variant->unknowna8 = 0;
        break;
    }
}


#include <xtl.h>

extern bool g_55c14c;
void function_125d60(void);

// @retail 0x2160b0
bool function_2160b0(long file_index, char *path)
{
    bool result = false;
    if (!((bool)(((dword)file_index >> 21) & 1)))
    {
        struct
        {
            char path[0x14];
            word name[0x16];
        } location;
        location.path[0] = 0;
        location.name[0] = 0;
        if (!g_55c14c)
        {
            while (!g_55c14c)
            {
                SwitchToThread();
                function_125d60();
            }
        }
        if (function_216800(&location, file_index))
        {
            strncpy(path, location.path, 0x100);
            path[0xff] = 0;
            result = true;
        }
    }
    return result;
}


struct s_saved_game_file_location
{
    char name[0x14];
    wchar_t display_name[0x12];
    long type;
    byte unknown3c;
    char language;
    byte unknown3e[2];
};

char *function_216400(long memory_unit, char *buffer, short capacity);
bool function_216da0(wchar_t *name, long type, wchar_t const *display_name, long language);

// @retail 0x215e60
void __stdcall function_215e60(long file_index)
{
    (void)&file_index;
    if (file_index != NONE)
    {
        if (!g_55c14c)
        {
            while (!g_55c14c)
            {
                SwitchToThread();
                function_125d60();
            }
        }
        long unit = (file_index >> 4) & 15;
        s_saved_game_file_location location;
        location.name[0] = 0;
        location.display_name[0] = 0;
        if (function_216800(&location, file_index))
        {
            if (!((bool)(((dword)file_index >> 21) & 1)))
            {
                char root[8] = "";
                char drive;
                if (function_1249f0(unit, &drive))
                {
                    wchar_t name[128];
                    name[0] = 0;
                    function_216da0(name, file_index & 15, location.display_name, location.language);
                    XDeleteSaveGame(function_216400(unit, root, 8), name);
                }
            }
            function_2169e0(file_index);
        }
    }
}


#include "files.h"

extern long g_55c154;
long function_217590(long memory_unit);
bool function_216d70(char *path, const char *directory, long type);
s_type_acf665 *function_136710(s_type_acf665 *file, bool replace, const char *name);
bool function_216f80(long type, s_saved_game_file_location *location);
long saved_game_file_type_size_in_blocks(long type);
bool saved_game_storage_has_free_blocks(long blocks);

// @retail 0x215c00
long function_215c00(long type, wchar_t const *display_name)
{
    long result = NONE;
    long const *type_reference = &type;
    (void)&display_name;
    if (!g_55c14c)
    {
        while (!g_55c14c)
        {
            SwitchToThread();
            function_125d60();
        }
    }
    if (g_51ea14)
    {
        long count = function_217590(0);
        if ((g_51ea14 && *(long *)((byte *)g_51ea14 + 0xbef8) >= 0x1000) || count >= 0x1000)
        {
            g_55c154 = 1;
            goto done;
        }
        long index = g_55c280;
        wchar_t name[128];
        name[0] = 0;
        if (function_216da0(name, (*type_reference), display_name, index))
        {
            char directory[256] = "";
            char root[8];
            function_216400(0, root, 8);
            if (!XCreateSaveGame(root, name, 1, 0, directory, sizeof(directory)))
            {
                s_saved_game_file_location location;
                memset(&location, 0, sizeof(location));
                char path[256];
                path[0] = 0;
                wcsncpy(location.display_name, display_name, 16);
                location.display_name[16] = 0;
                location.type = (*type_reference);
                location.language = (char)index;
                strncpy(location.name, directory, sizeof(location.name));
                location.name[sizeof(location.name) - 1] = 0;
                function_216d70(path, location.name, (*type_reference));
                struct
                {
                    dword signature;
                    word flags;
                    short location;
                    char path[256];
                    __int64 state;
                } file;
                if (function_136710((s_type_acf665 *)&file, false, path) &&
                    function_1367d0((s_type_acf665 *)&file) &&
                    function_216f80((*type_reference), (s_saved_game_file_location *)directory) &&
                    function_216990(0, (c_location_record const *)&location, &index))
                {
                    long generation = *(long *)((byte *)g_51ea14 + 0x4befc);
                    result = (((generation & 0x1ff) << 14 | (index & 0x1fff)) << 8) | ((*type_reference) & 15);
                }
                if (result == NONE)
                {
                    wchar_t remove_name[128];
                    remove_name[0] = 0;
                    function_216da0(remove_name, (*type_reference), display_name, location.language);
                    XDeleteSaveGame(root, remove_name);
                }
            }
            if (result == NONE)
                g_55c154 = saved_game_storage_has_free_blocks(saved_game_file_type_size_in_blocks((*type_reference))) ? 4 : 2;
            else
                g_55c154 = 0;
        }
    }
done:
    return result;
}


struct s_saved_game_read;
struct s_saved_game_file_task;
char *function_122810(char *path, const char *suffix);
bool signed_file_read_begin(void *header, dword header_size, void *body, dword body_size, bool non_roamable, s_saved_game_file_task *task, const char *path);
bool signed_file_write_begin(void *header, dword header_size, void *body, dword body_size, bool non_roamable, s_saved_game_file_task *task, const char *path);

// @retail 0x2174b0
bool __stdcall function_2174b0(long file_index, void *header, long header_size, void *body, long body_size, s_saved_game_read *task)
{
    (void)&header;
    (void)&header_size;
    (void)&body;
    (void)&body_size;
    (void)&task;
    bool result = false;
    char path[256];
    path[0] = 0;
    if (function_2160b0(file_index, path))
    {
        function_122810(path, "auxilary.bin");
        result = signed_file_read_begin(header, header_size, body, body_size, false, (s_saved_game_file_task *)task, path);
    }
    return result;
}

// @retail 0x217520
bool __stdcall function_217520(long file_index, void const *header, long header_size, void const *body, long body_size, s_saved_game_read *task)
{
    (void)&header;
    (void)&header_size;
    (void)&body;
    (void)&body_size;
    (void)&task;
    bool result = false;
    char path[256];
    path[0] = 0;
    if (function_2160b0(file_index, path))
    {
        function_122810(path, "auxilary.bin");
        result = signed_file_write_begin((void *)header, header_size, (void *)body, body_size, false, (s_saved_game_file_task *)task, path);
    }
    return result;
}

bool function_216240(long arg_0, void *arg_1, long arg_2, wchar_t *arg_3);

// @retail 0x212ec0
bool function_212ec0(s_game_variant *arg_0, long arg_1)
{
    long const *local_0 = &arg_1;
    bool local_1 = true;
    if (*local_0 != NONE)
    {
        function_19d650(arg_0);
        union
        {
            s_game_variant field_0;
            unsigned __int64 field_8[0x130 / 8];
        } local_2;
        local_2.field_0 = *arg_0;
        local_1 = function_216240(*local_0, &local_2.field_0, sizeof(local_2), arg_0->name);
    }
    return local_1;
}
