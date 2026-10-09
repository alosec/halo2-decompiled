#include "unknown_11c920.h"
#include "globals.h"
#include "object_iterator.h"

// @flags /O2 /arch:SSE /Gr

/* local views of the object, its header and the object list (the shared
   header data is s_object_header_data in globals.h; 0bad50 and 108a90 keep
   their own views of the same objects) */
struct s_object
{
	byte unknown00[4];
	union
	{
		dword flags;
		struct
		{
			dword : 14;
			dword flag14 : 1;
			dword flag15 : 1;
			dword : 16;
		};
	};
	byte unknown08[0x14];
	long next_index;
	long next_time;
	byte export24;
	byte export25;
	byte export_flags;
	byte unknown27[0xad];
	long unknownd4;
	byte unknownd8;
};

struct s_object_header
{
	short identifier;
	byte flag0 : 1;
	byte : 7;
	byte type;
	byte unknown04[4];
	s_object *object;
};

struct s_object_list
{
	byte unknown00[4];
	short count;
	short unknown06;
	long first_index;
	byte unknown0c[8];
	long time14;

};

long *g_4de2d0;
s_object_list *g_4de2f4;

struct s_scenario_kind_ab
{
	byte unknown00[0x10];
	short type;
};

// @retail 0xbc970
bool function_bc970(long object_index, long id, real *value_out)
{
	real value = 0.0f;
	bool result = false;
	if (object_index != NONE)
	{
		s_object *object = ((s_object_header *)g_4e0300->data)[object_index & 0xffff].object;
		s_scenario_kind_ab *scenario = (s_scenario_kind_ab *)g_4e0350;
		if ((object->export_flags & 8) || !scenario || scenario->type != 0)
		{
			switch (id)
			{
			case 0x0d0005ff: value = 1.0f; break;
			case 0x170005fa: value = object->export24 * (1.0f / 64.0f); break;
			case 0x170005fb: value = object->export25 * (1.0f / 32.0f); break;
			case 0x180005fc: if (!(object->export_flags & 1)) value = 1.0f; break;
			case 0x1d0005fd: value = (object->export_flags & 2) ? 1.0f : -1.0f; break;
			case 0x1d0005fe: value = (object->export_flags & 4) ? 1.0f : -1.0f; break;
			}
			result = true;
		}
	}
	*value_out = value;
	return result;
}

struct s_local_time
{
	long year;
	long month;
	long day;
	long hour;
	long minute;
	long second;
};

void function_139030(s_local_time *time);
long function_139090();

// @retail 0xbca70
real function_bca70()
{
	long stamp = g_4de2f4->time14;
	real result = 1.0f;
	if (stamp && g_510c54->game_time > stamp)
	{
		real elapsed = (g_510c54->game_time - stamp) * g_510c54->rate;
		if (elapsed > 1.0f)
			result = 0.0f;
		else if (elapsed > 20.0f)
			result = 1.0f;
		else
			result = (elapsed - 15.0f) * 0.2f;
	}
	return result;
}

// @retail 0xbcad0
real function_bcad0()
{
	s_local_time time;
	function_139030(&time);
	long hour = time.hour;
	if (hour < 1)
		hour = 1;
	else if (hour > 24)
		hour = 24;
	return (hour % 12) / 12.0f;
}

// @retail 0xbcb10
real function_bcb10()
{
	s_local_time time;
	function_139030(&time);
	long minute = time.minute;
	if (minute < 0)
		minute = 0;
	else if (minute > 59)
		minute = 59;
	return minute / 60.0f;
}

// @retail 0xbcb60
real function_bcb60()
{
	s_local_time time;
	function_139030(&time);
	long second = time.second;
	if (second < 0)
		second = 0;
	else if (second > 59)
		second = 59;
	return second / 60.0f;
}

// @retail 0xbcbb0
real function_bcbb0()
{
	/* Retail retains the scratch value's stores, including the initial zero. */
	volatile real result = 0.0f;
	switch (function_139090())
	{
	case 1: result = 0.25f; return 0.25f;
	case 2: result = 0.5f; return 0.5f;
	case 3: result = 0.75f; return 0.75f;
	case 4: result = 1.0f; break;
	}
	return result;
}

void function_108e10(long object_index);
void function_108e80(long object_index);
void function_108ef0(long object_index, long a);
void function_108f60(long object_index, long a);

// @retail 0xbb760
long function_bb760(short index)
{
#ifdef H2_NATIVE_PORT
	/* The offline UI bootstrap has no gameplay object-index table. */
	if (!g_4de2d0)
		return NONE;
#endif
	if (index >= 0 && index < 0x280)
		return g_4de2d0[index];
	return NONE;
}

