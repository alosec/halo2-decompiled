// @flags /O2 /Ob1 /Gr
/* UNKNOWN_1936E0.CPP: building the built-in game variants: a variant's
   names and descriptions from the user interface's string list, its
   place-to-points table and its settings (lane H) */

#include "unknown_11c920.h"
#include <xtl.h>
#include <wchar.h>
#include <string.h>
#include "globals.h"
#include "language.h"

#define PIN(x, lo, hi) ((x) < (lo) ? (lo) : (x) > (hi) ? (hi) : (x))

/* the settings of a variant of type 1 or 2: the places that score */
struct s_variant_place_settings
{
	long first;
	long last;
};

/* the settings of a variant of type 3: the teams that score */
struct s_variant_team_settings
{
	long team_size;
	long first_team;
	long last_team;
	bool flag_c;
	byte unknown0d[3];
};

/* the settings of a variant of type 4 */
struct s_variant_type4_settings
{
	long team_size;
	long first_team;
	long last_team;
	long field_c;
	long field_10;
	long field_14;
	bool flag_18;
	byte unknown19[3];
	long field_1c;
	long field_20;
	long field_24;
};

/* the settings of a variant of type 5 */
struct s_variant_type5_settings
{
	long team_size;
	long first_team;
	long last_team;
	long field_c;
};

/* a game variant (0x614 bytes); the checks are in unknown_193560.cpp */
struct s_built_variant
{
	long type;
	long field_4;
	dword field_8;
	char names[9][0x10];
	char descriptions[9][0x80];
	char field_51c[128];
	long field_59c;
	long field_5a0;
	long points[16];
	byte unknown5e4[8];
	union
	{
		s_variant_place_settings place;
		s_variant_team_settings team;
		s_variant_type4_settings type4;
		s_variant_type5_settings type5;
	} settings;
};

/* the shared user interface globals: the string list of the variants'
   names at +0x1c */
struct s_variant_strings_view
{
	byte unknown00[0x1c];
	long string_list_index;
};

/* a language's string table and a string list's range of its strings
   (unknown_1a0120.cpp) */
struct s_string_reference;

struct s_string_table
{
	s_string_reference *references;
	char *data;
	long count;
	long data_size;
	long references_offset;
	long data_offset;
	bool loaded;
	byte unknown19[3];
};

struct s_variant_string_tables_view
{
	byte unknown000[0x188];
	s_string_table tables[1];
};

struct s_variant_language_range
{
	short first;
	short count;
};

struct s_variant_string_list_view
{
	byte unknown00[0x10];
	s_variant_language_range languages[1];
};

struct s_user_interface_shared_globals *ui_mode_definition_get(void);
long function_11ca80(long value);
const char *string_table_find(s_string_table *table, long first, long count, long string_handle);
void utf8_string_to_utf16_string(const char *source, word *destination, long destination_count);
void utf16_string_to_utf8_string(const word *source, char *destination, long destination_size);

/* a string of a table, as 0x1a0350, which retail inlines here */
__forceinline void variant_string_table_get(s_string_table *table, long first, long count, long string_handle, word *buffer)
{
	if (table->loaded)
	{
		word string[0x100];

		utf8_string_to_utf16_string(string_table_find(table, first, count, string_handle), string, 0x100);
		wcsncpy((wchar_t *)buffer, (wchar_t *)string, 0xff);
		buffer[0xff] = 0;
	}
}

/* a string of a string list in the current language: 0x1a0180 without
   its test for NONE, which the caller makes */
__forceinline void variant_string_get(long tag_index, long string_handle, word *buffer)
{
	get_current_language();
	s_variant_string_list_view const *list = (s_variant_string_list_view *)g_4e3b44[tag_index & 0xffff].bytes;
	s_variant_string_tables_view *tables = (s_variant_string_tables_view *)g_4e034c;

	variant_string_table_get(&tables->tables[g_47ff38], list->languages[g_47ff38].first,
		list->languages[g_47ff38].count, string_handle, buffer);
}

