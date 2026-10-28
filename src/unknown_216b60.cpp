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

// score 20 (was 20): post-call name-address reuse still shortens the first branch;
// separate noinline name builder 20; validation-result lifetime 124; complete create predicate 50;
// aggregate path initializer 20; name initialized before path 20; success-scoped root 20;
// create rejection exit 34; formatting-result carry 22.
// score 20 (was 20): post-call name-address reload and shortened entry branch remain;
// short success 51; caller inline contract 20; drive-root noinline 20; API status local 20;
// creation-failure clear 71; validation noinline 20; conditional success 20; short/clear 57.
// formatted-name address reuse and the resulting short branch remain; ordinary
// type input 20; direct api status result 21; function scope drive root 20; role named buffer homes
// 20; explicit library path clear 58; byte status result 54; drive before status lifetime 20; query
// failure returns 63.
// formatted-name address reuse still shortens the branch.
// Long success result 52; initialized name/root arrays 67/64; path after
// name success 23; function scope buffers 35; drive root result 29;
// format failure single exit 20; nested validation 20.
// score 20 (was 20): after the call to 0x216da0, retail recomputes the name address with a lea
// where ours copies EDI. Path and name in one struct 58; (void)&name in 0x216da0 20; 0x216da0
// reading name through its address 39; noinline declarations of 0x216da0/0x217a80 20; 0x216da0
// result variable 20.
// The post-format name address is still reused and the first branch is short.
// Signed success byte 54; native status result 55; branch-owned result 61; failure-selected result
// 55; input validation carry 61; success/drive homes 20; initialized drive 20; direct success exit
// 62.
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

// score 8 (was 8): prefix address is reused in ESI after formatting instead of recomputed; existing
// copy helper 8; ordinary input parameters 8; guard owned prefix buffer 12; prefix value
// initialization 69; native string handle 8; out of line format declarations 8; available prefix
// buffer 21; lookup result function scope 8.
// Prefix lexical name 8; declaration after output clear 8; direct prefix clear 8;
// nested language guards 8; string-list local 8; result local 8;
// display/language input homes removed separately 8/8.
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

// score 5 (was 5): post-fill prefix address is reused in ESI instead of rematerialized.
// Prefix declared before the ordinal 5; function-scope index/language 5/5; early length declaration 5;
// prechecked append 91; force-inline append 5; ordinal/buffer lexical names 5/5; short language
// ordinal 84; signed byte language ordinal/result 46/75; short ordinal and byte language 85; byte
// ordinal and language 92; unsigned measured length 5; completed-name return 5; completed-name return
// with short ordinal and unsigned length 84; shared both/initial/suffix bounded copies 5/5/5; append
// result span 5; initialized prefix array 119; language before id query 13; named prefix comparison 5;
// unmatched prefix continue 5; incoming homes removed separately/all 5/5/5/5/5; immutable loop inputs 5;
// starting-language global per iteration 47; ordinary while traversal 111.
// 0x217a80 as a member of its 256-character buffer: this function MATCH, but 0x216da0 48 (its language
// parameter then shares esi, the zero constant takes ebp); same with swapped value parameters; buffer
// struct by pointer to a free function 5; 0x217a80 copying with unicode_string_copy 5.
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
