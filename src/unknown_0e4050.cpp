// @flags /O2 /Ob1 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "globals.h"
#include "engine_peer.h"
#include "effects.h"
#include "slot_handler.h"
#include <math.h>

struct s_e4050_object
{
	byte unknown00[0x14];
	long parent_index;
	byte unknown18[0xa9];
	byte flag_c1_0 : 1;
	byte unknownc1 : 7;
	byte unknownc2[0x286];
	byte flag_348_0 : 1;
	byte unknown348 : 7;
	byte unknown349[0x50];
	char value_399;
	byte unknown39a[0x42];
	byte state_3dc;
};

struct s_e4050_object_header
{
	byte unknown00[8];
	s_e4050_object *object;
};

static __forceinline long real_to_long(real value);

// @retail 0xe4050
bool function_e4050(long object_index)
{
	s_e4050_object *object = ((s_e4050_object_header *)g_4e0300->data)[object_index & 0xffff].object;

	return !TEST_FIELD_BIT(object->flag_c1_0) && TEST_FIELD_BIT(object->flag_348_0) &&
		object->value_399 >= real_to_long(g_510c54->field_2_3 * 0.18f) &&
		(object->state_3dc == 1 || object->state_3dc == 3) &&
		object->parent_index == NONE;
}

/* inline keeps it out of the callers' register tracking: retail's callers
   treat every scratch register (edx, the xmm registers) as clobbered by it
   (0x21e230 keeps `this` in esi across the call); noinline keeps the call */
inline __declspec(noinline) real function_12aff0(real a, real b, real c, bool flag);

// Round 19: with the outcome in a long (so the compare is at full width, as in retail) the edge
// path is written as a ternary, an if with two returns, an if/else with two returns, the same
// with the != polarity, the edge path in an else block of the clamp, and with int returns that
// MSVC folds to the same floats; all score 10. MSVC if-converts the two edge returns into one
// shared exit and hoists the materialised value before the compare, so retail's two separate
// returns are not reproduced. Dropping noinline from the declaration leaves the body unchanged.
// The word outcome and body are restored.
// score 8 (was 8): retail compares (a > c) with flag at full width and gives the edge path two
// separate returns after the clamp's exit; ours compares words and loads 0.0 before the test (select).
// Tried: (a > c) == flag in a ternary, an if with two returns, a long local, != with swapped
// constants 10 each; bool local 17; one result for both paths 10; the near-edge test first
// with a ternary 30, with two returns 27 (retail's exact edge and clamp blocks, laid out in the
// other order); fabs(a - b) < 0.0001f first 27; the clamp result declared first with the edge
// returns in the else 10.
// Retail full-width edge comparison and separate returns remain; native signed/unsigned
// outcome 10/10, signed word 9; false-first/full-arm outcome 13/10, zero-first real result 10, explicit edge
// returns 10, real truth return 24. Original word outcome and body restored.
// Round 202: independent clamp bounds 8; native bound returns 34; upper-first bounds 13;
// existing shared real clamp with word/full/direct edge outcomes 8/10/10, edge-first 27,
// one real result 35. All bodies and helper declarations restored.
// Round 207: native byte flag, word/signed/direct/explicit edge returns 8/10/10/10;
// comparison-first 20 (AL input, five caller losses), flag-first 33 (same five losses);
// edge-first/unsigned outcome 27/10; double difference 9 (qword epsilon load, broad match losses).
// Original Boolean contract and float producer restored; full-width independent edge exits remain.
// Round 48: word outcome with two returns 8; signed word with two returns 9; inverted word returns 8;
// eager word outcome 49; entry-declared/late-assigned word 8; shared real clamp with word returns 8,
// full-width returns 10 and direct full-width returns 10. Original body and declarations restored.
// @retail 0x12aff0
real function_12aff0(real a, real b, real c, bool flag)
{
	if (!(0.0001f > fabs(a - b)))
	{
		real t = (c - a) / (b - a);
		if (t < 0.0f)
			t = 0.0f;
		else if (t > 1.0f)
			t = 1.0f;
		return t;
	}

	word above = a > c;
	return above == flag ? 0.0f : 1.0f;
}

/* the particle system objects at 0x479868 and 0x479874, picked by the group
   of a tag (c_type_4e7709 is in effects.h) */
c_type_4e7709 g_479868;
c_type_4e7709 g_479874;

// @retail 0x137bd0
c_type_4e7709 *function_137bd0(long tag_index)
{
	return particle_object_for_tag(tag_index);
}

// @retail 0x15e020
bool function_15e020(short a, short b)
{
	bool result = false;
	c_engine_peer *engine = g_55e4d0[g_4e9ae8->engine_index];

	if (engine)
		result = engine->p27(a, b);
	return result;
}
