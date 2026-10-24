#ifndef CONVEX_SEARCH_3048A0_H
#define CONVEX_SEARCH_3048A0_H
// Havok (third-party physics library): the search for the closest points of
// two convex shapes, run from a pair's search state, with its working state
// kept on the caller's stack.
#include <xmmintrin.h>

class convex_2fefd0;
struct c_query_transform;
struct pair_state_2fefd0;

// the closest point found, its distance in the fourth lane
struct closest_3048a0
{
	__m128 point;
};

class class_3048a0
{
public:
	char unknown0[0xc];
	bool field_c;
#if H2_WHOLE_IMAGE
	/* the triangle test (0x303360) stores its three edge dots as a whole
	   vector at 0x160, its unused fourth lane at 0x16c: retail's frames leave
	   those four bytes free above the object, where the port's compiler puts
	   the caller's next local (the query transform in query_kept_2ee610,
	   whose first axis' x it overwrote, so every kept contact's point on the
	   second shape came out wrong and was dropped at once) */
	char unknownd[0x170 - 0xd];
#else
	char unknownd[0x16c - 0xd];
#endif

	// a status: nonzero passes the pair on without testing the distance
	int function_3048a0(convex_2fefd0 const *first, convex_2fefd0 const *second, c_query_transform const *transform, pair_state_2fefd0 *state, closest_3048a0 *closest);
};

#endif
