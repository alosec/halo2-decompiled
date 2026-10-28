// @flags /O1 /arch:SSE /Gr
/* UNKNOWN_24C177.CPP: a list's item widgets and the data they show: which
   item is focused, and moving the selection to a datum */

#include "unknown_11c920.h"
#include "data_array.h"
#include "screen_widgets.h"
#include "unknown_19b516.h"

long function_16bcc0(s_record_pool *data, long datum_index);

typedef long (__stdcall *datum_step_proc)(s_record_pool *data, long datum_index);

/* a list's items are list item widgets; retail reads the widget's type
   (a check whose read survived) wherever it treats a child as one */
static inline c_class_14750b *list_item(c_class_1a2c81 *widget)
{
	volatile long type = widget->type;
	return (c_class_14750b *)widget;
}

/* gives the list's items the list's data in order */
// @retail 0x24c0c4
void function_24c0c4(c_widget *widget)
{
	c_class_1474e8 *list = (c_class_1474e8 *)widget;

	if (list->data)
	{
		long datum = record_pool_next_used(list->data, NONE);
		c_class_1a2c81 *child;

		for (child = list->child; child; child = child->next)
		{
			list_item(child)->value70 = datum;
			if (datum != NONE)
			{
				datum = record_pool_next_used(list->data, datum);
			}
			else
			{
				datum = NONE;
			}
		}
	}
}

#if H2_WHOLE_IMAGE && !defined(H2_NATIVE_PORT)
/* the base's v3, which derived lists call by name, is retail 0x24bd61
   (recovered as c_widget::v11, the same code) */
extern "C" void h2_retail_24bd61(void *self);
void c_class_1474e8::v3()
{
	h2_retail_24bd61(this);
}

/* and its other slots derived lists call by name: retail table 0x45a030 */
extern "C" void h2_retail_24bb9c(void *self);
extern "C" void h2_retail_24bbd0(void *self);
extern "C" bool h2_retail_24c024(void *self, s_widget_event *event);
extern "C" void h2_retail_24bb60(void *self);
void c_class_1474e8::v1()
{
	h2_retail_24bb9c(this);
}

void c_class_1474e8::v2()
{
	h2_retail_24bbd0(this);
}

bool c_class_1474e8::v10(s_widget_event *event)
{
	return h2_retail_24c024(this, event);
}

void c_class_1474e8::v17()
{
	h2_retail_24bb60(this);
}
#endif

#if defined(H2_NATIVE_PORT)
struct s_event;
void function_24c7e4(void *list_pointer, s_event **event, long *key);

/* Retail 0x24bb9c initializes the item datums and focus after a screen has
   attached the list. */
void c_class_1474e8::v1()
{
	c_class_1a2c81::v1();
	function_24c0c4((c_widget *)this);
	if (!is_in_window() && child)
	{
		v7(child);
		function_24c610(child, (c_widget *)this);
	}
}

/* Retail 0x24bbd0 owns the list's optional record pool. */
void c_class_1474e8::v2()
{
	if (value7f && data)
	{
		data_dispose(data);
		data = 0;
	}
	c_class_1a2c81::v2();
}

/* Retail 0x24bd61 advances the list's scroll state, then its item widgets. */
void c_class_1474e8::v3()
{
	if (function_24c63e((c_widget *)this) && value74)
		--value74;
	if (function_24c676((c_widget *)this) && value76)
		--value76;
	c_class_1a2c81::v3();
}

/* Retail 0x24bb60 attaches the concrete list's embedded item widgets before
   the recovered list skin is applied. */
void c_class_1474e8::v17()
{
	c_class_14750b *items = (c_class_14750b *)get_item_data();
	long count = get_item_count();
	for (long index = 0; index < count; ++index)
	{
		items[index].set_user_flags(user_flags);
		add_child(&items[index]);
	}
}

/* Native spelling of retail 0x24c024. */
bool c_class_1474e8::v10(s_widget_event *event)
{
	if (event && (event->type == 1 || event->type == 3))
	{
		function_24c1c5((c_widget *)this, event->type == 1 ? -1 : 1);
		return true;
	}
	if (event && event->type == 5 && (event->param == 0 || event->param == 12))
	{
		c_class_1a2c81 *item = get_focused_item();
		if (item)
		{
			long datum = ((c_class_14750b *)item)->value70;
			s_event *controller_event = (s_event *)event;
			function_24c7e4(&item_handlers, &controller_event, &datum);
			return true;
		}
	}
	return c_class_1a2c81::v10(event);
}
#endif

