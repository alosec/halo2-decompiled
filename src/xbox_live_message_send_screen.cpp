#include <string.h>
#include "unknown_11c920.h"
#include <xtl.h>
#include <xonline.h>
#include "screen_widgets.h"
#include "unknown_19b510.h"
#include "unknown_19b516.h"
#include "unknown_234c64.h"
#include "online_message_entries.h"
#include "xbox_live_message_send_screen.h"

// @flags /O1 /Oi /Gr

/* XBOX_LIVE_MESSAGE_SEND_SCREEN.CPP: the screen that writes an Xbox Live
   message (text and voice) and sends it to the chosen players */

struct s_message;
void function_149f49(s_message *message, word a, dword *id, short b, long c, long d, long e);
void function_14887e(s_screen_settings_54dc6c *settings);
struct s_window_manager_text;
const char *function_148956(s_window_manager_text *text);
void function_236299(long sound);
void function_238c21(long controller, long type, word *name, long maximum_count);
void unicode_string_copy(word *destination, const word *source, long maximum_count);
void function_22d2ee(word *string, long maximum_length);
bool function_6c7e0();
bool function_1906da(long index);
bool voice_data_is_wave(const long *data, long size);
bool voice_port_can_talk(long port);
long online_task_poll(long task_index);
void function_08eeb0(s_state_block *block);
void online_message_block_set_text(s_state_block *block, const wchar_t *text);
void online_message_block_set_values(s_state_block *block, long value210, long value214, long value20c);
void function_08fa30(s_state_block *block);
long function_08eff0(s_state_block *block, long controller_index, XUID const *recipients, long recipient_count);
long function_08ef90(s_state_block *block, long controller_index, const char *gamertag);
c_class_1473c9 *__stdcall function_2c9012(s_screen_parameters *parameters);

/* the voice message being recorded: its size, data and length (the voice
   recording screen fills them in) */
dword g_voice_message_size;
byte g_voice_message_data[0x499a];
dword g_voice_message_length;

/* makes the screen for a message of this type */
// @retail 0x2b8045
c_class_1473c9 *xbox_live_message_send_screen_new(s_screen_parameters *parameters, long type, bool to_gamertag)
{
	c_xbox_live_message_send_screen *screen = new c_xbox_live_message_send_screen(parameters->a, parameters->b, parameters->user_flags);

	if (screen)
	{
		screen->list.type = type;
		screen->m6c = true;
		screen->list.to_gamertag = to_gamertag;
		screen->function_147f6d(parameters);
	}
	return screen;
}

/* the screen's load procedures, one per type and recipient */
// @retail 0x2b8099
c_class_1473c9 *__stdcall xbox_live_message_send_screen_load_type1(s_screen_parameters *parameters)
{
	return xbox_live_message_send_screen_new(parameters, 1, false);
}

// @retail 0x2b80a9
c_class_1473c9 *__stdcall xbox_live_message_send_screen_load_type2(s_screen_parameters *parameters)
{
	return xbox_live_message_send_screen_new(parameters, 2, false);
}

// @retail 0x2b80b9
c_class_1473c9 *__stdcall xbox_live_message_send_screen_load_type1_to_gamertag(s_screen_parameters *parameters)
{
	return xbox_live_message_send_screen_new(parameters, 1, true);
}

// @retail 0x2b80c9
c_class_1473c9 *__stdcall xbox_live_message_send_screen_load_type2_to_gamertag(s_screen_parameters *parameters)
{
	return xbox_live_message_send_screen_new(parameters, 2, true);
}

// @retail 0x2b80d9
c_class_1473c9 *__stdcall xbox_live_message_send_screen_load_type3(s_screen_parameters *parameters)
{
	return xbox_live_message_send_screen_new(parameters, 3, false);
}

// @retail 0x2b80e9
c_class_1473c9 *__stdcall xbox_live_message_send_screen_load_type4(s_screen_parameters *parameters)
{
	return xbox_live_message_send_screen_new(parameters, 4, false);
}

/* the load procedure that makes this screen again */
// @retail 0x2b80f9
screen_load_proc c_xbox_live_message_send_screen::get_load_proc()
{
	switch (list.type)
	{
	case 1:
		return list.to_gamertag ? xbox_live_message_send_screen_load_type1_to_gamertag : xbox_live_message_send_screen_load_type1;
	case 2:
		if (list.to_gamertag)
		{
			return xbox_live_message_send_screen_load_type2_to_gamertag;
		}
		else
		{
			return xbox_live_message_send_screen_load_type2;
		}
	case 3:
		return xbox_live_message_send_screen_load_type3;
	case 4:
		return xbox_live_message_send_screen_load_type4;
	}
	return 0;
}

