// @flags /O2 /GL- /GX
// Havok (third-party physics library): the query of 0x2ec020 run with the two
// sides the other way round: the query's direction is flipped, and the
// collectors (the second one optional) are wrapped in ones that keep the
// original direction.
#include <xmmintrin.h>

static __declspec(align(16)) const unsigned long g_4124a0[4] = {0x80000000, 0x80000000, 0x80000000, 0x80000000};

struct body_2ebc80;
struct input_2ebe40;

void __cdecl function_2ec020(body_2ebc80 const *a, body_2ebc80 const *b, input_2ebe40 const *input, void *arg_3, void *arg_4);

// the query's head (its dispatcher and filter, then two more values), its
// direction and two values after it
struct head_2ec580
{
	long field_0;
	long field_4;
	long field_8;
	long field_c;
};

struct query_2ec580
{
	head_2ec580 head;
	__m128 direction;
	long field_20;
	long field_24;
};

// a collector that keeps the nearest result, starting from the largest float
class nearest_collector_2ec580
{
public:
	nearest_collector_2ec580() : distance(3.40282347e+38f) {}
	virtual ~nearest_collector_2ec580() {}

	float distance;
};

// passes each result on to another collector with the query's original
// direction
class flipped_collector_2ec580 : public nearest_collector_2ec580
{
public:
	flipped_collector_2ec580(__m128 const &direction, void *target) : direction(direction), target(target) {}
	virtual ~flipped_collector_2ec580() {}

	__m128 direction;
	void *target;
};

// @retail 0x2ec580
void __cdecl function_2ec580(body_2ebc80 const *b, body_2ebc80 const *a, query_2ec580 const *query, void *collector, void *second_collector)
{
	query_2ec580 flipped;
	flipped.head = query->head;
	flipped.field_20 = query->field_20;
	flipped.field_24 = query->field_24;
	flipped.direction = _mm_xor_ps(query->direction, *(const __m128 *)g_4124a0);
	flipped_collector_2ec580 first(query->direction, collector);
	if (second_collector)
	{
		flipped_collector_2ec580 second(query->direction, second_collector);
		function_2ec020(a, b, (input_2ebe40 const *)&flipped, &first, &second);
	}
	else
	{
		function_2ec020(a, b, (input_2ebe40 const *)&flipped, &first, 0);
	}
}