/* fills a variant: its type and settings, its nine names and descriptions
   and its tables. Retail keeps field_8 on the stack: reading it through its
   address keeps it there */
// score 30 (was 30): Frame 0x408 and language/string homes agree; initial setup order and point-copy homes remain.
// Earlier: Frame 0x40c versus 0x408; tag spill and language/range/cursor homes remain.
// String zero and counter allocation also differ. Shared output cursor 221; native array cursors 229;
// Frame 0x40c versus 0x408 and tag/language/cursor homes remain; intermediate ownership does not remove the spill.
// Frame 0x40c versus 0x408; language/list registers and cursor homes remain.
// Earlier: Frame 0x40c versus 0x408; tag/language and string cursor homes remain.
// Frame 0x40c versus 0x408, tag/language and string cursor/count homes remain.
// Earlier 194 (was 194): Frame 0x40c versus 0x408; tag/language and string cursor homes remain.
// Frame 0x40c versus 0x408 and tag/language/string-cursor homes remain.
// Earlier 194 (was 229): frame 0x40c versus 0x408; string zero, cursor and
// frame 0x40c versus 0x408 and tag/language/string-cursor homes remain.
// Earlier: frame 0x40c versus 0x408; string zero, cursor and
// counter allocation remain. Shared output cursor 221; native array cursors 229;
// independent string counters 221; direct countdown points copy 226;
// shared cursor, independent counters and countdown copy 194.
// Separate increasing-counter cursors 194; short counters 194; native array
// cursors with countdown points 194; word table index 219 (frame 0x408,
// variant EBP); existing shared lookup 327.
// Separate points input cursor 228; direct string countdowns 194; both 228;
// independent countdown output cursors 228; shared remaining-string counter 228.
// Simultaneous output cursors 226; native string-array end bounds 263; shared increasing counter 194; score-array cursor 210; points-array end bound 191; shared counter/end bound 191; simultaneous cursors/end bound 222.
// Frame 0x40c versus 0x408 and language, tag and string-cursor homes remain.
// Earlier: Frame 0x40c versus 0x408 and language, tag and string-cursor homes remain.
// Earlier 194 (was 194): Frame 0x40c versus 0x408 and language, tag and string-cursor homes remain.
// Tried string workspace scope 211; masked list index 217; byte string counters 194; independent
// score counters 194; score upper clamps 226; unsigned score counter 194.
// Round 109: Complete point memcpy 217; role-named buffer/counters 194; whole wide-buffer zero 248 (three sibling losses); direct list field 212; independent named cursors 194.
// Explicit language last/first 194/194; explicit-language ordinary inline 433;
// original lookup/table/both ordinary inline 481/194/481; readonly language inputs 194;
// native wide workspace 194. Original lookup helpers, contracts and body restored.
// Inner string workspace at helper entry 194; file-local lookup helpers 194; string-first helper 194;
// native short range inputs 194; short ranges with file-local helpers 194 (fast); all restored.
// Round 113: Intermediate wide-buffer entry scope 194; unloaded-table early return 194; table pointer before list 194; list receiver before language query 202; existing table helper output-first 194.
// Round 123: Initializer-owned scratch 194; lookup-owned scratch 194; valid-list scratch 194; initializer scratch/native row cursors 194; initializer scratch/destination endpoint 191 (three-point gain dropped).
// retained list receiver 235; retained table root 223; both receivers 235; source-end copy 200;
// indexed source with destination cursor 221; both arrays indexed 221; list receiver/source end 241;
// table root/source end 215.
// List-index/language/list/table-root lexical homes separately and together 194 each;
// receiver declaration at producer 194; independent point count 194; short score ordinal 241.
// Round 201: Outer lookup output-first 194; existing bounded copy 194; unsigned string ordinals 194; entry point-destination cursor 223; intermediate buffer lexical order 194.
// Round 210: Native wide-array helper outputs 194; signed-byte string ordinals 194; direct table
// root 194; native table-reference input 194; unsigned language ordinal 194. Extra tag spill and
// language/cursor homes remain.
// Native range pointer/reference 172/172; table-helper range pointer/reference/value
// after table 172/172/217, before table 220/220/220. Keep the genuine
// twice-used range pointer; 791/782 bytes, original arithmetic and call values.
// Outer/inner/both actual lookup output results 172/172/172; native range value 217;
// outer tag-output-string/string-output-tag/output-string-tag orders 172 each;
// range value with both output results 217. Complete original helpers restored.
// Native table member long scalars/range pointer/range reference/short scalars 172 each;
// the same four receiver contracts with native fixed-array output 172 each. Original helpers restored.
// Round 207: uniform language-global reads 191; with native list field reads 36; with independent
// description cursor 30. On that graph: points input cursor 141; independent description plus input
// cursor 141; masked list ordinal 189; masked ordinal/input cursor 238; all three 238;
// use-stepped point cursors 30; directly indexed point arrays 218. Plain and complete-image checked.
// Round 210: entry point-input/output/both cursors 239/250/253; independent copy count with
// entry input/output/both 239/250/253; signed-short copy count 30; entry input/unsigned count 239.
// Round 213: point source before terminal fields/between fields/final score range/all score ranges
// 142/142/167/192, each with shared or independent countdown; full-width masked ordinal 189,
// with destination end 192, with late source cursor 238. Original body restored.
// Round 136: defining-unit search/conversion/both inline 271,172,271;
// the same with separate noinline 172,172,172;
// outlined inline search with outlined conversion/both noinline 172,172.
// List-index helper word/masked, word/direct, short/masked, unsigned/masked 51/51/51/30;
// word/short index local 212/212; word index and helper 212; short index and helper 212.
// Round 31 (lane 12): retail loads the point source into eax before the two field stores; ours keeps it
// in its stack home (with or without the field_8 read). Plain field_8 read 220; indexed destination with
// stepped source 30 (same code); stepped destination with indexed source 219 (frame 0x404); string loops
// on the shared counter 30; name third in the parameters, as the callers' loads order it, 30 (callers
// still match), the same with the plain field_8 read 220 (three callers lost).
// @retail 0x1936e0
void function_1936e0(long type, s_built_variant *variant, dword field_8, long description_string, long field_5a0, long const *points, long field_59c, long name_string)
{
	word text[0x100];
	s_variant_strings_view *strings;
	long i;
	dword const *field_8_reference = &field_8;

	text[0] = 0;
	variant->type = type;
	variant->field_4 = 0;
	variant->field_8 = *field_8_reference;
	strings = (s_variant_strings_view *)ui_mode_definition_get();
	if (strings && strings->string_list_index != NONE)
	{
		long string_list_index = strings->string_list_index;

		char *output = variant->names[0];

		for (long name_index = 0; name_index < 9; ++name_index)
		{
			variant_string_get(string_list_index, name_string, text);
			utf16_string_to_utf8_string(text, output, sizeof(variant->names[0]));
			output += sizeof(variant->names[0]);
		}
		char *description_output = variant->descriptions[0];
		for (long description_index = 0; description_index < 9; ++description_index)
		{
			variant_string_get(string_list_index, description_string, text);
			utf16_string_to_utf8_string(text, description_output, sizeof(variant->descriptions[0]));
			description_output += sizeof(variant->descriptions[0]);
		}
	}
	for (i = 0; i <= 5; i++)
		variant->field_51c[i] = PIN(i + 5, i, 127);
	for (i = 6; i <= 15; i++)
		variant->field_51c[i] = PIN(i + 6, i, 127);
	for (i = 16; i <= 25; i++)
		variant->field_51c[i] = PIN(i + 7, i, 127);
	for (i = 26; i <= 30; i++)
		variant->field_51c[i] = PIN(i + 8, i, 127);
	for (i = 31; i <= 35; i++)
		variant->field_51c[i] = PIN(i + 9, i, 127);
	for (i = 36; i <= 127; i++)
		variant->field_51c[i] = PIN(i + 10, i, 127);
	variant->field_59c = field_59c;
	variant->field_5a0 = field_5a0;
	long *point_output = variant->points;
	for (i = 16; i > 0; --i)
	{
		*point_output = *points;
		++point_output;
		++points;
	}
}

