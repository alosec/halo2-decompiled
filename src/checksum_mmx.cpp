// @flags /O2 /arch:SSE /Gr
/* CHECKSUM_MMX.CPP: an MMX exclusive-or checksum of a buffer, 8 bytes at a
   time (written in assembly, as retail's prefetchnta and MMX code shows). */

#include "unknown_11c920.h"

#pragma warning(disable: 4035)

/* the exclusive-or of a buffer's 8-byte words, folded to 32 bits */
// @retail 0x123110
dword __stdcall checksum_buffer_mmx(void const *buffer, dword buffer_size)
{
	byte seed[8];

	__asm
	{
		mov byte ptr seed[0], 0x01
		mov byte ptr seed[1], 0x02
		mov byte ptr seed[2], 0x04
		mov byte ptr seed[3], 0x08
		mov byte ptr seed[4], 0x10
		mov byte ptr seed[5], 0x20
		mov byte ptr seed[6], 0x40
		mov byte ptr seed[7], 0x80
		mov ecx, buffer_size
		shr ecx, 3
		mov ebx, buffer
		pxor mm2, mm2
		test ecx, 1
		je no_seed
		movq mm2, seed
	no_seed:
		mov eax, 0
		test ecx, ecx
		je done
	next:
		prefetchnta byte ptr [ebx + ecx * 8 - 0x108]
		pxor mm2, qword ptr [ebx + ecx * 8 - 8]
		dec ecx
		jne next
	done:
		movq mm0, mm2
		punpckhdq mm0, mm0
		pxor mm0, mm2
		movd eax, mm0
		emms
	}
}

#pragma warning(default: 4035)
