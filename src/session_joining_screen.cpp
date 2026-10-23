// @flags /O1 /arch:SSE /Gr
/* SESSION_JOINING_SCREEN.CPP: the screen shown while the game of a network
   session is being joined (vtable 0x45a420, screen 0xe7): the session's state
   and progress, with the music faded out in multiplayer */

#include "unknown_11c920.h"
#include "globals.h"
#include "screen_widgets.h"
#include "unknown_19b516.h"
#include "game_loop.h"

/* the music fades out while the screen shows */
long function_1480ff(long screen_id);
bool function_22f0ff(c_widget *widget);
long function_2197f0(real gain);
void __stdcall function_221980(char const *name, long value_bits, real time);
bool function_68290(void);
void __stdcall function_19a02d(long *string_handle, real *progress);

/* the joining screen (screen 0xe7) */
class c_session_joining_screen : public c_class_1473c9
{
public:
	c_session_joining_screen(long a, long b, word user_flags);
	~c_session_joining_screen();

	virtual void v3();
	virtual void v18(void *parameters);
	virtual screen_load_proc get_load_proc();
};

c_class_1473c9 *__stdcall session_joining_screen_load(s_screen_parameters *parameters);

// @retail 0x2521f2
screen_load_proc c_session_joining_screen::get_load_proc()
{
	return session_joining_screen_load;
}

/* opens the screen as its own (type bit 2) */
// @retail 0x2521f8
c_class_1473c9 *__stdcall session_joining_screen_load(s_screen_parameters *parameters)
{
	c_session_joining_screen *screen;

	parameters->type_bit2 = true;
	screen = new c_session_joining_screen(parameters->a, parameters->b, parameters->user_flags);
	screen->m6c = true;
	screen->function_147f6d(parameters);
	return screen;
}

/* fades the music out in multiplayer */
// @retail 0x252237
c_session_joining_screen::c_session_joining_screen(long a, long b, word user_flags) :
	c_class_1473c9(0xe7, a, b, user_flags)
{
	if (g_4e6948->state == _game_state_multiplayer)
	{
		struct { long gain_bits; real duration; } fade;
		fade.gain_bits = function_2197f0(0.0f);
		fade.duration = 1.0f;
		function_221980("", fade.gain_bits, fade.duration);
	}
}

/* fades the music back in */
// @retail 0x25229e
c_session_joining_screen::~c_session_joining_screen()
{
	if (g_4e6948->state == _game_state_multiplayer)
	{
		struct { long gain_bits; real duration; } fade;
		fade.gain_bits = function_2197f0(1.0f);
		fade.duration = 1.0f;
		function_221980("", fade.gain_bits, fade.duration);
	}
}

// @retail 0x252280 deleting c_session_joining_screen

/* builds the screen from its definition (its slot 18 is shared by the screens
   of 0x4549c0, 0x45a250, 0x45ac80 and 0x45bcd0) */
// @retail 0x2522d7
void c_session_joining_screen::v18(void *parameters)
{
	volatile long definition_index = function_1480ff(screen_id);
	s_screen_layout layout =
	{
		0,
		1,
		{
			{ 0, 0, 0, 0 }
		}
	};

	build(&layout);
	c_class_1a2c81::v1();
}

/* shows the search's state and progress, and leaves once it is over */
// @retail 0x252325
void c_session_joining_screen::v3()
{
	if (!function_22f0ff((c_widget *)this))
	{
		if (function_68290())
		{
			c_text_widget_45a5e0 *text = (c_text_widget_45a5e0 *)find_child(6, 1, false);
			c_class_2b01eb *join_progress = (c_class_2b01eb *)find_child(8, 0, false);
			long string_handle = 0;
			real progress = 0.0f;

			function_19a02d(&string_handle, &progress);
			if (text && string_handle)
			{
				text->function_253b1a(string_handle);
			}
			if (join_progress)
			{
				join_progress->value84 = progress;
			}
		}
		else
		{
			start_animation(3);
		}
	}
	c_class_1a2c81::v3();
}