/* the list's definition in its screen's current pane */
// @retail 0x24bbf3
s_list_definition *c_class_1474e8::function_1751d0()
{
	c_class_1473c9 *screen = (c_class_1473c9 *)parent;
	s_list_definition *result = 0;

	if (screen)
	{
		s_screen_pane *pane = screen->get_current_pane();
		if (pane && pane->list_count > 0)
		{
			result = pane->lists;
		}
	}
	return result;
}

// @retail 0x24c0b3
long c_class_1474e8::get_skin_index()
{
	s_list_definition *definition = function_1751d0();

	if (definition)
	{
		return definition->skin_index;
	}
	return 0;
}

/* an item animation of the list's skin (16 bytes each) */
// @retail 0x24bd3b
void *c_class_1474e8::get_item_animation(long index)
{
	s_list_definition *definition = function_1751d0();
	s_sprite_placement *skin;

	if (definition)
	{
		skin = ui_definition_get_skin(definition->skin_index);
	}
	else
	{
		skin = ui_definition_get_skin(0);
	}
	return skin->item_animations + index * 16;
}

// @retail 0x24c177
long c_class_1a2c81::child_count()
{
	long count = 0;

	for (c_class_1a2c81 *widget = child; widget; widget = widget->next)
	{
		count++;
	}
	return count;
}

/* the list's children, less the items that show no datum */
// @retail 0x24c187
long c_class_1474e8::count_filled_items()
{
	long count = 0;

	for (c_class_1a2c81 *widget = child; widget; widget = widget->next)
	{
		switch (widget->type)
		{
		case 2:
			if (((c_class_14750b *)widget)->value70 == NONE)
			{
				break;
			}
		default:
			count++;
			break;
		}
	}
	return count;
}

/* a datum of the list's data */
// @retail 0x24c5f2
void *c_class_1474e8::function_24c5f2(long datum)
{
	void *result;
	if (data && datum != NONE)
	{
		result = data->data + (datum & 0xffff) * data->size;
	}
	else
	{
		result = 0;
	}
	return result;
}

// @retail 0x24c1a4
c_class_1a2c81 *c_class_1a2c81::get_child(long index)
{
	c_class_1a2c81 *result = 0;

	for (c_class_1a2c81 *widget = child; widget; widget = widget->next)
	{
		if (!index--)
		{
			result = widget;
			break;
		}
	}
	return result;
}

// @retail 0x24bae6
c_class_1a2c81 *c_class_1474e8::get_focused_item()
{
	c_class_1a2c81 *widget;

	for (widget = child; widget; widget = widget->next)
	{
		if (widget->is_in_window())
		{
			break;
		}
	}
	return widget;
}

// @retail 0x24c447
long c_class_1474e8::get_focused_datum()
{
	c_class_1a2c81 *item = get_focused_item();
	long datum = NONE;

	if (item)
	{
		datum = list_item(item)->value70;
	}
	return datum;
}

// @retail 0x24c5d0
c_class_1a2c81 *c_class_1474e8::find_item(long datum)
{
	c_class_1a2c81 *widget;

	for (widget = child; widget; widget = widget->next)
	{
		if (list_item(widget)->value70 == datum)
		{
			break;
		}
	}
	return widget;
}

/* the steps between the data of a list's items: with or without wrapping */

// @retail 0x24c6b7
long __stdcall datum_next_wrapping(s_record_pool *data, long datum_index)
{
	long result;

	if (data)
	{
		result = record_pool_next_used(data, datum_index);
		if (result != NONE)
		{
			return result;
		}
		result = record_pool_next_used(data, NONE);
		if (result != datum_index)
		{
			return result;
		}
	}
	return NONE;
}

