// @flags /O2 /GL-
// Havok (third-party physics library): boxes held as 16-bit integer corners,
// each word moved one step for each of two limits it passes.
#include <mmintrin.h>

// a box's corner words: lower x, lower y, upper x, upper y, then lower z,
// upper z and two unused words, read as two 64-bit halves
struct __declspec(align(16)) quantized_box_2dfcc0
{
	unsigned short words[8];

	__m64 &xy() { return *(__m64 *)&words[0]; }
	__m64 &z() { return *(__m64 *)&words[4]; }
};

class quantized_boxes_2dfcc0
{
public:
	void function_2dfcc0(quantized_box_2dfcc0 *boxes, long count, unsigned short const *limits);
	void function_2dfdc0(quantized_box_2dfcc0 *boxes, long count, unsigned short const *limits);
};

// each corner word gets one added for each limit it passes: the limit less
// one and the limit less two
// @retail 0x2dfcc0
void quantized_boxes_2dfcc0::function_2dfcc0(quantized_box_2dfcc0 *boxes, long count, unsigned short const *limits)
{
	__m64 mask;
	quantized_box_2dfcc0 low;
	quantized_box_2dfcc0 high;
	low.words[5] = low.words[4] = limits[4] - 1;
	low.words[2] = low.words[0] = limits[0] - 1;
	low.words[3] = low.words[1] = limits[1] - 1;
	high.words[5] = high.words[4] = limits[5] - 2;
	high.words[2] = high.words[0] = limits[2] - 2;
	high.words[3] = high.words[1] = limits[3] - 2;
	for (quantized_box_2dfcc0 *box = boxes; box < boxes + count; box++)
	{
		mask.m64_u16[0] = 0xffff;
		mask.m64_u16[1] = 0xffff;
		mask.m64_u16[2] = 0;
		mask.m64_u16[3] = 0;
		__m64 const z = box->z();
		__m64 const z_steps = _mm_and_si64(_mm_add_pi16(_mm_cmpgt_pi16(z, low.z()), _mm_cmpgt_pi16(z, high.z())), mask);
		__m64 const xy = box->xy();
		box->xy() = _mm_sub_pi16(xy, _mm_add_pi16(_mm_cmpgt_pi16(xy, low.xy()), _mm_cmpgt_pi16(xy, high.xy())));
		box->z() = _mm_sub_pi16(box->z(), z_steps);
	}
	_mm_empty();
}

// each corner word gets one taken off for each limit it passes: the limit
// for the lower corner, and the one for the upper
// @retail 0x2dfdc0
void quantized_boxes_2dfcc0::function_2dfdc0(quantized_box_2dfcc0 *boxes, long count, unsigned short const *limits)
{
	__m64 mask;
	quantized_box_2dfcc0 low;
	quantized_box_2dfcc0 high;
	low.words[5] = low.words[4] = limits[4];
	low.words[2] = low.words[0] = limits[0];
	low.words[3] = low.words[1] = limits[1];
	high.words[5] = high.words[4] = limits[5];
	high.words[2] = high.words[0] = limits[2];
	high.words[3] = high.words[1] = limits[3];
	*(long *)&low.words[6] = -1;
	*(long *)&high.words[6] = -1;
	for (quantized_box_2dfcc0 *box = boxes; box < boxes + count; box++)
	{
		mask.m64_u16[0] = 0xffff;
		mask.m64_u16[1] = 0xffff;
		mask.m64_u16[2] = 0;
		mask.m64_u16[3] = 0;
		__m64 const z = box->z();
		__m64 const z_steps = _mm_and_si64(_mm_add_pi16(_mm_cmpgt_pi16(z, low.z()), _mm_cmpgt_pi16(z, high.z())), mask);
		__m64 const xy = box->xy();
		box->xy() = _mm_add_pi16(xy, _mm_add_pi16(_mm_cmpgt_pi16(xy, low.xy()), _mm_cmpgt_pi16(xy, high.xy())));
		box->z() = _mm_add_pi16(box->z(), z_steps);
	}
	_mm_empty();
}
