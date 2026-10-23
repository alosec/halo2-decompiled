// @flags /O1 /arch:SSE /Gr
/* UNKNOWN_19B516.CPP: the methods of the widget class whose vtable is at
   0x4594e0 */

#include "unknown_11c920.h"
#include <xtl.h>
#include <xonline.h>
#include "globals.h"
#include "unknown_19b516.h"
#include "unknown_234c64.h"

bool xuid_equal(XUID const *a, XUID const *b, bool compare_guest_number);

dword g_54d5b8;


// @retail 0x24bb60
void c_widget::v1()
{
	c_list_view *list = (c_list_view *)this;
	c_class_14750b *items = (c_class_14750b *)list->get_first();
	long count = list->get_count();
	word flags = m8;

	for (long i = 0; i < count; i++)
	{
		c_class_14750b *item = &items[i];

		item->set_user_flags(flags);
		((c_class_1a2c81 *)(void *)this)->add_child(item);
	}
}

// @retail 0x19b516
void *c_widget::v2()
{
	return sub88;
}

// @retail 0x232d3f
long c_widget::v3()
{
	return 0x10;
}

// @retail 0x24bb9c
void c_widget::v9()
{
	function_22e315();
	function_24c0c4(this);
	if (!function_22ed7a() && child)
	{
		v7(child);
		function_24c610(child, this);
	}
}

// @retail 0x24bbd0
void c_widget::v10()
{
	if (m7f)
	{
		s_record_pool *p = m70;
		if (p)
		{
			data_dispose(p);
			m70 = 0;
		}
	}
	function_22e335();
}

// @retail 0x24bd61
void c_widget::v11()
{
	if (function_24c63e(this))
	{
		word value = m74;
		if (value)
		{
			m74 = value - 1;
		}
	}
	if (function_24c676(this))
	{
		word value = m76;
		if (value)
		{
			m76 = value - 1;
		}
	}
	function_22e391();
}

// @retail 0x22e930
void c_widget::v13(s_event *event)
{
	function_22e89c(event);
	for (c_widget *widget = child; widget; widget = widget->next)
	{
		widget->v5(event);
	}
}

// @retail 0x230130
long c_widget::v14()
{
	return m78;
}

// @retail 0x22eb8c
void c_widget::v15(c_widget *widget)
{
	if (widget)
	{
		widget->function_22ecb4(true);
		for (c_widget *item = widget->child; item; item = item->next)
		{
			if (item->function_22e37f())
			{
				widget->v7(item);
				break;
			}
		}
	}
}

// @retail 0x22ebc3
void c_widget::v16()
{
	c_widget *candidate = next;
	if (!candidate)
	{
		c_widget *p = prev;
		candidate = this;
		while (p)
		{
			candidate = p;
			p = candidate->prev;
		}
	}
	c_widget *widget;
	while ((widget = candidate) != 0)
	{
		if (widget->function_22e37f())
		{
			function_22ecb4(false);
			widget->function_22ecb4(true);
			return;
		}
		candidate = widget->next;
		if (!candidate)
		{
			c_widget *p = widget->prev;
			candidate = widget;
			while (p)
			{
				candidate = p;
				p = candidate->prev;
			}
		}
	}
}

// @retail 0x22ec1b
void c_widget::v17()
{
	c_widget *candidate = prev;
	if (!candidate)
	{
		c_widget *p = next;
		candidate = this;
		while (p)
		{
			candidate = p;
			p = candidate->next;
		}
	}
	c_widget *widget;
	while ((widget = candidate) != 0)
	{
		if (widget->function_22e37f())
		{
			function_22ecb4(false);
			widget->function_22ecb4(true);
			return;
		}
		candidate = widget->prev;
		if (!candidate)
		{
			c_widget *p = widget->next;
			candidate = widget;
			while (p)
			{
				candidate = p;
				p = candidate->next;
			}
		}
	}
}

bool window_manager_channel_window_in_use(long channel, long index);

/* whether a window over the widget's screen's window is in use */
// @retail 0x22ef1b
bool c_widget::function_22ef1b()
{
	c_class_1473c9 *screen = (c_class_1473c9 *)function_22eeee();
	long index = screen->v21();

	switch (screen->v20())
	{
	case 0:
		break;
	case 1:
		return window_manager_channel_window_in_use(0, index);
	case 2:
		if (window_manager_channel_window_in_use(1, index) || g_54d598.window_0.current || g_54d598.window_0.next)
		{
			return true;
		}
		break;
	case 3:
		if (window_manager_channel_window_in_use(1, index) || g_54d598.window_0.current || g_54d598.window_0.next ||
			window_manager_channel_window_in_use(2, 4))
		{
			return true;
		}
		break;
	case 4:
		if (window_manager_channel_window_in_use(1, index) || g_54d598.window_0.current || g_54d598.window_0.next ||
			window_manager_channel_window_in_use(3, index) || window_manager_channel_window_in_use(2, 4))
		{
			return true;
		}
		break;
	case 5:
	case 6:
		if (window_manager_channel_window_in_use(1, index) || g_54d598.window_0.current || g_54d598.window_0.next ||
			window_manager_channel_window_in_use(3, index) || window_manager_channel_window_in_use(4, index) ||
			window_manager_channel_window_in_use(2, 4))
		{
			return true;
		}
		break;
	}
	return false;
}