// @retail 0x24c6e2
long __stdcall datum_previous_wrapping(s_record_pool *data, long datum_index)
{
	long result;

	if (data)
	{
		result = function_16bcc0(data, datum_index);
		if (result == NONE)
		{
			long index = result;
			while ((index = record_pool_next_used(data, index)) != NONE)
			{
				result = index;
			}
			if (result == datum_index)
			{
				result = NONE;
			}
		}
	}
	else
	{
		result = NONE;
	}
	return result;
}

// @retail 0x24c71e
long __stdcall datum_next(s_record_pool *data, long datum_index)
{
	if (data && datum_index != NONE)
	{
		return record_pool_next_used(data, datum_index);
	}
	return NONE;
}

// @retail 0x24c73e
long __stdcall datum_previous(s_record_pool *data, long datum_index)
{
	long result = NONE;

	if (data && datum_index != NONE)
	{
		result = function_16bcc0(data, datum_index);
	}
	return result;
}

long data_last_index(s_record_pool *data);
void function_236299(long sound);

/* the item next to this one in the step's direction */
static inline c_class_1a2c81 *list_item_step(c_class_1a2c81 *item, long step)
{
	return step >= 0 ? item->next : item->previous;
}

/* moves the list's focus one item in the direction; at either end the data
   scrolls through the items instead (or wraps to the other end) */
// @retail 0x24c1c5
void function_24c1c5(c_widget *widget, char direction)
{
	c_class_1474e8 *list = (c_class_1474e8 *)widget;
	c_class_1a2c81 *focused = list->get_focused_item();

	if (focused)
	{
		long old_datum = list->get_focused_datum();
		bool moved = false;
		bool blocked = false;
		long step = direction ? (direction >= 0 ? 1 : -1) : 0;

		if (step >= 0)
		{
			c_class_1a2c81 *next = focused->next;

			if (next)
			{
				if (next->type == 2 && !((c_class_14750b *)next)->v17())
				{
					if (list->value7c)
					{
						focused = list->child;
						if (((c_class_14750b *)focused)->v17())
						{
							list->v7(focused);
							moved = true;
						}
					}
					else
					{
						blocked = true;
					}
				}
				else
				{
					list->v7(list_item_step(focused, step));
					moved = true;
				}
			}
			else
			{
				long datum = list->get_focused_datum();

				if (datum != NONE && record_pool_next_used(list->data, datum) == NONE && list->value7c)
				{
					focused = list->child;
					list->v7(focused);
					function_24c0c4(widget);
					moved = true;
				}
			}
			list->value74 = 0;
			list->value76 = 5;
		}
		else
		{
			c_class_1a2c81 *previous = focused->previous;

			if (previous)
			{
				if (previous->type == 2 && !((c_class_14750b *)previous)->v17())
				{
					blocked = true;
				}
				else
				{
					list->v7(list_item_step(focused, step));
					moved = true;
				}
			}
			else
			{
				long datum = list->get_focused_datum();

				if (datum != NONE && function_16bcc0(list->data, datum) == NONE && list->value7c)
				{
					long count = list->count_filled_items();

					if (count > 0)
					{
						focused = list->get_child(count - 1);
						list->v7(focused);
						datum = data_last_index(list->data);
						if (datum != NONE)
						{
							do
							{
								if (!focused) break;
								c_class_1a2c81 *previous = focused->previous;
								((c_class_14750b *)focused)->value70 = datum;
								focused = previous;
								datum = function_16bcc0(list->data, datum);
							} while (datum != NONE);
						}
						moved = true;
					}
				}
			}
			list->value76 = 0;
			list->value74 = 5;
		}
		if (!moved && !blocked)
		{
			datum_step_proc step_proc;

			if (list->wraps)
			{
				step_proc = step >= 0 ? datum_next_wrapping : datum_previous_wrapping;
			}
			else
			{
				step_proc = step >= 0 ? datum_next : datum_previous;
			}
			bool at_end = list->wraps ? false : step_proc(list->data, list_item(focused)->value70) == NONE;

			if (!at_end)
			{
				for (c_class_1a2c81 *item = list->child; item; item = item->next)
				{
					c_class_14750b *entry = list_item(item);
					entry->value70 = step_proc(list->data, entry->value70);
				}
			}
		}
		c_class_1a2c81 *selected = list->get_focused_item();

		function_24c610(selected, widget);
		{
			long datum = list->get_focused_datum();

			if (datum != NONE && datum != old_datum)
			{
				function_236299(0);
			}
		}
	}
}