void matchmaking_hopper_build_player_start_delays(long scale, long first, long last, long *points);
void matchmaking_hopper_build_team_start_delays(long *points, long team_size, long last_team, long first_team, long scale);

/* fills a variant of type 4 */
// @retail 0x193ac0
void function_193ac0(s_built_variant *variant, long name_string, long description_string, dword field_8, long scale, s_variant_type4_settings const *settings, long field_59c)
{
	long points[16];

	matchmaking_hopper_build_team_start_delays(points, settings->team_size, settings->last_team, settings->first_team, scale);
	function_1936e0(4, variant, field_8, description_string, scale, points, field_59c, name_string);
	variant->settings.type4 = *settings;
}

/* fills a variant of type 3 */
// @retail 0x193b30
void function_193b30(s_built_variant *variant, long name_string, long description_string, dword field_8, long scale, s_variant_team_settings const *settings, long field_59c)
{
	long points[16];

	matchmaking_hopper_build_team_start_delays(points, settings->team_size, settings->last_team, settings->first_team, scale);
	function_1936e0(3, variant, field_8, description_string, scale, points, field_59c, name_string);
	variant->settings.team = *settings;
}

/* fills a variant of type 1, or of type 2 when flag is set */
// score 34 (was 34): scale still enters EBX rather than EAX; typed mode enum 34;
// entry enum value 34; enum success update 33; complete enum outcomes 34; entry enum outcomes 32;
// enum member publication 34; enum last member first 34; enum complete settings value 13 (one-use copy rejected).
// mode production and settings-copy schedule still differ; 98/92 bytes. (held: the 23 wrote the type as 2 >> !flag, an arithmetic stand-in for the conditional, like round M139's 2 - !flag)
// Left/right shift and XOR complement at call 33/23/32; same values produced before delay 33/23/32;
// full delay-workspace initialization 66, with XOR mode 68. Right shift retained after plain whole-image check.
// scale now enters EAX and is saved in EBX as in retail; (held: the 24 wrote the type as 2 - !flag, six bytes long, and moved the unmatched caller 0x193c70 from 203 to 208)
// mode selection and settings-copy scheduling still differ (98/92 bytes).
// Fast: settings memcpy 34; memmove 15 (extra call, reverted); settings before
// initializer 34, before delays 34; inline 34; noinline 34; Boolean mode 37;
// disabled mode 24; disabled conditional 34; decrement mode 33; increment mode 33;
// bool disabled part 34; integer disabled part 24; byte option 34; signed byte option 34.
// scale arrives in EBX rather than EAX saved to EBX;
// inline/noinline declarations 34; settings memcpy/field stores 34;
// numeric type after/before delays 37; delay-table rename/scope 34.
// @retail 0x193ba0
void function_193ba0(long scale, bool flag, s_built_variant *variant, long name_string, long description_string, dword field_8, s_variant_place_settings const *settings, long field_59c)
{
	long points[16];

	matchmaking_hopper_build_player_start_delays(scale, settings->first, settings->last, points);
	function_1936e0(flag ? 2 : 1, variant, field_8, description_string, scale, points, field_59c, name_string);
	variant->settings.place = *settings;
}

