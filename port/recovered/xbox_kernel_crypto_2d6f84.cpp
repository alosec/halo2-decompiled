// @flags /O2 /GL-
// The Xbox kernel's SHA-1 and HMAC exports, which retail reaches through
// import thunks (jmp [import]) that the XDK libraries' signature code calls:
//   0x2d6f90  XcSHAInit (kernel ordinal 335): a hash context started
//   0x2d6f8a  XcSHAUpdate (336): bytes added to the hash
//   0x2d6f84  XcSHAFinal (337): the 20-byte digest of what was added
//   0x2d6f96  XcHMAC (340): the HMAC-SHA1 of two pieces of data under a key
//             (at most 64 bytes of it are used)
// The kernel's context is 0x74 bytes (XapipCalculateContentSignatureBegin
// keeps one at +8 of a 0x7c-byte block); only these functions read it, and
// the state kept here fits in it.
//
// Not recovered: the thunk at 0x2d6f9c, ExQueryNonVolatileSetting (ordinal
// 24), which reads the console's EEPROM settings; the XDK calls that use it
// (XGetLanguage, XGetVideoFlags, ...) are the port runtime's own.
#include <string.h>

struct sha_context_2d6f84
{
	unsigned long state[5];
	unsigned long count_low;  // bytes added, low word
	unsigned long count_high; // bytes added, high word
	unsigned char buffer[64];
};

typedef char assert_sha_context_2d6f84[(sizeof(sha_context_2d6f84) <= 0x74) ? 1 : -1];

static __forceinline unsigned long rotate_2d6f84(unsigned long value, int bits)
{
	return ((value << bits) | (value >> (32 - bits))) & 0xffffffff;
}

// one 64-byte block into the state
static void sha_transform_2d6f84(unsigned long state[5], unsigned char const block[64])
{
	unsigned long w[80];
	for (int i = 0; i < 16; i++)
	{
		w[i] = ((unsigned long)block[i * 4] << 24) | ((unsigned long)block[i * 4 + 1] << 16) |
			((unsigned long)block[i * 4 + 2] << 8) | (unsigned long)block[i * 4 + 3];
	}
	for (int i = 16; i < 80; i++)
	{
		w[i] = rotate_2d6f84(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1);
	}
	unsigned long a = state[0];
	unsigned long b = state[1];
	unsigned long c = state[2];
	unsigned long d = state[3];
	unsigned long e = state[4];
	for (int i = 0; i < 80; i++)
	{
		unsigned long f;
		unsigned long k;
		if (i < 20)
		{
			f = (b & c) | (~b & d);
			k = 0x5a827999;
		}
		else if (i < 40)
		{
			f = b ^ c ^ d;
			k = 0x6ed9eba1;
		}
		else if (i < 60)
		{
			f = (b & c) | (b & d) | (c & d);
			k = 0x8f1bbcdc;
		}
		else
		{
			f = b ^ c ^ d;
			k = 0xca62c1d6;
		}
		unsigned long temp = (rotate_2d6f84(a, 5) + f + e + k + w[i]) & 0xffffffff;
		e = d;
		d = c;
		c = rotate_2d6f84(b, 30);
		b = a;
		a = temp;
	}
	state[0] = (state[0] + a) & 0xffffffff;
	state[1] = (state[1] + b) & 0xffffffff;
	state[2] = (state[2] + c) & 0xffffffff;
	state[3] = (state[3] + d) & 0xffffffff;
	state[4] = (state[4] + e) & 0xffffffff;
}

// @retail 0x2d6f90
extern "C" void __stdcall xc_sha_init_2d6f90(unsigned char *context)
{
	sha_context_2d6f84 *sha = (sha_context_2d6f84 *)context;
	sha->state[0] = 0x67452301;
	sha->state[1] = 0xefcdab89;
	sha->state[2] = 0x98badcfe;
	sha->state[3] = 0x10325476;
	sha->state[4] = 0xc3d2e1f0;
	sha->count_low = 0;
	sha->count_high = 0;
}

// @retail 0x2d6f8a
extern "C" void __stdcall xc_sha_update_2d6f8a(unsigned char *context, unsigned char const *input, unsigned long length)
{
	sha_context_2d6f84 *sha = (sha_context_2d6f84 *)context;
	unsigned long used = sha->count_low & 63;
	unsigned long low = (sha->count_low + length) & 0xffffffff;
	if (low < sha->count_low)
	{
		sha->count_high++;
	}
	sha->count_low = low;
	while (length)
	{
		unsigned long take = 64 - used;
		if (take > length)
		{
			take = length;
		}
		memcpy(sha->buffer + used, input, take);
		used += take;
		input += take;
		length -= take;
		if (used == 64)
		{
			sha_transform_2d6f84(sha->state, sha->buffer);
			used = 0;
		}
	}
}

// @retail 0x2d6f84
extern "C" void __stdcall xc_sha_final_2d6f84(unsigned char *context, unsigned char *digest)
{
	sha_context_2d6f84 *sha = (sha_context_2d6f84 *)context;
	unsigned long high = (sha->count_high << 3) | (sha->count_low >> 29);
	unsigned long low = (sha->count_low << 3) & 0xffffffff;
	unsigned char length[8];
	for (int i = 0; i < 4; i++)
	{
		length[i] = (unsigned char)(high >> (24 - 8 * i));
		length[4 + i] = (unsigned char)(low >> (24 - 8 * i));
	}
	static unsigned char const padding[64] = {0x80};
	unsigned long used = sha->count_low & 63;
	xc_sha_update_2d6f8a(context, padding, used < 56 ? 56 - used : 120 - used);
	xc_sha_update_2d6f8a(context, length, 8);
	for (int i = 0; i < 5; i++)
	{
		digest[i * 4] = (unsigned char)(sha->state[i] >> 24);
		digest[i * 4 + 1] = (unsigned char)(sha->state[i] >> 16);
		digest[i * 4 + 2] = (unsigned char)(sha->state[i] >> 8);
		digest[i * 4 + 3] = (unsigned char)sha->state[i];
	}
}

// @retail 0x2d6f96
extern "C" void __stdcall xc_hmac_2d6f96(unsigned char const *key, unsigned long key_length, unsigned char const *data,
	unsigned long data_length, unsigned char const *data2, unsigned long data2_length, unsigned char *digest)
{
	if (key_length > 64)
	{
		key_length = 64;
	}
	unsigned char inner[64];
	unsigned char outer[64 + 20];
	memset(inner, 0, sizeof(inner));
	memset(outer, 0, sizeof(outer));
	memcpy(inner, key, key_length);
	memcpy(outer, key, key_length);
	for (int i = 0; i < 64; i++)
	{
		inner[i] ^= 0x36;
		outer[i] ^= 0x5c;
	}

	unsigned long words[0x74 / 4];
	unsigned char *context = (unsigned char *)words;
	xc_sha_init_2d6f90(context);
	xc_sha_update_2d6f8a(context, inner, sizeof(inner));
	if (data)
	{
		xc_sha_update_2d6f8a(context, data, data_length);
	}
	if (data2)
	{
		xc_sha_update_2d6f8a(context, data2, data2_length);
	}
	xc_sha_final_2d6f84(context, outer + 64);

	xc_sha_init_2d6f90(context);
	xc_sha_update_2d6f8a(context, outer, sizeof(outer));
	xc_sha_final_2d6f84(context, digest);
}