/* shows the data around the datum in the items around the focused one */
// @retail 0x24c461
void c_class_1474e8::assign_items(long datum)
{
	c_class_1a2c81 *focused = get_focused_item();
	datum_step_proc next = wraps ? datum_next_wrapping : datum_next;
	datum_step_proc previous = wraps ? datum_previous_wrapping : datum_previous;

	if (focused)
	{
		c_class_1a2c81 *widget;
		long value;

		list_item(focused)->value70 = datum;
		value = datum;
		for (widget = focused->next; value = next(data, value), widget; widget = widget->next)
		{
			list_item(widget)->value70 = value;
		}
		value = datum;
		for (widget = focused->previous; value = previous(data, value), widget; widget = widget->previous)
		{
			list_item(widget)->value70 = value;
		}

		if (notify_screen)
		{
			c_class_1473c9 *screen = get_screen();
			if (screen)
			{
				long focused_datum = get_focused_datum();
				if (focused_datum != NONE)
				{
					/* 0x230427 reads the datum's low word */
					screen->select_pane((short *)&focused_datum);
				}
			}
		}
	}
}

// @retail 0x24c515
void c_class_1474e8::select_datum(long datum)
{
	c_class_1a2c81 *item = find_item(datum);

	if (!item)
	{
		long count = child_count();
		s_record_pool *items = data;
		long datum_count = items ? items->actual_count : 0;

		if (datum_count <= count)
		{
			return;
		}

		long datum_index = record_pool_next_used(items, NONE);
		long index = 0;
		for (; datum_index != NONE; datum_index = record_pool_next_used(items, datum_index), index++)
		{
			if (datum_index == datum)
			{
				item = get_child(index <= count - 1 ? index : count - 1);
				break;
			}
		}
		if (!item)
		{
			return;
		}
	}
	v7(item);
	assign_items(datum);
}

// @retail 0x24c591
void c_class_1474e8::select_item(short item)
{
	s_record_pool *items = data;
	long datum_index;

	for (datum_index = record_pool_next_used(items, NONE); datum_index != NONE; datum_index = record_pool_next_used(items, datum_index))
	{
		if ((datum_index & 0xffff) == item)
		{
			break;
		}
	}
	if (datum_index != NONE)
	{
		select_datum(datum_index);
	}
}

/* shows the text of the item's datum, looked up in a table of texts */
// @retail 0x24c75c
bool function_24c75c(c_class_1474e8 *list, c_class_1a2c81 *item, s_list_item_text *table, long text_index, long count)
{
	bool result = false;

	if (item)
	{
		c_text_widget_45a5e0 *text = (c_text_widget_45a5e0 *)item->find_text(text_index);
		if (text)
		{
			s_list_item_datum *datum = (s_list_item_datum *)record_pool_lookup(list->data, ((c_class_14750b *)item)->value70);
			if (datum)
			{
				short value = datum->item;
				for (long i = 0; i < count; i++)
				{
					if (table[i].item == value)
					{
						text->function_253b1a(table[i].string_handle);
						result = true;
						break;
					}
				}
			}
		}
	}
	return result;
}

struct s_widget_group_definition;
void function_2bacbc(c_class_1a2c81 *widget, s_widget_group_definition *group, s_widget_point *point);
void function_2bafa4(c_class_1a2c81 *widget, s_widget_bounds *bounds);

/* takes an item out of the list, focusing its neighbour */
// @retail 0x24c102
void function_24c102(c_class_1474e8 *list, c_class_1a2c81 *item)
{
	c_class_1a2c81 *previous = item->previous;
	c_class_1a2c81 *next = item->next;

	list->remove_child(item);
	function_24c0c4((c_widget *)list);
	if (item->is_in_window())
	{
		if (next)
		{
			list->v7(next);
			function_24c610(next, (c_widget *)list);
		}
		else if (previous)
		{
			list->v7(previous);
			function_24c610(previous, (c_widget *)list);
		}
		else
		{
			list->parent->v7(list);
		}
	}
}

