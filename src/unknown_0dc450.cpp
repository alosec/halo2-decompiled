// @flags /O2 /arch:SSE /Gr
#include <math.h>
#include "unknown_11c920.h"
#include "globals.h"

/* Callbacks in the table at 0x467a88, called with an object index (or a
   pointer) on the stack. */

struct s_dc_object
{
	long definition_index;
	byte unknown04[0x14 - 4];
	long l14;
	byte unknown18[4];
	real r1c;
	real r20;
	real r24;
	byte unknown28[0x34 - 0x28];
	real r34;
	real r38;
	real r3c;
	byte unknown40[0x88 - 0x40];
	real vx;
	real vy;
	real vz;
	byte unknown94[0x34b - 0x94];
	byte b34b;
	byte unknown34c[4];
	long l350;
	long l354;
	long l358;
	long l35c;
	long l360;
	long l364;
	byte unknown368[0x390 - 0x368];
	long l390;
	byte unknown394[0x39d - 0x394];
	byte b39d;
	byte unknown39e[0x3dc - 0x39e];
	byte b3dc;
};

struct s_dc_header
{
	byte unknown00[8];
	s_dc_object *object;
};

struct s_dc_tag
{
	byte unknown00[0x264];
	union
	{
		byte flags_byte;
		struct
		{
			dword flag0 : 1;
			dword flag1 : 1;
			dword flag2 : 1;
			dword flag3 : 1;
			dword flag4 : 1;
		};
	};
	byte unknown268[0x270 - 0x268];
	real r270;
	byte unknown274[0x2dc - 0x274];
	real r2dc;
};

struct s_dc_options
{
	byte unknown00[0x1130];
	long l1130;
};


real function_30bf0(vector3f *v);

static inline s_dc_object *dc_object_get(long index)
{
	return ((s_dc_header *)g_4e0300->data)[index & 0xffff].object;
}

static inline s_dc_tag *dc_tag_get(s_dc_object *object)
{
	return (s_dc_tag *)g_4e3b44[object->definition_index & 0xffff].bytes;
}

// @retail 0xdc450
void __stdcall function_dc450(long index)
{
	s_dc_object *object = dc_object_get(index);

	object->l354 = NONE;
	object->l358 = NONE;
	object->l35c = NONE;
	object->l360 = NONE;
	object->l364 = NONE;
	object->l350 = NONE;
}

// @retail 0xdc4c0
void __stdcall function_dc4c0(s_dc_object *object)
{
	s_dc_tag *tag = dc_tag_get(object);

	if ((tag->flags_byte & 1) && !TEST_FIELD_BIT(tag->flag4))
	{
		real scale = tag->r270;

		object->r1c = object->r34 * scale + object->r1c;
		object->r20 = object->r38 * scale + object->r20;
		object->r24 = object->r3c * scale + object->r24;
	}
}

// @retail 0xdc550
void __stdcall function_dc550(long index)
{
	s_dc_object *object = dc_object_get(index);

	if (object->b34b == 1)
		((s_dc_options *)g_4e6948)->l1130--;
}

// @retail 0xdc580
void __stdcall function_dc580(long index, long value)
{
	s_dc_object *object = dc_object_get(index);

	if (object->l390 == value)
		object->l390 = NONE;
}

// @retail 0xde3b0
bool __stdcall function_de3b0(long index, long code, real *out, bool *flag)
{
	s_dc_object *object = dc_object_get(index);
	s_dc_tag *tag = dc_tag_get(object);
	bool result = false;
	real value = 0.0f;
	real clamped;

	switch (code)
	{
	case 0x6000542:
		if (object->b3dc == 2)
			value = 1.0f;
		goto clamp;
	case 0x70000c9:
		if (object->b39d > 0)
		{
			value = 1.0f;
			result = true;
			clamped = value;
			goto store;
		}
		break;
	case 0xc0005a4:
		if (tag->r2dc > 0.0f)
		{
			value = (real)sqrt(object->vx * object->vx + object->vy * object->vy + object->vz * object->vz) / tag->r2dc;
clamp:
			result = true;
			if (value < 0.0f)
				clamped = 0.0f;
			else if (value > 1.0f)
				clamped = 1.0f;
			else
				clamped = value;
store:
			*out = clamped;
			*flag = value > 0.0f;
		}
		break;
	}
	return result;
}

// @retail 0xdffa0
void __stdcall function_dffa0(long index, long unused, vector3f *a, vector3f *b)
{
	s_dc_object *object = dc_object_get(index);

	if (object->l14 == NONE)
	{
		s_dc_tag *tag = dc_tag_get(object);

		if (!TEST_FIELD_BIT(tag->flag3) && object->b3dc != 3)
		{
			if (b)
			{
				if (b->k != g_4687b0->k)
					*b = *g_4687b0;
			}
			if (a)
			{
				if (a->k != 0.0f)
				{
					a->k = 0.0f;
					if (function_30bf0(a) == g_45dbd8)
						*a = *g_4687a8;
				}
			}
		}
	}
}

/* the callbacks of the table at 0x467a88 that are decompiled (the others, and
   the shared empty handlers, belong to other files) */
void *g_467a88[29] =
{
	0, 0, 0, 0, 0,
	(void *)function_dc450, (void *)function_dc4c0, 0, 0, 0,
	0, (void *)function_dc550, 0, 0, 0,
	(void *)function_de3b0, 0, 0, (void *)function_dc580, 0,
	0, 0, (void *)function_dffa0, 0, 0,
	0, 0, 0, 0
};
