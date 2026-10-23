// @flags /O2 /GL-
// Havok (third-party physics library): shifting every endpoint of a sorted
// endpoint list by an offset given in world units: the offset is turned into
// whole even steps per axis (and the offset those steps amount to is passed
// back), and an endpoint pushed past either end of the range is clamped
// there, its object noted in a list.
#include "growable_array_2d91d0.h"
#include <xmmintrin.h>

struct endpoint_2e1640
{
	unsigned short value;
	unsigned short id;
};

struct axis_2e1640
{
	ELEMENTS_2D91D0_MEMBERS(endpoint_2e1640)
};

struct object_2e1640
{
	long field_0;
	long field_4;
	long field_8;
	void *field_c;
};

ELEMENTS_2D91D0(pointers_2e1640, void *);

// a float to an integer in the current rounding mode
__forceinline long round_2e1640(float value)
{
	long result;
	__asm
	{
		fld value
		fistp result
	}
	return result;
}

class class_2e1640
{
public:
	char unknown0[0x30];
	float scale[4];
	object_2e1640 *objects;
	long object_count;
	long object_capacity;
	axis_2e1640 axes[3];

	void function_2e1640(__m128 const *offset, __m128 *applied, pointers_2e1640 *clamped);
};

// @retail 0x2e1640
void class_2e1640::function_2e1640(__m128 const *offset, __m128 *applied, pointers_2e1640 *clamped)
{
	__declspec(align(16)) float shifted[4];
	*(__m128 *)shifted = *offset;
	long steps[3];
	long k;
	for (k = 0; k < 3; k++)
	{
		float const *axis_scale = &scale[k];
		long const step = round_2e1640(shifted[k] * *axis_scale) & ~1;
		steps[k] = step;
		shifted[k] = (float)steps[k] / *axis_scale;
	}
	*applied = *(__m128 *)shifted;
	for (k = 0; k < 3; k++)
	{
		long step = steps[k];
		axis_2e1640 *axis = &axes[k];
		for (long j = 1; j < axis->size - 1; j++)
		{
			endpoint_2e1640 *endpoint = &axis->data[j];
			long value = endpoint->value;
			if (value > 1 && value < 0xfffc)
			{
				long moved = step + value;
				if (moved < 0)
				{
					moved = value & 1;
					clamped->append_copy(objects[endpoint->id].field_c);
				}
				else if (moved >= 0xfffc)
				{
					moved = (value & 1) | 0xfffc;
					clamped->append_copy(objects[endpoint->id].field_c);
				}
				endpoint->value = (unsigned short)moved;
			}
		}
	}
}