// @retail 0xbb780
void function_bb780(long object_index, long value)
{
	((s_object_header *)g_4e0300->data)[object_index & 0xffff].object->unknownd4 = value;
	function_108e10(object_index);
}

// @retail 0xbb7b0
void function_bb7b0(long object_index)
{
	s_object *object = ((s_object_header *)g_4e0300->data)[object_index & 0xffff].object;
	function_108e80(object_index);
	object->unknownd4 = NONE;
	object->unknownd8 = 0;
}

// @retail 0xbb7f0
void function_bb7f0()
{
	struct
	{
		s_object *object;
		s_type_f1af8e iterator;
	} state;
	function_bae80(&state.iterator, 0, 0);
	while ((state.object = function_baeb0(&state.iterator)) != 0)
	{
		if (state.object->unknownd4 != NONE)
			function_bb7b0(state.iterator.object_index);
	}
}

// @retail 0xbb880
void __stdcall function_bb880(long a)
{
	struct
	{
		s_object *object;
		s_type_f1af8e iterator;
	} state;

	function_bae80(&state.iterator, 0, 0);
	while ((state.object = function_baeb0(&state.iterator)) != 0)
	{
		function_108ef0(state.iterator.object_index, a);
	}
}

// @retail 0xbb8f0
void function_bb8f0(long a)
{
	struct
	{
		s_object *object;
		s_type_f1af8e iterator;
	} state;
	function_bae80(&state.iterator, 0, 0);
	while ((state.object = function_baeb0(&state.iterator)) != 0)
	{
		function_108f60(state.iterator.object_index, a);
	}
}

// @retail 0xbb950
void function_bb950(long object_index, bool add, long delta)
{
	s_object_header *header = &((s_object_header *)g_4e0300->data)[object_index & 0xffff];
	s_object *object = header->object;

	if (add)
	{
		if (!TEST_FIELD_BIT(object->flag14) && !TEST_FIELD_BIT(object->flag15))
		{
			s_object_list *list = g_4de2f4;
			object->next_index = list->first_index;
			list->first_index = object_index;
			object->flag14 = 1;
			if (TEST_FIELD_BIT(header->flag0))
				list->count++;
		}
		long time = g_510c54->game_time;
		time += delta;
		long current = object->next_time;
		object->next_time = (current > time) ? current : time;
	}
	else if (TEST_FIELD_BIT(object->flag14))
	{
		s_object_list *list = g_4de2f4;
		long *link = &list->first_index;
		while (*link != object_index)
			link = &((s_object_header *)g_4e0300->data)[*link & 0xffff].object->next_index;
		*link = object->next_index;
		object->next_index = NONE;
		object->flag14 = 0;
		if (TEST_FIELD_BIT(header->flag0))
			list->count--;
	}
}

#include "unknown_0259d0.h"
#include <math.h>

transform4x3f *function_b8bd0(long object_index, short node_index);
real function_0bff60(real a, real b);
real function_c18e0();
real function_1588b0(long player_index, long type);

