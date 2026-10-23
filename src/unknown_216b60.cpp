// @flags /O2 /Gr
/* UNKNOWN_216B60.CPP: saved game file types */

#include "unknown_11c920.h"
#include <string.h>
#if defined(H2_NATIVE_PORT)
#include <wchar.h>
#endif
#include <xtl.h>

extern void *g_51ea14;
extern long g_55c280;
bool function_1249f0(long memory_unit, char *drive_letter);
char *function_216400(long memory_unit, char *buffer, short capacity);
bool function_216da0(long type, wchar_t *name, wchar_t const *display_name, long language);

struct s_saved_game_drive
{
	word utility_drive : 1;
	word unused : 15;
};

extern s_saved_game_drive g_4e61c0;
extern long g_55c154;
#if defined(H2_NATIVE_PORT)
extern "C" int h2_native_profile_list(unsigned int *identifiers, unsigned int capacity);
extern "C" int h2_native_profile_read(unsigned int identifier, void *record,
                                      unsigned int size);
#endif
char *function_11c9c0(char *buffer, long maximum_count, char const *format, ...);
void unicode_string_snprintf(word *buffer, long maximum_count, word const *format, ...);

// score 9 (was 9): the call to XCreateSaveGame takes the full name from the
// register edi, retail recomputes its address with a lea, so retail's code is
// five bytes longer and ours has five more bytes of padding at the end
// @retail 0x215f40
bool __stdcall function_215f40(long type, byte *buffer)
{
	word *name = (word *)buffer;
	name[0] = 0;
	s_saved_game_drive drive = g_4e61c0;
	if (TEST_FIELD_BIT(drive.utility_drive))
	{
		char root[8] = "";
		char path[256] = "";
		char letter = TEST_FIELD_BIT(g_4e61c0.utility_drive) ? 'u' : 0;
		function_11c9c0(root, sizeof(root), "%c:\\", letter);
		long index = 0;
		do
		{
			unicode_string_snprintf(name, 128, L"Halo%04d", index + 1);
			word full_name[128];
			full_name[0] = 0;
			function_216da0(type, full_name, name, g_55c280);
			if (XCreateSaveGame(root, full_name, 3, 0, path, sizeof(path)) != 0)
				break;
			index++;
		}
		while (index < 4096);
		if (index == 4096)
		{
			g_55c154 = 1;
			name[0] = 0;
		}
	}
	return name[0] != 0;
}

// @retail 0x216120
bool function_216120(word *string, long type)
{
	(void)&type;
#if defined(H2_NATIVE_PORT)
	if (!string || !string[0] || type != 0)
		return false;
	unsigned int identifiers[4096];
	int count = h2_native_profile_list(identifiers, 4096);
	if (count < 0 || count > 4096)
		return false;
	for (int index = 0; index < count; ++index)
	{
		byte record[0x1e0];
		if (!h2_native_profile_read(identifiers[index], record, sizeof(record)))
			return false;
		word const *stored = (word const *)(record + 8);
		long character = 0;
		for (; character < 32; ++character)
		{
			word left = string[character];
			word right = stored[character];
			if (left >= 'A' && left <= 'Z')
				left += 'a' - 'A';
			if (right >= 'A' && right <= 'Z')
				right += 'a' - 'A';
			if (left != right)
				break;
			if (!left)
				return false;
		}
	}
	return true;
#else
	bool result = false;
	char drive;
	if (string && *string && g_51ea14 && function_1249f0(0, &drive))
	{
		char path[256] = "";
		word name[128];
		char root[8];
		name[0] = 0;
		if (function_216da0(type, name, string, g_55c280))
		{
			if (XCreateSaveGame(function_216400(0, root, 8), name, 3, 0, path, sizeof(path)) != 0)
				result = true;
		}
	}
	return result;
#endif
}

__declspec(noinline) long function_217480(long type);

/* the size of a saved game file of a type */
// @retail 0x217480
long function_217480(long type)
{
	if (type != 0)
	{
		return type > 9 ? 0x15cb8 : 0x130;
	}
	return 0x1e0;
}

extern long g_55c158;
word const *g_470054 = L": ";
long saved_game_type_get_name_id(long type);
bool unicode_string_list_has_string(long tag_index, long string_handle);
void function_217a80(long string_handle_2, long language, word *buffer);
word *unicode_string_append(word *destination, word const *source, long maximum_count);

// @retail 0x216da0
bool function_216da0(long type, wchar_t *name, wchar_t const *display_name, long language)
{
	(void)&display_name;
	(void)&language;
	word text[256];
	name[0] = 0;
	text[0] = 0;
	if (language != NONE && g_55c158 != NONE)
	{
		long string_handle = saved_game_type_get_name_id(type);
		if (unicode_string_list_has_string(g_55c158, string_handle))
		{
			function_217a80(string_handle, language, text);
			wcsncpy(name, text, 127);
			name[127] = 0;
			unicode_string_append(name, g_470054, 128);
			name[127] = 0;
		}
	}
	unicode_string_append(name, display_name, 128);
	name[127] = 0;
	return true;
}

extern long g_55c280;
dword function_217b70(word const *text);

// @retail 0x216e50
void __stdcall function_216e50(word const *display_name, long type, char *language, word *name)
{
	(void)&display_name;
	(void)&type;
	(void)&language;
	(void)&name;
	*language = -1;
	wcsncpy(name, display_name, 16);
	name[16] = 0;
	long string_handle = saved_game_type_get_name_id(type);
	long first_language = g_55c280;
	for (long i = 0; i < 9; i++)
	{
		long current_language = (first_language + i) % 9;
		word prefix[256];
		prefix[0] = 0;
		function_217a80(string_handle, current_language, prefix);
		unicode_string_append(prefix, g_470054, 256);
		prefix[255] = 0;
		long length = function_217b70(prefix);
		if (!wcsncmp(display_name, prefix, length))
		{
			*language = (char)current_language;
			wcsncpy(name, display_name + length, 16);
			name[16] = 0;
			break;
		}
	}
}
