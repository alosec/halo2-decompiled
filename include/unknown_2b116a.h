/* UNKNOWN_2B116A.H: shared types of the screens and lists of
   src/unknown_2b116a.cpp */

#ifndef UNKNOWN_2B116A_H
#define UNKNOWN_2B116A_H

#include "unknown_11c920.h"
#include "data_array.h"
#include "screen_widgets.h"

/* an iterator over a list's items (the item, then the data iterator) */
struct s_list_item_iterator
{
	byte *item;
	s_record_pool_iterator iterator;
};

bool function_2b2327(s_list_item_iterator *iterator);

/* a screen that shows a short text and a bitmap (vtable 0x45bd40; the window
   channels load it, 0x23591a) */
class c_screen_45bd40 : public c_class_1473c9
{
public:
	c_screen_45bd40(long a, long b, word user_flags);

	/* shows the text */
	virtual void v3();
	virtual void v18(void *parameters);
	virtual screen_load_proc get_load_proc();

	void set_text(const char *string);
	void set_bitmap(short index);

	word text[0x10];
};

/* a saved game header read in the background (0x124360): the flags the
   reader sets, its error and its progress */
struct s_saved_game_read
{
	bool volatile done;
	bool cancel;
	bool success;
	byte unknown03;
	long error;
	byte unknown08[0x108 - 0x08];
	real progress;
};

/* the saved game header it reads (0x1288 bytes) */
struct s_saved_game_header
{
	byte unknown000[0x148];
	long level;
	byte unknown14c[0x25a - 0x14c];
	short difficulty;
	byte unknown25c[0x1288 - 0x25c];
};

/* "campaign options list" (vtable 0x45af18): play a new campaign or continue
   the saved one */
class c_campaign_options_list : public c_class_1474e8
{
public:
	c_campaign_options_list(word user_flags);

	/* waits for the saved game read to stop */
	virtual void v2();
	/* continues the saved game once it has been read */
	virtual void v3();
	virtual void *get_item_data() { return items; }
	virtual long get_item_count();
	virtual void v20(c_class_1a2c81 *widget, long index);

	void handle_item(s_controller_reference **controller, long *item);

	c_class_14750b items[3];
	c_list_item_handler handler;
	bool reading;
	s_saved_game_read read;
	s_saved_game_header header;
	byte unknown15b8[0x1730 - 0x15b8];
};

/* the campaign options dialog (vtable 0x458a00) */
class c_screen_458a00 : public c_screen_with_menu
{
public:
	c_screen_458a00(long a, long b, word user_flags);

	/* shows the focused option's description */
	virtual void v3();
	virtual screen_load_proc get_load_proc();

	c_campaign_options_list list;
};

#endif
