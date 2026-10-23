// @flags /O1 /arch:SSE /Gr
/* START_PROMPT_SCREEN.CPP: the "press start" screen that opens
   the main menu: a press of start (its button's handler) accepts a pending
   game invitation, or signs the user in */

#include "unknown_11c920.h"
#include <xtl.h>
#include <xonline.h>
#include <stddef.h>
#include "globals.h"
#include "screen_widgets.h"
#include "unknown_19b510.h"
#include "unknown_234c64.h"
#include "unknown_2312b4.h"

struct s_message;
void function_149f49(s_message *message, word a, dword *id, short b, long c, long d, long e);
void function_236299(long sound);
long function_1480ff(long screen_id);
s_screen_definition *function_22f871(c_class_1473c9 *screen);
struct s_bitmap_view
{
	byte unknown00[0x74];
};

struct s_bitmap_group_view
{
	byte unknown00[0x44];
	long bitmap_count;
	s_bitmap_view *bitmaps;
};
typedef char assert_bitmap_group_view_bitmaps[(offsetof(s_bitmap_group_view, bitmaps) == 0x48) ? 1 : -1];

D3DTexture *function_12360(s_bitmap_view *bitmap, real priority);

/* loads every bitmap of a bitmap tag (retail's copy of 0x23625d) */
static __forceinline void bitmap_tag_load(long tag_index)
{
	if (tag_index != NONE)
	{
		s_bitmap_group_view *group = (s_bitmap_group_view *)g_4e3b44[tag_index & 0xffff].bytes;
		long count = group->bitmap_count;

		for (long i = 0; i < count; i++)
		{
			function_12360(&group->bitmaps[i], 0.0f);
		}
	}
}
class c_widget;
bool function_22f0ff(c_widget *widget);
long player_slot_iterate_next(long value);
word function_1901fc(void);
bool online_get_accepted_game_invite(XONLINE_ACCEPTED_GAMEINVITE *invite);
long minimal_storage_size_in_blocks();
bool saved_game_storage_has_free_blocks(long blocks);
bool function_8d7c0(void);
void function_1905bf(long controller, bool flag);
bool __stdcall dialog_callback_dashboard_memory_for_minimum_storage(long controller);
void function_6cb60(void);
void function_1906b4(void);
void window_manager_reset_windows_except_channels_1_and_5(void);
c_class_1473c9 *__stdcall main_menu_open(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_252481(s_screen_parameters *parameters);

extern dword g_51ebec;
extern dword g_54d5b8;

/* what the name lookups take (unknown_2b2d7d.cpp) */
struct s_name_request
{
	long type;
	XONLINE_FRIEND field_xb3bdcf;
	byte unknown[0x78 - 4 - sizeof(XONLINE_FRIEND)];
};

void __stdcall function_148893(s_name_request *request, long flag);

/* a user's slot: whether the user is signed in */
struct s_player_slot_signed_in_view
{
	dword flags0 : 4;
	dword signed_in : 1;
	byte unknown004[0xc70 - 4];
};
typedef char assert_player_slot_signed_in_view_size[(sizeof(s_player_slot_signed_in_view) == 0xc70) ? 1 : -1];

class __single_inheritance c_press_start_screen;

/* the button's handler (vtable 0x45bdb0, folded with the list item
   handlers') */
class c_press_start_handler : public c_list_item_delegate
{
public:
	typedef void (c_press_start_screen::*method_t)(s_controller_reference **controller, long *item);

	c_press_start_handler(c_press_start_screen *owner, method_t method) :
		owner(owner),
		method(method)
	{
	}
	virtual void invoke(s_controller_reference **controller, long *item)
	{
		(owner->*method)(controller, item);
	}

	c_press_start_screen *owner;
	method_t method;
};

/* the press start screen (vtable 0x4587d0) */
class c_press_start_screen : public c_class_1473c9
{
public:
	c_press_start_screen(long a, long b, word user_flags);

	virtual void v3();
	virtual bool v10(s_widget_event *event);
	virtual void v18(void *parameters);
	virtual screen_load_proc get_load_proc();

	void handle_start(s_controller_reference **controller, long *item);

	c_class_19b8b1 button;
	c_press_start_handler handler;
};

c_class_1473c9 *__stdcall press_start_screen_open(s_screen_parameters *parameters);

/* the screen's load proc */
// @retail 0x22f118
screen_load_proc c_press_start_screen::get_load_proc()
{
	return press_start_screen_open;
}

/* loads the press start screen */
// @retail 0x22f11e
c_class_1473c9 *__stdcall press_start_screen_open(s_screen_parameters *parameters)
{
	c_press_start_screen *screen;

	parameters->type_bit2 = true;
	screen = new c_press_start_screen(parameters->a, parameters->b, parameters->user_flags);
	screen->m6c = true;
	screen->function_147f6d(parameters);
	return screen;
}

/* the screen holds one button, which the handler ties to handle_start */
// @retail 0x22f15d
c_press_start_screen::c_press_start_screen(long a, long b, word user_flags) :
	c_class_1473c9(9, a, b, user_flags),
	button(0, user_flags),
	handler(this, &c_press_start_screen::handle_start)
{
}

// @retail 0x22f1b2 deleting c_press_start_screen
// @retail 0x22f1ce destructor c_press_start_screen

/* events go to the screen's default handling */
// @retail 0x22f4c8
bool c_press_start_screen::v10(s_widget_event *event)
{
	return c_class_1473c9::v10(event);
}

/* builds the screen around its button */
// @retail 0x22f1f7
void c_press_start_screen::v18(void *parameters)
{
	c_class_1a2c81 *volatile widget = (c_class_1a2c81 *)function_1480ff(screen_id);
	s_screen_layout layout =
	{
		0,
		1,
		{
			{ 1, (c_class_1a2c81 **)&widget, 0, 0 }
		}
	};

	widget = &button;
	build(&layout);
	delegate_register(&button.handlers, &handler);
	c_class_1a2c81::v1();
	if (button.parent == this)
	{
		v7(&button);
	}
	function_6cb60();
	function_1906b4();
	g_51ebec = g_54d5b8;
	window_manager_reset_windows_except_channels_1_and_5();
}

/* a signed in user leaves for the main menu; the screen's bitmaps stay
   loaded */
// @retail 0x22f28c
void c_press_start_screen::v3()
{
	s_screen_definition *definition;
	long bitmap_index;

	if (!function_1473b6(&g_54d598.windows_3[4]) && !function_22f0ff((c_widget *)this))
	{
		long user_index = 0;

		do
		{
			if (TEST_FIELD_BIT(((s_player_slot_signed_in_view *)&g_54e8e0[user_index])->signed_in))
			{
				s_screen_parameters parameters;

				parameters.field_c = 0;
				function_149f49((s_message *)&parameters, 0, 0, 1 << user_index, 5, 4, (long)main_menu_open);
				parameters.load(&parameters);
				function_236299(5);
				break;
			}
			user_index = player_slot_iterate_next(user_index);
		} while (user_index != NONE);
	}
	definition = function_22f871(this);
	for (bitmap_index = 0; bitmap_index < definition->bitmap_count; bitmap_index++)
	{
		s_tag_reference volatile *bitmap = &definition->bitmaps[bitmap_index];

		bitmap_tag_load(bitmap->tag_index);
	}
	c_class_1a2c81::v3();
}

/* the invitation dialog's yes: opens the sign in screen (in its mode 4)
   for the users who are signed in */
// @retail 0x22f36f
bool __stdcall press_start_join_invite_confirmed(void *data)
{
	s_screen_parameters parameters;

	parameters.field_c = 0;
	function_149f49((s_message *)&parameters, 0, 0, function_1901fc(), 5, 4, (long)function_252481);
	parameters.load(&parameters);
	return true;
}

/* signs the user in (also the choice that skips the memory dialog) */
// @retail 0x22f3a1
bool __stdcall press_start_sign_in(long controller_index)
{
	function_1905bf(controller_index, function_8d7c0());
	return true;
}

/* a game invitation accepted in the last 15 minutes asks whether to join
   it; otherwise the first user signs in, if the drive has room for a profile */
// @retail 0x22f3b5
void c_press_start_screen::handle_start(s_controller_reference **controller, long *item)
{
#ifndef H2_NATIVE_PORT
	XONLINE_ACCEPTED_GAMEINVITE invite;

	if (online_get_accepted_game_invite(&invite))
	{
		unsigned __int64 now;

		GetSystemTimeAsFileTime((FILETIME *)&now);
		unsigned __int64 accepted = *(unsigned __int64 *)&invite.InviteAcceptTime;
		if (now > accepted && now - accepted < (unsigned __int64)XONLINE_ACCEPTED_GAMEINVITE_EXPIRATION_INTERVAL * 10000000)
		{
			s_name_request request;

			request.type = 2;
			request.field_xb3bdcf = invite.InvitingFriend;
			request.field_xb3bdcf.gameinviteTime = invite.InviteAcceptTime;
			function_148893(&request, 1);
			dialog_choice_show(3, 0x7d, 4, 1 << (*controller)->controller_index, (dialog_choice_callback)press_start_join_invite_confirmed, 0, 0);
			return;
		}
	}
#endif
	if (!TEST_FIELD_BIT(((s_player_slot_signed_in_view *)&g_54e8e0[0])->signed_in) &&
		!TEST_FIELD_BIT(((s_player_slot_signed_in_view *)&g_54e8e0[1])->signed_in) &&
		!TEST_FIELD_BIT(((s_player_slot_signed_in_view *)&g_54e8e0[2])->signed_in) &&
		!TEST_FIELD_BIT(((s_player_slot_signed_in_view *)&g_54e8e0[3])->signed_in))
	{
		if (saved_game_storage_has_free_blocks(minimal_storage_size_in_blocks()))
		{
			press_start_sign_in((*controller)->controller_index);
		}
		else
		{
			dialog_choice_show(3, 0x20, 4, 1 << (*controller)->controller_index, dialog_callback_dashboard_memory_for_minimum_storage, press_start_sign_in, 0);
		}
	}
}