/* lays the list's items out as its definition and skin describe; the items
   past the definition's count are removed when asked */
// @retail 0x24bc12
void function_24bc12(c_class_1474e8 *list, bool remove_extra)
{
	s_list_definition *definition;
	c_class_1a2c81 *item;

	list->v17();
	definition = list->function_1751d0();
	item = list->child;
	list->value7c = (bool)(definition ? (definition->flags & 1) : 0);
	list->value68 = definition ? definition->value0c - 1 : 0;
	list->value78 = definition ? definition->value0e : 0;
	if (definition)
	{
		short width = 0;
		short height = 0;
		s_widget_group_definition *skin = (s_widget_group_definition *)ui_definition_get_skin(definition->skin_index);
		s_widget_point point;
		s_widget_bounds bounds;
		long i;

		point = *(s_widget_point const *)&definition->x;
		for (i = 0; i < definition->item_count && item; i++)
		{
			s_widget_bounds local_50a200;

			function_2bacbc(item, skin, &point);
			item->value68 = list->value68;
			((c_class_14750b *)item)->value74 = list->value78;
			function_2bafa4(item, &local_50a200);
			height = local_50a200.top - local_50a200.bottom;
			point.y -= height;
			width = local_50a200.right - local_50a200.left;
			item = item->next;
		}
		bounds.left = point.x;
		bounds.right = point.x + width;
		bounds.top = point.y;
		bounds.bottom = (short)(i * height) + point.y;
		list->bounds = bounds;
	}
	if (remove_extra)
	{
		while (item)
		{
			c_class_1a2c81 *next = item->next;

			function_24c102(list, item);
			item = next;
		}
	}
}

/* takes every item out of the list */
// @retail 0x24c166
void function_24c166(c_class_1474e8 *list)
{
	c_class_1a2c81 *item;

	while ((item = list->child) != 0)
	{
		function_24c102(list, item);
	}
}

/* whether the item is the focused one */
// @retail 0x24c3f8
bool c_class_1474e8::v21(c_class_1a2c81 *item)
{
	return get_focused_item() == item;
}

/* whether the item comes before the focused item */
// @retail 0x24c40b
bool function_24c40b(c_class_1474e8 *list, c_class_1a2c81 *item)
{
	bool result = false;

	if (!list->v21(item))
	{
		c_class_1a2c81 *widget = list->child;
		c_class_1a2c81 *focused = list->get_focused_item();

		for (; widget; widget = widget->next)
		{
			if (widget == focused)
			{
				break;
			}
			result = widget == item;
			if (result)
			{
				break;
			}
		}
	}
	return result;
}

struct s_item_list;
void function_24c7c1(s_item_list *list, long a);

/* tells the list's focus handlers which datum the item shows */
// @retail 0x24c610
void function_24c610(void *item, c_widget *widget)
{
	c_class_1474e8 *list = (c_class_1474e8 *)widget;

	if (item)
	{
		long datum = list_item((c_class_1a2c81 *)item)->value70;

		if (datum != NONE)
		{
			function_24c7c1((s_item_list *)&list->head80, (long)&datum);
		}
	}
}

/* whether the list's data has a datum before its first item's */
// @retail 0x24c63e
bool function_24c63e(c_widget *widget)
{
	c_class_1474e8 *list = (c_class_1474e8 *)widget;
	c_class_1a2c81 *first = list->child;
	bool result = false;

	if (first && list->data)
	{
		long datum = list_item(first)->value70;

		if (datum != NONE && function_16bcc0(list->data, datum) != NONE)
		{
			result = true;
		}
		else
		{
			result = false;
		}
	}
	return result;
}

/* whether the list's data has a datum after its last item's */
// @retail 0x24c676
bool function_24c676(c_widget *widget)
{
	c_class_1474e8 *list = (c_class_1474e8 *)widget;
	bool result = false;

	if (list->child && list->data)
	{
		s_record_pool *data = list->data;
		c_class_1a2c81 *last = list->child;

		while (last->next)
		{
			last = last->next;
		}

		long datum = list_item(last)->value70;

		if (datum != NONE && record_pool_next_used(data, datum) != NONE)
		{
			result = true;
		}
		else
		{
			result = false;
		}
	}
	return result;
}
