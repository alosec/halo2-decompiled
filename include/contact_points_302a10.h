#ifndef CONTACT_POINTS_302A10_H
#define CONTACT_POINTS_302A10_H
// Havok (third-party physics library): a pair's kept contact points, at most
// four, each a point in the first body's frame, a point in the second's with
// the contact's id in its fourth term, and the normal; and the owner that
// hands the ids out and takes them back.
#include <xmmintrin.h>

struct point_302a10
{
	__m128 position;
	union
	{
		__m128 normal;
		struct
		{
			float normal_xyz[3];
			unsigned short id;
		};
		struct
		{
			float other_xyz[3];
			unsigned long id_and_flags;
		};
	};
	__m128 field_20;
};

// a contact handed to the owner: its point, with the separation in the
// fourth term, and its normal
struct contact_302a10
{
	__m128 position;
	__m128 normal;
	char unknown20[0x10];
};

class owner_302a10
{
public:
	virtual void slot_0() {}
	virtual unsigned short add_contact(void const *a, void const *b, void const *input, contact_302a10 *contact) { return 0; }
	virtual void release_id(unsigned short id) {}
};

// cuts five points down to four, handing the dropped one's id back; returns
// the count left
int __cdecl function_302a10(point_302a10 **points, void *arg_1, void *arg_2, owner_302a10 *owner);

#endif
