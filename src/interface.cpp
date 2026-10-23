// @flags /O1 /Oi /arch:SSE /Gr
/* INTERFACE.CPP: the interface game system (its entries sit in the game
   system table at 0x441500) and the interface tags of the globals tag */

#include "unknown_11c920.h"
#include "globals.h"
#include <string.h>

void function_19170f(void);
void game_state_initialize_165cc3(void);
void mission_objectives_init();
void function_19173e(void);
void function_13e8a0();
void function_165ce5(void);
void function_165db0(void);
void function_13edb0(long font, long style, long justification, dword flags, color4f const *color, color4f const *field_24);

/* unknown_29f5b0.cpp */
extern long *g_502248;

/* unknown_033a0b.cpp: the second global colour pointer */
struct s_33a0b_default;
extern s_33a0b_default *g_4686d4;

// @retail 0x13e5c7
void function_13e5c7(void)
{
	function_19170f();
	game_state_initialize_165cc3();
	mission_objectives_init();
}

// @retail 0x13e5d6
void interface_initialize_for_new_map(void)
{
	function_19173e();
	function_13e8a0();
	function_165ce5();
	memset(g_502248, 0, 5 * sizeof(long));
	function_13edb0(0, NONE, 0, 0, g_4686cc, (color4f const *)g_4686d4);
}

// @retail 0x13e610
void interface_dispose_from_old_map(void)
{
	function_165db0();
}

struct s_interface_tag_reference
{
	dword group_tag;
	long index;
};

struct s_interface_globals_view
{
	byte unknown00[0x110];
	long interface_tag_count;
	s_interface_tag_reference *interface_tags;
};

// @retail 0x13e615
long function_13e615(short arg_e332ed)
{
	s_interface_globals_view *globals = (s_interface_globals_view *)g_4e034c;
	s_interface_tag_reference *references = globals->interface_tag_count ? globals->interface_tags : NULL;

	return references[arg_e332ed].index;
}
