// @flags /O1 /Oi /Gr
/* PROFILE_MODE_SCREEN.CPP: the screen (id 0xa) whose list shows the four
   saved profiles a controller can sign in with; opened either to sign in
   locally or to sign in online as well (loop_allocator.h) */

#include "screen_widgets.h"
#include "loop_allocator.h"
#include <new>

#if defined(H2_NATIVE_PORT)
static_assert(sizeof(c_screen_with_menu) == 0x614, "profile screen base layout");
static_assert(sizeof(c_profile_mode_screen) == 0x11e0, "profile screen retail layout");
#endif

/* the list's item widgets */
// @retail 0x18f3ba
void *c_unknown_249fa3::get_item_data()
{
	return entries;
}

/* the list's profiles (always four) */
// @retail 0x18f3c1
void *c_unknown_249fa3::get_items(long *count)
{
	*count = 4;
	return profiles;
}

c_class_1473c9 *__stdcall profile_mode_screen_load(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall profile_mode_screen_load_online(s_screen_parameters *parameters);

/* tells the list whether choosing a profile signs it in online */
// @retail 0x18f4f6
void c_profile_mode_screen::v17()
{
	menu.field88 = sign_in_online;
}

/* the load proc that rebuilds this screen in its mode */
// @retail 0x18f503
screen_load_proc c_profile_mode_screen::get_load_proc()
{
	return sign_in_online ? profile_mode_screen_load_online : profile_mode_screen_load;
}

// @retail 0x18f533 destructor c_profile_mode_screen

// @retail 0x18f517 deleting c_profile_mode_screen

// @retail 0x18f3f0 destructor c_list_24a64d

// @retail 0x18f372
c_unknown_249fa3::c_unknown_249fa3(word user_flags) : c_list_24a64d(user_flags)
{
}

// @retail 0x18f4bb
c_profile_mode_screen::c_profile_mode_screen(long a, long b, word user_flags) :
 c_screen_with_menu(0xa, a, b, user_flags, &menu),
 menu(user_flags),
 sign_in_online(false)
{
}

/* opens the screen to sign a profile in locally */
// @retail 0x18f42d
c_class_1473c9 *__stdcall profile_mode_screen_load(s_screen_parameters *parameters)
{
	c_profile_mode_screen *screen;
	void *memory = function_1a47fd(sizeof(c_profile_mode_screen));
	if (memory)
	    screen = ::new (memory) c_profile_mode_screen(parameters->a, parameters->b, parameters->user_flags);
	else
	    screen = 0;
	screen->m6c = true;
	screen->sign_in_online = false;
	((c_class_1473c9 *)screen)->function_147f6d(parameters);
	return (c_class_1473c9 *)screen;
}

/* opens the screen to sign a profile in online as well */
// @retail 0x18f474
c_class_1473c9 *__stdcall profile_mode_screen_load_online(s_screen_parameters *parameters)
{
	c_profile_mode_screen *screen;
	void *memory = function_1a47fd(sizeof(c_profile_mode_screen));
	if (memory)
	    screen = ::new (memory) c_profile_mode_screen(parameters->a, parameters->b, parameters->user_flags);
	else
	    screen = 0;
	screen->m6c = true;
	screen->sign_in_online = true;
	((c_class_1473c9 *)screen)->function_147f6d(parameters);
	return (c_class_1473c9 *)screen;
}
