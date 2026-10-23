#include "unknown_11c920.h"
#include "files.h"
#include <string.h>

// @flags /O2 /Gr

bool function_1249f0(long memory_unit, char *drive_letter);
char *function_11c9c0(char *buffer, long maximum_count, const char *format, ...);
char *function_122810(char *string, const char *suffix);
const char *saved_game_type_get_storage_name(long type);
bool function_1368f0(s_type_acf665 *file);

// @retail 0x216400
char *function_216400(long memory_unit, char *buffer, short capacity)
{
	char drive_letter;
	if (function_1249f0(memory_unit, &drive_letter))
		function_11c9c0(buffer, capacity, "%c:\\", drive_letter);
	else
		buffer[0] = 0;
	return buffer;
}

// @retail 0x216d70
bool function_216d70(char *path, const char *directory, long type)
{
	strncpy(path, directory, 0x100);
	path[0xff] = 0;
	function_122810(path, saved_game_type_get_storage_name(type));
	return true;
}

struct s_storage_file_reference
{
	dword signature;
	word flags;
	short location;
	char path[0x100];
	__int64 state;
};

// @retail 0x217590
long function_217590(long memory_unit)
{
	char root[8] = "";
	char drive_letter;
	long count = 0;
	if (function_1249f0(memory_unit, &drive_letter))
		function_11c9c0(root, sizeof(root), "%c:\\", drive_letter);
	else
		root[0] = 0;
	XGAME_FIND_DATA data;
	HANDLE find = XFindFirstSaveGame(root, &data);
	if (find != INVALID_HANDLE_VALUE)
	{
		do
		{
			count++;
		}
		while (XFindNextSaveGame(find, &data));
		XFindClose(find);
	}
	return count;
}



void __stdcall function_216e50(word const *display_name, long type, char *language, word *name);
void unicode_string_snprintf(word *buffer, long maximum_count, word const *format, ...);

struct s_save_entry_2164c0
{
    char directory[20];
    word name[17];
    byte unknown36[2];
    long type;
    byte unknown3c;
    char language;
    byte unknown3e[2];
};

struct s_save_list_2164c0
{
    long count;
    s_save_entry_2164c0 entries[4096];
};

PRIVATE inline bool append_save_entry_2164c0(s_save_list_2164c0 *list, s_save_entry_2164c0 const *entry)
{
    long count = list->count;
    bool result = count != 4096;
    if (result)
    {
        list->count = count + 1;
        list->entries[count] = *entry;
    }
    return result;
}

// @retail 0x2164c0
void function_2164c0(long memory_unit, s_save_list_2164c0 *list)
{
    (void)&list;
    char drive;
    if (function_1249f0(memory_unit, &drive))
    {
        char root[8] = "";
        if (function_1249f0(memory_unit, &drive))
            function_11c9c0(root, sizeof(root), "%c:\\", drive);
        else
            root[0] = 0;
        XGAME_FIND_DATA data;
        HANDLE find = XFindFirstSaveGame(root, &data);
        if (find != INVALID_HANDLE_VALUE)
        {
            do
            {
                s_save_entry_2164c0 entry;
                memset(&entry, 0, sizeof(entry));
                strncpy(entry.directory, data.szSaveGameDirectory, sizeof(entry.directory));
                entry.directory[19] = 0;
                long type;
                for (type = 0; type < 11; type++)
                {
                    char path[256];
                    path[0] = 0;
                    strncpy(path, entry.directory, sizeof(path));
                    path[255] = 0;
                    function_122810(path, saved_game_type_get_storage_name(type));
                    s_storage_file_reference file;
                    memset(&file, 0, sizeof(file));
                    file.signature = FILE_REFERENCE_SIGNATURE;
                    file.location = NONE;
                    if (file.flags & 1)
                    {
                        short length = (short)strlen(file.path);
                        while (length > 0 && file.path[length] != '\\') length--;
                        file.path[length] = 0;
                    }
                    if (*path)
                    {
                        size_t length = strlen(file.path);
                        char *end = file.path + length;
                        if (end != file.path && end[-1] != '\\')
                        {
                            *end++ = '\\';
                            *end = 0;
                            length++;
                        }
                        strncpy(end, path, sizeof(file.path) - length);
                        file.path[255] = 0;
                    }
                    file.flags |= 1;
                    if (function_1368f0((s_type_acf665 *)&file))
                        goto found;
                }
                {
                    word description[256];
                    unicode_string_snprintf(description, 256, L"unrecognized saved game file found by XFindNextSaveGame(): display name= '%s' path= '%hs'", data.szSaveGameName, data.wfd.cFileName);
                    char text[256];
                    word const *source = description;
                    char *destination = text;
                    long left = 256;
                    word character;
                    do
                    {
                        character = *source;
                        if (left == 1)
                            *destination = 0;
                        else if (character <= 0x7f)
                            *destination = (char)character;
                        else
                            *destination = '?';
                        source++;
                        destination++;
                        left--;
                    } while (character && left > 0);
                }
                goto next;
found:
                {
                    entry.type = type;
                    function_216e50(data.szSaveGameName, type, &entry.language, entry.name);
                    if (!append_save_entry_2164c0(list, &entry))
                        break;
                }
next:;
            } while (XFindNextSaveGame(find, &data));
            XFindClose(find);
        }
    }
}