// @retail 0xbcc20
bool __stdcall function_bcc20(long object_index, long id, real *out, bool *active)
{
    byte *object = (byte *)((s_object_header *)g_4e0300->data)[object_index & 0xffff].object;
    real value = 0.0f;
    bool forced_active = false;
    bool result = true;
    switch (id)
    {
    case 0x030005a6: value = 1.0f; break;
    case 0x040005a7: value = 0.0f; break;
    case 0x0500055b: value = (bool)((object[0x10a] >> 2) & 1) ? 0.0f : 1.0f; break;
    case 0x0700055c:
        {
            transform4x3f *matrix = function_b8bd0(object_index, 0);
            if (fabs(matrix->forward.k) < 0.995f)
            {
                real angle = (real)atan2(matrix->forward.i, matrix->forward.j);
                value = function_0bff60(angle, *(real *)((byte *)g_4e0350 + 0x1c)) * 0.15915493667125702f + 0.5f;
            }
            else
                value = 1.0f;
        }
        break;
    case 0x0700055d:
        {
            byte *definition = g_4e3b44[*(long *)object & 0xffff].bytes;
            byte *model = g_4e3b44[*(long *)(definition + 0x38) & 0xffff].bytes;
            long count = *(long *)(model + 0x50);
            if (count > 0)
            {
                long variant = *(char *)(object + 0xb1) + 1;
                variant = variant < 0 ? 0 : variant > count ? count : variant;
                value = (real)variant / count;
            }
        }
        break;
    case 0x070006ba: value = function_bcbb0(); break;
    case 0x090006b5: value = function_bca70(); break;
    case 0x0a0006b6: value = function_bcad0(); break;
    case 0x0a0006b9: value = 1.0f; break;
    case 0x0c0006b7: value = function_bcb10(); break;
    case 0x0c0006b8: value = function_bcb60(); break;
    case 0x0d000558: value = *(real *)(object + 0xec); break;
    case 0x0d0005ff: forced_active = function_bc970(object_index, id, &value); break;
    case 0x0f000559: value = *(real *)(object + 0xf0); break;
    case 0x0f0005aa: value = (((dword)object_index * 0x19660d + 0x3c6ef35f) >> 16) * 0.000015259021893143654f; break;
    case 0x0f0005ac:
        if (g_4e6948->state == 2 && ((1 << object[0xaa]) & 3) && *(long *)(object + 0x13c) != NONE)
        {
            if (function_1588b0(*(long *)(object + 0x13c), 2) <= 0.0f)
                break;
        }
        if (*(real *)(object + 0xf0) == 0.0f)
            value = 1.0f;
        break;
    case 0x100006b4: value = 1.0f; break;
    case 0x13000556: value = *(real *)(object + 0xf8); break;
    case 0x130006bb: value = function_c18e0(); break;
    case 0x15000557: value = *(real *)(object + 0xf4); break;
    case 0x170005fa: forced_active = function_bc970(object_index, id, &value); break;
    case 0x170005fb: forced_active = function_bc970(object_index, id, &value); break;
    case 0x1800055a: value = *(real *)(object + 0xf0) - 1.0f; break;
    case 0x180005fc: forced_active = function_bc970(object_index, id, &value); break;
    case 0x1a00071a: value = *(real *)((byte *)g_4de2f4 + 0x70); break;
    case 0x1a00071b: value = *(real *)((byte *)g_4de2f4 + 0x74); break;
    case 0x1a00071c: value = *(real *)((byte *)g_4de2f4 + 0x78); break;
    case 0x1a00071d: value = *(real *)((byte *)g_4de2f4 + 0x7c); break;
    case 0x1d0005fd: forced_active = function_bc970(object_index, id, &value); break;
    case 0x1d0005fe: forced_active = function_bc970(object_index, id, &value); break;
    default: result = false; break;
    }
    value = value < 0.0f ? 0.0f : value > 1.0f ? 1.0f : value;
    *out = value;
    *active = forced_active || value > 0.0f;
    return result;
}

struct s_colour_choice_ab
{
    real weight;
    color3f lower;
    color3f upper;
    long field_1c_4;
};
struct s_colour_choices_ab
{
    long count;
    s_colour_choice_ab *choices;
    long function_count;
    struct s_colour_function_ab *functions;
};
hsv3f *function_1318d0(color3f const *rgb, hsv3f *hsv);
color3f *function_131a00(hsv3f const *hsv, color3f *rgb);
void function_3dd10(long object_index, bool force);

// @retail 0xbe240
void __stdcall function_be240(long object_index, dword color_mask, color3f const *colors)
{
    byte *object = (byte *)((s_object_header *)g_4e0300->data)[object_index & 0xffff].object;
    byte *definition = g_4e3b44[*(long *)object & 0xffff].bytes;
    color3f *output = (color3f *)(object + *(short *)(object + 0x126));
    long count = (long)((dword)(long)*(short *)(object + 0x124) / 12) / 2;
    for (long i = 0; i < count; ++i)
    {
        s_colour_choices_ab *entry = &(*(s_colour_choices_ab **)(definition + 0xb0))[i];
        if (color_mask & (1 << i))
            output[i] = colors[i];
        else
        {
            output[i] = *(color3f *)g_468710;
            long choice_count = entry->count;
            if (choice_count > 0)
            {
                real index = (real)i;
                real seed = (real)fabs(*(real *)(object + 0x6c) * 744.1241455078125f + *(real *)(object + 0x64) * 315.89312744140625f + index * 431.1289367675781f + *(real *)(object + 0x68) * 587.1294555664062f);
                real selection = (real)fmod((double)seed, 1.0);
                seed = (real)fabs(index * 0.7121099829673767f + *(real *)(object + 0x68));
                real blend = (real)fmod((double)seed, 1.0);
                long variant = 0;
                char variant_index = *(char *)(object + 0xb1);
                if (variant_index != NONE && *(long *)(definition + 0x38) != NONE)
                {
                    byte *model = g_4e3b44[*(long *)(definition + 0x38) & 0xffff].bytes;
                    variant = *(long *)(*(byte **)(model + 0x54) + variant_index * 0x38);
                }
                dword eligible = 0;
                long last = NONE;
                real weight = 0.0f;
                for (long j = 0; j < choice_count; ++j)
                {
                    s_colour_choice_ab *choice = &entry->choices[j];
                    if (!choice->field_1c_4 || choice->field_1c_4 == variant)
                    {
                        weight += choice->weight;
                        last = j;
                        eligible |= 1 << j;
                    }
                }
                if (eligible && weight > 0.0f)
                {
                    real target = selection * weight;
                    real cumulative = 0.0f;
                    for (long j = 0; j < choice_count; ++j)
                    {
                        s_colour_choice_ab *choice = &entry->choices[j];
                        if (eligible & (1 << j))
                        {
                            cumulative += choice->weight;
                            if (j == last || cumulative >= target)
                            {
                                hsv3f a, b, mixed;
                                real inverse = 1.0f - blend;
                                function_1318d0(&choice->lower, &a);
                                function_1318d0(&choice->upper, &b);
                                if (fabs(a.hue - b.hue) > 0.5f)
                                {
                                    if (b.hue > a.hue) a.hue += 1.0f;
                                    else b.hue += 1.0f;
                                }
                                mixed.hue = a.hue * inverse + b.hue * blend;
                                if (mixed.hue > 1.0f) mixed.hue -= 1.0f;
                                mixed.saturation = a.saturation * inverse + b.saturation * blend;
                                mixed.value = a.value * inverse + b.value * blend;
                                function_131a00(&mixed, &output[i]);
                                break;
                            }
                        }
                    }
                }
            }
        }
        output[i].red = output[i].red < 0.0f ? 0.0f : output[i].red > 1.0f ? 1.0f : output[i].red;
        output[i].green = output[i].green < 0.0f ? 0.0f : output[i].green > 1.0f ? 1.0f : output[i].green;
        output[i].blue = output[i].blue < 0.0f ? 0.0f : output[i].blue > 1.0f ? 1.0f : output[i].blue;
        output[i + count] = output[i];
    }
    function_3dd10(object_index, true);
}