// @retail 0x24c024
bool c_widget::v18(s_event *event)
{
	bool handled = false;
	switch (event->type)
	{
	case 1:
		function_24c1c5(this, -1);
		handled = true;
		break;
	case 3:
		function_24c1c5(this, 1);
		handled = true;
		break;
	case 5:
		{
			long button = event->param;
			if ((button == 0 || button == 12) && !function_22ef1b())
			{
				c_widget *item = function_24bae6(this);
				if (item)
				{
					long type = *(long *)((byte *)item + 4);
					long datum = *(long *)((byte *)item + 0x70);
					volatile long key = type;
					key = datum;
					function_24c7e4(sub84, &event, (long *)&key);
				}
				handled = true;
			}
			break;
		}
	}
	if (!handled)
		handled = function_22ec73(event);
	return handled;
}

// @retail 0x22ec90
long c_widget::v19()
{
	return function_22eeee()->v20();
}

// @retail 0x22ec9e
long c_widget::v20()
{
	c_class_1473c9 *screen = (c_class_1473c9 *)function_22eeee();
	if (screen)
	{
		return screen->v21();
	}
	return 4;
}

// @retail 0x24c09d
void c_widget::v21()
{
	v7(child);
	function_24c610(child, this);
}

#include "slot_handler.h"
static __forceinline long real_to_long(real value);

// @retail 0x24bda2
void c_widget::v12(long a)
{
	real offset_y;
	real offset_x;
	short x;
	short y;
	s_bounds bounds;

	function_2363d4((short_rectangle2d const *)a, &x, &y);
	if (function_22f0ff(this))
	{
		return;
	}
	s_sprite_placement *placement = ui_definition_get_skin(function_24c0b3(this));
	if (!placement)
	{
		return;
	}
	long tag_index = placement->tag_index;
	s_sprite_tag *tag = (s_sprite_tag *)g_4e3b44[tag_index & 0xffff].flags;
	real scale = function_22e9aa(this);
	function_22e9c6(&bounds.a);
	if (tag_index == NONE)
	{
		return;
	}
	if (function_24c63e(this))
	{
		real opacity = 1.0f;
		if (m74 == 0)
		{
			opacity = 0.5f;
		}
		s_sprite_element *element = &tag->sprite->main;
		if (tag->sprite)
		{
			s_float_rect from;
			s_float_rect to;
			from.y0 = 0.0f;
			from.x0 = 0.0f;
			from.x1 = (real)element->width;
			from.y1 = (real)element->height;
			to.x0 = (real)(placement->main_x + bounds.b);
			to.y0 = (real)(placement->main_y + bounds.c);
			to.y1 = to.y0 - from.y1;
			to.x1 = to.x0 + from.x1;
			offset_x = (real)x;
			offset_y = (real)y;
			to.x0 += offset_x;
			to.y0 += offset_y;
			to.x1 += offset_x;
			to.y1 += offset_y;
			function_23618e(&to, scale, (short_rectangle2d const *)a);
			opacity *= 255.0f;
			long alpha = real_to_long(opacity);
			function_235e5e(element, &from, &to, (alpha << 24) | 0xffffff, 0, 0);
		}
	}
	if (function_24c676(this))
	{
		real opacity = 1.0f;
		if (m76 == 0)
		{
			opacity = 0.5f;
		}
		s_sprite_element *element = &tag->sprite->second;
		if (element)
		{
			s_float_rect from;
			s_float_rect to;
			from.y0 = 0.0f;
			from.x0 = 0.0f;
			from.x1 = (real)element->width;
			from.y1 = (real)element->height;
			to.x0 = (real)(placement->second_x + bounds.b);
			to.y0 = (real)(placement->second_y + bounds.c);
			to.y1 = to.y0 - from.y1;
			to.x1 = to.x0 + from.x1;
			offset_x = (real)x;
			offset_y = (real)y;
			to.x0 += offset_x;
			to.y0 += offset_y;
			to.x1 += offset_x;
			to.y1 += offset_y;
			function_23618e(&to, scale, (short_rectangle2d const *)a);
			opacity *= 255.0f;
			long alpha = real_to_long(opacity);
			function_235e5e(element, &from, &to, (alpha << 24) | 0xffffff, 0, 0);
		}
	}
}