/* fills a variant of type 5 */
// @retail 0x193c00
void function_193c00(s_built_variant *variant, long name_string, long description_string, dword field_8, s_variant_type5_settings const *settings, long field_59c)
{
	long points[16];

	matchmaking_hopper_build_team_start_delays(points, settings->team_size, settings->last_team, settings->first_team, 0);
	function_1936e0(5, variant, field_8, description_string, 0, points, field_59c, name_string);
	variant->settings.type5 = *settings;
}

/* fills one of the seven built-in variants */
// score 186 (was 186): frame 0x38 agrees; scale builder EBX/EAX, shifted settings homes and constant pushes remain.
// Frame 0x38 agrees; scale builder EBX/EAX, settings homes and constant pushes remain.
// Frame 0x38 agrees; scale builder EBX/EAX, shifted settings homes and constant pushes remain.
// Earlier 186 (was 186): Frame 0x38 agrees; scale callee EBX/EAX and constant pushes remain.
// Frame 0x38 agrees; scale callee EBX/EAX and constant pushes still differ.
// frame 0x38 agrees; scale callee EBX/EAX and constant pushes still differ.
// Frame 0x38 agrees; scale callee EBX/EAX and constant pushes remain.
// Earlier: Frame 0x38 agrees; scale callee EBX/EAX and constant pushes still differ.
// Frame 0x38 agrees; scale callee EBX/EAX and constant-argument pushes still differ.
// Earlier 199 (was 203): frame 0x38 agrees; callee scale in EBX versus EAX
// adds a save and shifts settings homes; constant argument pushes also differ.
// Native zero initialization 199; complete aggregate values 307; grouped
// place cases 204; case returns 203; zero initialization and grouping 206.
// Extra EBX save and settings homes remain; native leading
// Earlier: extra EBX save and settings homes remain; native leading
// Earlier at 186 (was 199): extra EBX save and settings homes remain; native leading
// defaults for type-four/type-five/both/team/type-four-team/type-five-team/all
// 229/216/241/204/233/224/249; longer type-four prefix 258.
// Native settings memset 203; Boolean fields first 200; shared type-four
// workspace 206; equal-place assignment chain 199; team case first 218.
// Round 109: Type-four padding-only clear 272; team padding-only clear 204; complete type-five assignment 228; all three forms 301; distinct native case settings names 199.
// Round 111: Type-five/team/place native scopes 189/196/199; reverse type-four stores 204; type-four cases first 220; combined native scopes 186 kept; type-five value initialization alone/combined 193/190; combined lexical home 186; combined reversed stores 191.
// Complete type-four team/flag, output-first, equal-first/equal-last schedules
// on first case 186/193/192/193; on both cases 187/193/198/199. All restored.
// Shared place/type-four/type-five case bodies 224/245/227; paired families 250/236/269;
// all shared case bodies 268; all shared with native type-four zero value 249.
// Builder scale/type-four/type-five/team inline-noinline declarations 186 each; place workspace beside retained pair 186; type-four workspace beside pair 203; type-five-first/separate-place-first cases 213/192.
// Round 113: Shared type-five overwritten clear omitted 224; equal team-field chain 185 (one-point gain dropped); type-four memset with shared workspaces 203; shared declarations reversed 186; complete shared-record entry initialization 237.
// Type-four native value reset 216; first-case shared three/four 193/193; second-case shared five 192; all
// equal type-four values chained 199; type-five range/full reversed stores 186/186; descending cases 228.
// Scale EBX/EAX and constant pushes remain.
// Native output zero value 214; with native type-four/type-five/both/team/type-four+team/type-
// five+team/all resets 261/226/276/220/267/232/282. Temporary output frame grows to 0x64c;
// original direct clear and scale contract remain.
// Round 148: Complementary place/type-four/type-five pair reversal 188/190/188; place plus type-four/type-five
// 196/190; type-four plus type-five 192; all pairs 198; alternate presets before base presets 206. Frame 0x38
// agrees; scale EBX/EAX and constant pushes remain.
// Round 317: Mixed complete-case orders 1023456 196; 0142356 197; 0124356 200; 0123645 188; 0126345 198; 0412356 205; 4012365 214; 0214356 206.
// Round 199: Static native type-four/type-five/both/team/type-four-team/type-five-team/all preset
// objects 430/294/525/232/474/335/569; all with static place presets 610 (fast). All restored.
// Equal type-five team/mode field chain in both directions 186/186; with implicit
// first-preset zero 180/180, with equal team pair 185/185, with both 180/180 fast.
// Six-point reduction remains below retention gate; original complete presets restored.
// Round 163: Complete immutable first/second/both type-four/team/first+team/
// second+team/all settings, then all with implicit type-five zero default
// 235/228/259/190/239/237/263/267. All regress; original native workspaces, clears and
// builder arguments restored.
// Round 324: memset destination result 186; implicit type-four zero defaults 211; destination/type-four 211;
// implicit type-five zero default 180 (below gate); destination/type-five 180; both default kinds 215;
// destination/both 215; destination/defaults/native place values 215.
// Round 210: implicit type-five zero with complete equal-value type-four producers, first/second/both
// 187/186/193; each with equal-place chain 187/186/193; reversed chains 193; native place values 193.
// Round 213: direct destination type-four/type-five/both/team/type-four-team/type-five-team/all
// settings 245/212/267/200/259/231/271; all with destination place settings 300. Original scratch restored.
// Round 167: Independent first/second/both type-four workspaces 202/198/203; separate
// place pair 186; first/second/both with separate place pair 202/198/203; all
// independent workspaces with native type-four value resets 216. All restored.
// @retail 0x193c70
void function_193c70(s_built_variant *variant, long index)
{
	s_variant_type5_settings type5_settings;
	s_variant_team_settings team_settings;
	memset(variant, 0, sizeof(*variant));
	switch (index)
	{
	case 0:
	{
		s_variant_place_settings settings;

		settings.first = 4;
		settings.last = 8;
		function_193ba0(0x7530, true, variant, 0x90007a4, 0xf0007a5, 1, &settings, 0x249f0);
		break;
	}
	case 1:
	{
		s_variant_type4_settings settings = {0};
		settings.team_size = 2;
		settings.first_team = 3;
		settings.last_team = 4;
		settings.field_c = 0;
		settings.field_10 = 1;
		settings.field_14 = 4;
		settings.flag_18 = true;
		settings.field_1c = 3;
		settings.field_20 = 4;
		settings.field_24 = 0;
		function_193ac0(variant, 0x100007a6, 0x160007a7, 2, 0x7530, &settings, 0x249f0);
		break;
	}
	case 2:
	{
		s_variant_place_settings settings;

		settings.first = 2;
		settings.last = 2;
		function_193ba0(0x7530, true, variant, 0x120007a8, 0x180007a9, 3, &settings, 0x1d4c0);
		break;
	}
	case 3:
	{
		s_variant_type4_settings settings = {0};
		settings.team_size = 2;
		settings.first_team = 5;
		settings.last_team = 8;
		settings.field_c = 1;
		settings.field_10 = 1;
		settings.field_14 = 5;
		settings.flag_18 = true;
		settings.field_1c = 3;
		settings.field_20 = 5;
		settings.field_24 = 2;
		function_193ac0(variant, 0xe0007aa, 0x140007ab, 4, 0xafc8, &settings, 0x493e0);
		break;
	}
	case 4:
	{
		memset(&type5_settings, 0, sizeof(type5_settings));
		type5_settings.team_size = 2;
		type5_settings.first_team = 3;
		type5_settings.last_team = 4;
		type5_settings.field_c = 0;
		function_193c00(variant, 0x1a0007ac, 0x200007ad, 0x19, &type5_settings, 0x1d4c0);
		break;
	}
	case 5:
	{
		memset(&type5_settings, 0, sizeof(type5_settings));
		type5_settings.team_size = 2;
		type5_settings.first_team = 6;
		type5_settings.last_team = 8;
		type5_settings.field_c = 2;
		function_193c00(variant, 0x180007ae, 0x1e0007af, 0x1a, &type5_settings, 0x1d4c0);
		break;
	}
	case 6:
	{
		memset(&team_settings, 0, sizeof(team_settings));
		team_settings.team_size = 2;
		team_settings.first_team = 2;
		team_settings.last_team = 4;
		team_settings.flag_c = true;
		function_193b30(variant, 0x190007b0, 0x1f0007b1, 5, 0x7530, &team_settings, 0x249f0);
		break;
	}
	}
}
