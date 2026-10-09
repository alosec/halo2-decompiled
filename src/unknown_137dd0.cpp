// @flags /O2 /Gr
/* UNKNOWN_137DD0.CPP: the game options (0x1118 bytes, kept at +8 of the
   game globals g_4e6948): their defaults and starting a game with them */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_19d220.h"
#include <string.h>

struct s_game_options
{
	long state;
	byte unknown04;
	byte unknown05;
	short field_2_3;
	byte random_id[8];
	dword field_8_4;
	long unknown14;
	long unknown18;
	byte unknown1c[0x124 - 0x1c];
	bool unknown124;
	byte unknown125[0x128 - 0x125];
	byte unknown128;
	byte unknown129;
	short unknown12a;
	byte unknown12c[0x134 - 0x12c];
	s_game_variant variant;
	byte unknown_end[0x1118 - 0x134 - sizeof(s_game_variant)];
};

/* the game globals as this file sees them */
struct s_game_globals_137dd0
{
	byte unknown00[8];
	s_game_options options;
	byte unknown1120;
	bool flag1121;
	byte unknown1122[0x1128 - 0x1122];
	bool flag1128;
	byte unknown1129[0x1130 - 0x1129];
	long value1130;
	byte unknown1134[0x11fa - 0x1134];
	short value11fa;
};

bool function_19d650(s_game_variant *variant);
void function_07ad80(long count, byte *buffer);

#ifdef H2_NATIVE_PORT
extern byte g_510c49;
#else
byte g_510c49;
#endif
/* unknown_12b070.cpp */
extern short g_485ac0;

#define GAME_GLOBALS ((s_game_globals_137dd0 *)g_4e6948)

// @retail 0x137dd0
void function_137dd0(s_game_options const *options)
{
	s_game_options const *const *options_reference = &options;

	GAME_GLOBALS->options = **options_reference;
	GAME_GLOBALS->options.unknown124 = false;
	if (GAME_GLOBALS->options.state == 2 || g_4e6948->mode_180)
	{
		function_19d650(&GAME_GLOBALS->options.variant);
	}
	g_4e7408->unknown0 = GAME_GLOBALS->options.field_8_4;
	GAME_GLOBALS->flag1121 = false;
	GAME_GLOBALS->flag1128 = false;
	GAME_GLOBALS->value11fa = 0;
	GAME_GLOBALS->value1130 = 0;
}

// @retail 0x138110
void function_138110(s_game_options *options)
{
	long ticks;

	memset(options, 0, sizeof(*options));
	options->state = 1;
	options->unknown128 = g_510c49;
	options->unknown04 = 1;
	options->unknown05 = 0;
	ticks = g_485ac0 > 0 ? g_485ac0 : 60;
	options->unknown12a = 1;
	options->field_2_3 = (short)(ticks / 2);
	options->unknown14 = NONE;
	options->unknown18 = NONE;
	function_07ad80(sizeof(options->random_id), options->random_id);
	options->field_8_4 = 0x78a8;
}
