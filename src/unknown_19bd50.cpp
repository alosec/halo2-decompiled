// @flags /O2 /Gr
/* UNKNOWN_19BD50.CPP: the level handle tables (entry 13 of the game
   module table) and the multiplayer maps loaded from files */

#include "unknown_11c920.h"
#include "globals.h"
#include "data_array.h"
#include "crc.h"
#include "files.h"
#include <string.h>

// @retail 0x19bcd0
void level_handle_tables_initialize(void)
{
	s_record_pool *data = data_new_inlined("level handles", 20, 8, 0, g_468758);
	g_4ee4e8 = data;
	data = data_new_inlined("multiplayer level handles", 50, 8, 0, g_468758);
	g_4ee4e4 = data;
}

// @retail 0x19bd50
void level_handle_tables_dispose(void)
{
	if (g_4ee4e4)
	{
		data_dispose(g_4ee4e4);
		g_4ee4e4 = 0;
	}
	if (g_4ee4e8)
	{
		data_dispose(g_4ee4e8);
		g_4ee4e8 = 0;
	}
}

/* a multiplayer map file's header: a checksum of the whole file (0x2d3fc
   bytes) with its own field cleared */
struct s_map_file_header
{
	long type;
	dword checksum;
};

__declspec(noinline) bool map_file_checksum_valid(s_map_file_header *header);

// @retail 0x19bdc0
bool map_file_checksum_valid(s_map_file_header *header)
{
	if (header->type != 2)
		return false;

	dword checksum = header->checksum;
	dword crc;

	header->checksum = 0;
	crc = 0xffffffff;
	function_163ba0(&crc, header, 0x2d3fc);
	bool valid = checksum == crc;
	header->checksum = checksum;

	return valid;
}

struct s_map_package
{
	s_map_file_header header;
	long kind;
	char path[0x100];
	long map_id;
	long unknown110;
	byte names[9][0x40];
	byte descriptions[9][0x100];
	long valuec54;
	byte flags;
	byte unknownc59[3];
	dword metadatac5c[4];
	byte images[207][0x370];
};

struct s_level_tags
{
	long tag_index;
	long bitmap_tag_index;
};

struct s_loaded_map_view
{
	long map_id;
	s_level_tags tags;
	byte names[9][0x40];
	byte descriptions[9][0x100];
	char path[0x100];
	long valuec4c;
	byte flags;
	byte unknownc51[3];
	dword metadatac54[4];
};

struct s_content_item
{
	char directory[0x104];
	word display_name[0x80];
};

struct s_entry_c;
struct s_bitmap_data;
struct s_bitmap_view;
struct D3DTexture;
struct s_level_path { char string[0x104]; };
s_entry_c *function_19c5f0(long map_id);
D3DTexture *function_1cfb0(s_bitmap_view *bitmap);
void *function_1d5e0(s_bitmap_view *bitmap, long wait, long *pitch);
bool function_12cb80(s_bitmap_data *bitmap);
char *level_path_print(s_level_path *path, char const *format, ...);
s_type_acf665 *file_ref_create(s_type_acf665 *file, bool replace, const char *name);
bool function_1368f0(s_type_acf665 *file);

// @retail 0x19be00
bool __stdcall function_19be00(s_content_item *item, s_map_package *package)
{
	bool result = false;
	if (package->kind == 1)
	{
		result = map_file_checksum_valid(&package->header);
		if (result)
		{
			s_loaded_map_view *entry = (s_loaded_map_view *)function_19c5f0(package->map_id);
			if (!entry)
				entry = (s_loaded_map_view *)function_19c5f0(NONE);
			if (!entry)
				result = false;
			else
			{
				s_level_tags tags = entry->tags;
				memset(entry, 0, sizeof(*entry));
				memcpy(entry->descriptions, package->descriptions, sizeof(entry->descriptions));
				entry->flags = package->flags;
				entry->map_id = package->map_id;
				memcpy(entry->metadatac54, package->metadatac5c, sizeof(entry->metadatac54));
				memcpy(entry->names, package->names, sizeof(entry->names));
				memcpy(entry->path, package->path, sizeof(entry->path));
				entry->valuec4c = package->valuec54;
				entry->tags.bitmap_tag_index = tags.bitmap_tag_index;
				entry->tags.tag_index = tags.tag_index;
				long const *bitmap_tag_index = &entry->tags.bitmap_tag_index;
				s_bitmap_data *bitmap = *(s_bitmap_data **)(g_4e3b44[*bitmap_tag_index & 0xffff].bytes + 0x48);
				function_1cfb0((s_bitmap_view *)bitmap);
				function_12cb80(bitmap);
				long stride;
				void *bits = function_1d5e0((s_bitmap_view *)bitmap, false, &stride);
				if (bits)
				{
					byte *pixels = (byte *)bits;
					byte const *image = package->images[0];
					for (long row = 0; row < 207; row++)
					{
						memcpy(pixels, image, sizeof(package->images[0]));
						image += sizeof(package->images[0]);
						pixels += stride;
					}
					function_1cfb0((s_bitmap_view *)bitmap);
				}
				s_level_path path;
				path.string[0] = 0;
				char const *name = strrchr(package->path, '\\');
				if (name)
					name++;
				else
					name = package->path;
				level_path_print(&path, "%s\\%s", item->directory, name);
				char *destination = entry->path;
				strncpy(destination, path.string, sizeof(entry->path));
				destination[sizeof(entry->path) - 1] = 0;
			}
		}
	}
	return result;
}

// @retail 0x19bfd0
bool __stdcall function_19bfd0(s_content_item *item)
{
	s_type_acf665 file;
	s_map_package package;
	package.flags = 0;
	file_ref_create(&file, true, item->directory);
	bool result = false;
	if ((byte)file.flags & 1)
		function_1373c0(file.path);
	function_137320(file.path, "patch.lvl");
	*(byte *)&file.flags |= 1;
	if (function_1368f0(&file))
	{
		dword error;
		result = function_136970(&file, 1, &error);
		if (!result)
			return result;
		result = file_ref_read(&file, 0x2d3fc, &package, false);
		function_136bb0(&file);
		if (result)
			return function_19be00(item, &package);
	}
	return result;
}
