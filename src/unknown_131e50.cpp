// @flags /O2 /Gr /arch:SSE
/* UNKNOWN_131E50.CPP: real color to pixel32 conversion */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"

#ifdef H2_NATIVE_PORT
PRIVATE __forceinline dword color_channel_byte(real value)
{
	real const scaled = value * 255.0f;
	long result = (long)scaled;
	real const fraction = scaled - (real)result;
	if (fraction > 0.5f || (fraction == 0.5f && (result & 1)))
		++result;
	return (dword)result & 0xff;
}
#endif

// @retail 0x131e50
dword __cdecl pack_color4f(const color4f *color)
{
#ifdef H2_NATIVE_PORT
	return color_channel_byte(color->blue) |
		(color_channel_byte(color->green) << 8) |
		(color_channel_byte(color->red) << 16) |
		(color_channel_byte(color->alpha) << 24);
#else
	long b, g, r, a;
	real scale = 255.0f;
	dword pixel = 0;

	__asm
	{
		mov edx, color
		fld dword ptr [edx]
		fld dword ptr [edx + 4]
		fld dword ptr [edx + 8]
		fld dword ptr [edx + 12]
		fld scale
		fmul st(4), st(0)
		fmul st(3), st(0)
		fmul st(2), st(0)
		fmulp st(1), st(0)
		fistp b
		fistp g
		fistp r
		fistp a
		mov edx, b
		mov ebx, g
		mov ecx, r
		mov eax, a
		shl ebx, 8
		shl ecx, 0x10
		shl eax, 0x18
		or edx, ebx
		or edx, ecx
		or edx, eax
		mov pixel, edx
	}
	return pixel;
#endif
}

// @retail 0x131ed0
dword __cdecl pack_color3f(const color3f *color)
{
#ifdef H2_NATIVE_PORT
	return color_channel_byte(color->blue) |
		(color_channel_byte(color->green) << 8) |
		(color_channel_byte(color->red) << 16);
#else
	real scale = 255.0f;
	dword pixel = 0;

	__asm
	{
		mov edx, color
		fld dword ptr [edx]
		fld dword ptr [edx + 4]
		fld dword ptr [edx + 8]
		fld scale
		fmul st(3), st(0)
		fmul st(2), st(0)
		fmulp st(1), st(0)
		fistp pixel
		and pixel, 0xff
		mov edx, pixel
		fistp pixel
		and pixel, 0xff
		shl pixel, 8
		or edx, pixel
		fistp pixel
		and pixel, 0xff
		shl pixel, 0x10
		or edx, pixel
		mov pixel, edx
	}
	return pixel;
#endif
}

// @retail 0x131f40
dword __cdecl function_131f40(real alpha, const color3f *color)
{
#ifdef H2_NATIVE_PORT
	return color_channel_byte(color->blue) |
		(color_channel_byte(color->green) << 8) |
		(color_channel_byte(color->red) << 16) |
		(color_channel_byte(alpha) << 24);
#else
	real scale = 255.0f;
	dword pixel = 0;

	__asm
	{
		mov edx, color
		fld alpha
		fld dword ptr [edx]
		fld dword ptr [edx + 4]
		fld dword ptr [edx + 8]
		fld scale
		fmul st(4), st(0)
		fmul st(3), st(0)
		fmul st(2), st(0)
		fmulp st(1), st(0)
		fistp pixel
		and pixel, 0xff
		mov edx, pixel
		fistp pixel
		and pixel, 0xff
		shl pixel, 8
		or edx, pixel
		fistp pixel
		and pixel, 0xff
		shl pixel, 0x10
		or edx, pixel
		fistp pixel
		shl pixel, 0x18
		or edx, pixel
		mov pixel, edx
	}
	return pixel;
#endif
}

// @retail 0x131fc0
dword __cdecl function_131fc0(real alpha)
{
#ifdef H2_NATIVE_PORT
	return color_channel_byte(alpha) << 24;
#else
	real scale = 255.0f;
	dword pixel = 0;

	__asm
	{
		fld alpha
		fld scale
		fmulp st(1), st(0)
		fistp pixel
		shl pixel, 0x18
	}
	return pixel;
#endif
}
