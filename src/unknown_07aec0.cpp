// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_07AEC0.CPP: network addresses */

#include "unknown_11c920.h"
#include <string.h>

/* Bungie's macro, as in Halo CE's cseries.h */
#ifndef MIN
#define MIN(a,b) ((a)>(b)?(b):(a))
#endif

enum
{
	k_ipv4_address_length = 4,
	k_ipv6_address_length = 16
};

struct s_type_99af70
{
	union
	{
		dword ipv4_address;
		word ipv6_address[8];
	};
	word port;
	short address_length;
};

bool function_7af40(s_type_99af70 const *address);

/* an IPv4 address in host byte order whose last octet is zero */
// score 26 (was 55): retail ends with "if (!(byte)output) result = true;" (test cl, cl; jne;
// mov al, 1), but that form moves the address parameter from ecx to edx (score 14 here, and
// callers 0x784a0 and 0x7ab60 stop matching), so the sete form keeps the callers' convention.
// Tried (with the if-form tail): an outer if (address) with a short length local (inlines
// into 0x7acc0), "address &&" before the call (calls 0x7af40, 86), byte_swap_long() helper
// (16), a value local (16), nested ifs / sizeof(dword) / (long) cast / reference before
// result (all 14), early return true (48), "result = true; if ((byte)*ipv4_address) result = false;" (23, but the
// tail is mov al, 1 / je / xor al, al), (output & 0xff) == 0 (28), swapped parameter order (14).
// @retail 0x7aec0
bool function_07aec0(s_type_99af70 const *address, dword *ipv4_address)
{
	dword &output = *ipv4_address;
	bool result = false;

	if (function_7af40(address) && address->address_length == k_ipv4_address_length)
	{
		output = (((address->ipv4_address & 0xff0000) | (address->ipv4_address >> 16)) >> 8) | (((address->ipv4_address << 16) | (address->ipv4_address & 0xff00)) << 8);
		result = !(byte)output;
	}
	return result;
}

// @retail 0x7af40
bool function_7af40(s_type_99af70 const *address)
{
	bool result = false;

	if (address)
	{
		switch (address->address_length)
		{
		case NONE:
		case k_ipv4_address_length:
			result = address->ipv4_address != 0;
			break;
		case k_ipv6_address_length:
			for (long i = 0; i < 8; i++)
			{
				if (address->ipv6_address[i])
					return true;
			}
			break;
		}
	}
	return result;
}

// @retail 0x7af80
bool function_7af80(s_type_99af70 const *a, s_type_99af70 const *b, bool compare_ports)
{
	short length = MIN(a->address_length, b->address_length);

	if (a->address_length > 0 && a->address_length == b->address_length && memcmp(a, b, length) == 0 &&
		(!compare_ports || a->port == b->port))
		return true;
	return false;
}