bool __stdcall function_bab40(long object_index, long name, real *value);

// @retail 0xbe8b0
real __stdcall function_be8b0(long object_index, long name)
{
    real value;
    if (!function_bab40(object_index, name, &value))
        value = 0.0f;
    return value;
}

// @retail 0xbe6d0
real function_be6d0(long object_index, long attachment_index)
{
    byte *object = (byte *)((s_object_header *)g_4e0300->data)[object_index & 0xffff].object;
    byte *definition = g_4e3b44[*(long *)object & 0xffff].bytes;
    byte *attachment = *(byte **)(definition + 0x98) + attachment_index * 24;
    real value = 1.0f;
    long name = *(long *)(attachment + 0x10);
    if (name)
        function_bab40(object_index, name, &value);
    if ((bool)(((dword)*(word *)(definition + 2) >> 9) & 1))
        return *(real *)(object + 0xa0) * value;
    return value;
}

struct s_colour_function_ab
{
    long unused;
    dword flags;
    color3f lower;
    color3f upper;
    long scale_name;
    long blend_name;
};
color3f *function_131c20(color3f const *a, color3f const *b, dword flags, real t, color3f *result);

// @retail 0xbe8e0
bool __stdcall function_be8e0(long object_index)
{
    bool result = false;
    byte *object = (byte *)((s_object_header *)g_4e0300->data)[object_index & 0xffff].object;
    byte *definition = g_4e3b44[*(long *)object & 0xffff].bytes;
    if (definition[0x1c] & 1)
    {
        long count = (long)((dword)(long)*(short *)(object + 0x124) / 12) / 2;
        color3f *base = (color3f *)(object + *(short *)(object + 0x126));
        color3f *current = base + count;
        if (count > 0)
        {
            result = true;
            for (long i = 0; i < count; ++i)
            {
                s_colour_choices_ab *entry = &(*(s_colour_choices_ab **)(definition + 0xb0))[i];
                current[i] = base[i];
                for (long j = 0; j < entry->function_count; ++j)
                {
                    s_colour_function_ab *function = &entry->functions[j];
                    if (function->blend_name)
                    {
                        real value;
                        if (!function_bab40(object_index, function->blend_name, &value)) value = 0.0f;
                        function_131c20(&function->lower, &function->upper, function->flags, value, &current[i]);
                    }
                }
                for (long j = 0; j < entry->function_count; ++j)
                {
                    s_colour_function_ab *function = &entry->functions[j];
                    if (function->scale_name)
                    {
                        real value;
                        if (!function_bab40(object_index, function->scale_name, &value)) value = 0.0f;
                        current[i].red = value * current[i].red;
                        current[i].green *= value;
                        current[i].blue *= value;
                    }
                }
                current[i].red = current[i].red < 0.0f ? 0.0f : current[i].red > 1.0f ? 1.0f : current[i].red;
                current[i].green = current[i].green < 0.0f ? 0.0f : current[i].green > 1.0f ? 1.0f : current[i].green;
                current[i].blue = current[i].blue < 0.0f ? 0.0f : current[i].blue > 1.0f ? 1.0f : current[i].blue;
            }
        }
    }
    return result;
}
