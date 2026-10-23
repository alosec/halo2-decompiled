/* UNKNOWN_19B510.H: the error dialog screens (unknown_19b510.cpp) and the
   button widget the "ok" dialog holds */

#ifndef UNKNOWN_19B510_H
#define UNKNOWN_19B510_H

#include "unknown_11c920.h"
#include "screen_widgets.h"

/* a dialog's definition, as 0x23661f reads it (0x20 bytes): the string list
   and the ids of its strings, the number of choices and its screen */
struct s_dialog_definition
{
	long dialog_id;
	byte unknown04[2];
	char choices;
	byte unknown07;
	long string_list_index;
	long title;
	long message;
	long first_choice;
	long second_choice;
	short screen_id;
	byte unknown1e[2];
};

/* called when a dialog closes, with the dialog id; true lets it close */
typedef bool (__stdcall *dialog_closed_callback)(c_class_1473c9 *screen, long dialog_id);

/* called when the player chooses, with the player's controller; true closes
   the dialog */
typedef bool (__stdcall *dialog_choice_callback)(long controller_index);

/* the base dialog screen (vtable 0x454640) */
class c_dialog_screen : public c_class_1473c9
{
public:
	c_dialog_screen(long screen_id, long a, long b, word user_flags);

	virtual void v3();
	/* a press of B, back or start is the dialog's */
	virtual bool v10(s_widget_event *event);

	void set_dialog(long dialog_id, bool unused);

	long dialog_id;
	word title[0x100];
	word message[0x100];
	word first_choice[0x100];
	word second_choice[0x100];
	char choices;
	byte unknowne15[3];
	dialog_closed_callback closed;
};

/* opens the "ok" dialog (0x19b527) */
void dialog_ok_show(long a, long dialog_id, long b, short user_flags, dialog_choice_callback chosen, dialog_closed_callback closed);

/* opens the ok/cancel dialog (0x19b5af), and without its second choice and
   closed callbacks (0x19b590) */
void dialog_choice_show(long a, long dialog_id, long b, short user_flags, dialog_choice_callback first_chosen, dialog_choice_callback second_chosen, dialog_closed_callback closed);
void dialog_choice_show_default(long a, long b, short user_flags, dialog_choice_callback first_chosen, long dialog_id);

void function_23661f(s_dialog_definition *definition, long dialog_id);

#endif