/* the screen, with the list as its menu */
// @retail 0x2b8143
c_xbox_live_message_send_screen::c_xbox_live_message_send_screen(long a, long b, word user_flags) :
	c_screen_with_menu(0xd7, a, b, user_flags, &list),
	list(user_flags)
{
	opener = 0;
	sent = false;
}

// @retail 0x2b8186 deleting c_xbox_live_message_send_screen
// @retail 0x2b81a4 destructor c_xbox_live_message_send_screen

// @retail 0x2b81b9
bool c_xbox_live_message_send_screen::v10(s_widget_event *event)
{
	if (sent)
		return sent;
	return c_class_1473c9::v10(event);
}

// @retail 0x2b81cb
void c_xbox_live_message_send_screen::v3()
{
	c_class_1a2c81 *text = find_child(6, 2, false);
	c_class_2b01eb *meter = (c_class_2b01eb *)find_child(8, 6, false);
	c_text_widget_45a5e0 *length_text = (c_text_widget_45a5e0 *)find_child(6, 4, false);

	switch (list.type)
	{
	case 1:
		((c_text_widget_45a5e0 *)&title)->function_253b1a(0x1d0002cc);
		break;
	case 2:
		((c_text_widget_45a5e0 *)&title)->function_253b1a(0x1b0002d1);
		break;
	case 3:
		if (opener == 1)
			((c_text_widget_45a5e0 *)&title)->function_253b1a(0x260002cf);
		else if (opener == 2)
			((c_text_widget_45a5e0 *)&title)->function_253b1a(0x280002d0);
		else
			((c_text_widget_45a5e0 *)&title)->function_253b1a(0x1b0002ce);
		break;
	default:
		if (opener == 1)
			((c_text_widget_45a5e0 *)&title)->function_253b1a(0x1f0002d6);
		else if (opener == 2)
			((c_text_widget_45a5e0 *)&title)->function_253b1a(0x210002d7);
		else
			((c_text_widget_45a5e0 *)&title)->function_253b1a(0x140002d5);
		break;
	}
	if (text)
	{
		text->function_22f52e()->set_text(list.text);
	}
	if (meter)
	{
		meter->value84 = (real)g_voice_message_length * 6.6666667e-5f;
	}
	if (length_text)
	{
		if (g_voice_message_length > 0)
			length_text->function_253b1a(0x120002e1);
		else
			length_text->function_253b1a(0x110002e2);
	}
	c_class_1a2c81::v3();
	if (sent && !ANIMATION_FLAG(animation, 1))
	{
		if (list.task_index != NONE)
		{
			switch (online_task_poll(list.task_index))
			{
			case 0:
				return;
			case 1:
				return;
			default:
				break;
			}
		}
		start_animation(3);
	}
}

/* adds the send item, and the text and voice items when the user is online */
// @retail 0x2b7b48
c_xbox_live_message_send_list::c_xbox_live_message_send_list(word user_flags) :
	c_class_1474e8(user_flags),
	type(4),
	block(0),
	recipients(0),
	recipient_count(0),
	task_index(NONE),
	to_gamertag(false),
	handler(this, (list_item_method)&c_xbox_live_message_send_list::handle_item)
{
	bool online;

	text[0] = 0;
	memset(g_voice_message_data, 0, sizeof(g_voice_message_data));
	g_voice_message_size = 0;
	g_voice_message_length = 0;
	data = user_interface_data_new("xbox live message send list", 3, 4);
	function_16b790(data);
	online = function_1906da(get_controller_index());
	list_item_add(this, 0);
	if (online)
	{
		list_item_add(this, 1);
		list_item_add(this, 2);
	}
	delegate_register(&item_handlers, &handler);
}

// @retail 0x2b7c6a deleting c_xbox_live_message_send_list

/* frees the message block when it was not sent */
// @retail 0x2b7c86
c_xbox_live_message_send_list::~c_xbox_live_message_send_list()
{
	if (block)
	{
		function_08fa30(block);
		block = 0;
	}
}

// @retail 0x2b7cd8
void c_xbox_live_message_send_list::v1()
{
	s_state_block *memory = (s_state_block *)function_1a47fd(sizeof(s_state_block));
	s_state_block *allocated_block;

	if (memory)
	{
		function_08eeb0(memory);
		allocated_block = memory;
	}
	else
	{
		allocated_block = 0;
	}
	block = allocated_block;
	((c_widget *)this)->c_widget::v9();
}

/* the items' texts */
// @retail 0x2b7d00
void c_xbox_live_message_send_list::v20(c_class_1a2c81 *widget, long index)
{
	s_list_item_text table[3];

	table[0].item = _message_send_item_send;
	table[0].string_handle = 0xc0002a6;
	table[1].item = _message_send_item_edit_text;
	table[1].string_handle = 0x13000601;
	table[2].item = _message_send_item_record_voice;
	table[2].string_handle = 0x14000602;
	function_24c75c(this, widget, table, 0, 3);
}

