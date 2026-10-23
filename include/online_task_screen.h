#pragma once
#include "screen_widgets.h"

class c_online_task_screen;
typedef void (__stdcall *online_task_screen_callback)(c_online_task_screen *screen);

/* the screen (0x624 bytes, screen id 0xb6) */
class c_online_task_screen : public c_class_1473c9
{
public:
	c_online_task_screen(long a, long b, word user_flags);
	virtual ~c_online_task_screen();
	virtual void v3();
	virtual bool v10(s_widget_event *event);
	virtual screen_load_proc get_load_proc();

	void set_title(long string_handle);
	void set_description(long string_handle);
	void set_title_from_block(long block_index, long index);
	void set_description_from_block(long block_index, long index);

	online_task_screen_callback finished;
	online_task_screen_callback cancelled;
	byte unknown618[4];
	long task_index;
	HRESULT result;
};

