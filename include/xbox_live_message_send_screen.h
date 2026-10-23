/* XBOX_LIVE_MESSAGE_SEND_SCREEN.H: the screen that writes an Xbox Live
   message (text and an optional voice recording) and sends it, and the list
   of its three actions (src/xbox_live_message_send_screen.cpp) */

#ifndef XBOX_LIVE_MESSAGE_SEND_SCREEN_H
#define XBOX_LIVE_MESSAGE_SEND_SCREEN_H

#include "unknown_11c920.h"
#include <xtl.h>
#include <xonline.h>
#include "screen_widgets.h"
#include "online_message_entries.h"

/* the player the online screens act on, as function_14887e returns it:
   a user (type 1) or a friend (type 2) */
struct s_message_send_selection
{
	long type;
	union
	{
		XONLINE_USER user;
		XONLINE_FRIEND friend_;
	};
	byte unknown[0x78 - 4 - sizeof(XONLINE_USER)];
};
typedef char assert_message_send_selection_size[(sizeof(s_message_send_selection) == 0x78) ? 1 : -1];

/* the voice recording screen that 0x2c9012 loads: where it records */
struct s_voice_record_screen_view
{
	byte unknown000[0x61c];
	byte *data;
	long maximum_size;
	dword *size;
	dword *length;
};
typedef char assert_voice_record_screen_view_size[(sizeof(s_voice_record_screen_view) == 0x62c) ? 1 : -1];

/* the list's three items */
enum e_message_send_item
{
	_message_send_item_send = 0,
	_message_send_item_edit_text = 1,
	_message_send_item_record_voice = 2
};

/* "xbox live message send list" (vtable 0x45c068): send, edit the text,
   record a voice message */
class c_xbox_live_message_send_list : public c_class_1474e8
{
public:
	c_xbox_live_message_send_list(word user_flags);
	virtual ~c_xbox_live_message_send_list();

	/* makes the message block */
	virtual void v1();
	/* folded with c_squad_privacy_setting_list's */
	virtual long get_item_count() { return 3; }
	virtual void v20(c_class_1a2c81 *widget, long index);

	void handle_item(s_controller_reference **controller, long *item);
	void send(s_controller_reference **controller);
	void open_text_editor(s_controller_reference **controller);
	void set_default_text();

	c_class_14750b items[3];
	/* the message's type, 1 to 4 (finish_message_gamertag sends type 1,
	   finish_friend_gamertag type 2; type 4 is the default, and is not sent
	   without text or a voice recording) */
	long type;
	s_state_block *block;
	word text[0x100];
	/* the players to send to when the opener sets them (the clan and
	   friends options lists), else the selected player */
	XUID const *recipients;
	long recipient_count;
	long task_index;
	/* send to the gamertag of the settings (function_14887e) */
	bool to_gamertag;
	byte unknown41d[3];
	c_list_item_handler handler;
};

/* the screen (vtable 0x45bff8) */
class c_xbox_live_message_send_screen : public c_screen_with_menu
{
public:
	c_xbox_live_message_send_screen(long a, long b, word user_flags);

	/* shows the title, the text and the voice message's length */
	virtual void v3();
	/* ignores input once the message is sent */
	virtual bool v10(s_widget_event *event);
	virtual screen_load_proc get_load_proc();

	c_xbox_live_message_send_list list;
	bool sent;
	byte unknowna4d[3];
	/* who opened it, for the title: 1 the friends options list, 2 the clan
	   options list, 0 anything else */
	long opener;
};
typedef char assert_message_send_screen_list[(offsetof(c_xbox_live_message_send_screen, list) + offsetof(c_xbox_live_message_send_list, recipients) == 0xa24) ? 1 : -1];
typedef char assert_message_send_screen_opener[(offsetof(c_xbox_live_message_send_screen, opener) == 0xa50) ? 1 : -1];

c_class_1473c9 *xbox_live_message_send_screen_new(s_screen_parameters *parameters, long type, bool to_gamertag);
c_class_1473c9 *__stdcall xbox_live_message_send_screen_load_type1(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall xbox_live_message_send_screen_load_type2(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall xbox_live_message_send_screen_load_type1_to_gamertag(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall xbox_live_message_send_screen_load_type2_to_gamertag(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall xbox_live_message_send_screen_load_type3(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall xbox_live_message_send_screen_load_type4(s_screen_parameters *parameters);
void xbox_live_message_send_record_voice(s_controller_reference **controller);

#endif