/* runs the chosen item, unless the message is already sent */
// @retail 0x2b7d40
void c_xbox_live_message_send_list::handle_item(s_controller_reference **controller, long *item)
{
	if (!((c_xbox_live_message_send_screen *)get_screen())->sent && function_6c7e0())
	{
		if (*item != NONE)
		{
			switch (*item & 0xffff)
			{
			case _message_send_item_send:
				send(controller);
				break;
			case _message_send_item_edit_text:
				open_text_editor(controller);
				break;
			case _message_send_item_record_voice:
				xbox_live_message_send_record_voice(controller);
				break;
			}
		}
	}
	else
	{
		function_236299(2);
	}
}

/* lets the user write the text, starting from the default one */
// @retail 0x2b7da6
void c_xbox_live_message_send_list::open_text_editor(s_controller_reference **controller)
{
	unsigned long length = 0;

	while (length < 0x100 && text[length])
	{
		length++;
	}
	if (!length)
	{
		set_default_text();
	}
	function_238c21((*controller)->controller_index, 0xe, text, 0x100);
}

/* opens the voice recording screen */
// @retail 0x2b7de4
void xbox_live_message_send_record_voice(s_controller_reference **controller)
{
	if (voice_port_can_talk((*controller)->controller_index))
	{
		s_screen_parameters parameters;
		s_voice_record_screen_view *screen;

		parameters.field_c = 0;
		function_149f49((s_message *)&parameters, 0, 0, 1 << (*controller)->controller_index, 1, 4, (long)function_2c9012);
		screen = (s_voice_record_screen_view *)parameters.load(&parameters);
		g_voice_message_size = 0;
		g_voice_message_length = 0;
		if (screen)
		{
			screen->data = g_voice_message_data;
			screen->maximum_size = sizeof(g_voice_message_data);
			screen->size = &g_voice_message_size;
			screen->length = &g_voice_message_length;
		}
	}
	else
	{
		function_236299(2);
	}
}

/* sends the message */
// @retail 0x2b7e69
void c_xbox_live_message_send_list::send(s_controller_reference **controller)
{
	XUID const *message_recipients;
	long count;
	bool voice;
	s_message_send_selection selection;
	s_screen_settings_54dc6c settings;

	if (recipients && recipient_count > 0)
	{
		message_recipients = recipients;
		count = recipient_count;
	}
	else if (!to_gamertag)
	{
		XUID *xuid;

		function_14887e((s_screen_settings_54dc6c *)&selection);
		xuid = 0;
		switch (selection.type)
		{
		case 1:
			xuid = &selection.user.xuid;
			break;
		case 2:
			xuid = &selection.friend_.xuid;
			break;
		}
		message_recipients = xuid;
		if (xuid && *(unsigned __int64 *)xuid)
			count = 1;
		else
			count = 0;
	}
	voice = false;
	if (g_voice_message_size > 0 && g_voice_message_length > 0 && voice_data_is_wave((const long *)g_voice_message_data, g_voice_message_size))
	{
		voice = true;
	}
	if (block)
	{
		long task;

		if (type == 4 && !text[0] && !voice)
		{
			function_236299(2);
			return;
		}
		online_message_block_set_text(block, (const wchar_t *)text);
		if (voice)
		{
			online_message_block_set_values(block, (long)g_voice_message_size, (long)g_voice_message_data, g_voice_message_length);
		}
		block->unknown4 = type;
		if (!to_gamertag)
		{
			task = function_08eff0(block, (*controller)->controller_index, message_recipients, count);
		}
		else
		{
			function_14887e(&settings);
			task = function_08ef90(block, (*controller)->controller_index, function_148956((s_window_manager_text *)&settings));
		}
		block = 0;
		task_index = task;
	}
	((c_xbox_live_message_send_screen *)get_screen())->sent = true;
}

/* the text a message of this type starts with */
// @retail 0x2b7fc5
void c_xbox_live_message_send_list::set_default_text()
{
	word buffer[0x100];

	buffer[0] = 0;
	switch (type)
	{
	case 1:
		((c_widget *)get_screen())->function_230134(0x130002dc, buffer);
		break;
	case 2:
		((c_widget *)get_screen())->function_230134(0x110002dd, buffer);
		break;
	case 3:
		((c_widget *)get_screen())->function_230134(0x120002da, buffer);
		break;
	default:
		((c_widget *)get_screen())->function_230134(0x140002de, buffer);
		break;
	}
	unicode_string_copy(text, buffer, 0x100);
	function_22d2ee(text, 0x100);
}
