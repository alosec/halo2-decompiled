// @flags /O2 /GL-
// zlib 1.1.4 (third-party compression library, linked into the game): the
// default allocator (zutil.c) and the Adler-32 checksum (adler32.c):
//   0x2ce780  zcalloc: calloc(items, size)
//   0x2ce7a0  zcfree: free(ptr)
//   0x2ce7b0  adler32
// All three keep the stack (they are reached through function pointers:
// z_stream's zalloc/zfree, inflate_blocks' check function) and pop their
// arguments. crc32 is not in the image.

extern "C" void *calloc(unsigned int count, unsigned int size);
extern "C" void free(void *block);

// @retail 0x2ce780
void *__stdcall zcalloc_2ce780(void *opaque, unsigned int items, unsigned int size)
{
	if (opaque)
	{
		items += size - size; // make compiler happy
	}
	return calloc(items, size);
}

// @retail 0x2ce7a0
void __stdcall zcfree_2ce7a0(void *opaque, void *address)
{
	free(address);
	if (opaque)
	{
		return; // make compiler happy
	}
}

enum
{
	BASE_2CE7B0 = 65521, // largest prime smaller than 65536
	// the largest n such that 255n(n+1)/2 + (n+1)(BASE-1) <= 2^32-1
	NMAX_2CE7B0 = 5552
};

#define DO1_2CE7B0(buf, i) {s1 += buf[i]; s2 += s1;}
#define DO2_2CE7B0(buf, i) DO1_2CE7B0(buf, i); DO1_2CE7B0(buf, i + 1);
#define DO4_2CE7B0(buf, i) DO2_2CE7B0(buf, i); DO2_2CE7B0(buf, i + 2);
#define DO8_2CE7B0(buf, i) DO4_2CE7B0(buf, i); DO4_2CE7B0(buf, i + 4);
#define DO16_2CE7B0(buf) DO8_2CE7B0(buf, 0); DO8_2CE7B0(buf, 8);

// @retail 0x2ce7b0
unsigned long __stdcall adler32_2ce7b0(unsigned long adler, unsigned char const *buf, unsigned int len)
{
	unsigned long s1 = adler & 0xffff;
	unsigned long s2 = (adler >> 16) & 0xffff;
	int k;

	if (buf == 0)
	{
		return 1L;
	}

	while (len > 0)
	{
		k = len < NMAX_2CE7B0 ? (int)len : NMAX_2CE7B0;
		len -= (unsigned int)k;
		while (k >= 16)
		{
			DO16_2CE7B0(buf);
			buf += 16;
			k -= 16;
		}
		if (k != 0)
		{
			do
			{
				s1 += *buf++;
				s2 += s1;
			} while (--k);
		}
		s1 %= BASE_2CE7B0;
		s2 %= BASE_2CE7B0;
	}
	return (s2 << 16) | s1;
}
