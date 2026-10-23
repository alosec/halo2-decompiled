// @flags /O2 /GL- /GX
// Havok (third-party physics library): a list of child objects' query run
// the other way round: the query's direction is flipped, and the collectors
// (the second one optional) are wrapped in ones that keep the original
// direction.
#include <xmmintrin.h>

static __declspec(align(16)) const unsigned long g_4124a0[4] = {0x80000000, 0x80000000, 0x80000000, 0x80000000};

struct description_2eda30;

struct head_2ee0a0
{
	long field_0;
	long field_4;
	long field_8;
	long field_c;
};

struct query_2ee0a0
{
	head_2ee0a0 head;
	__m128 direction;
	long field_20;
	long field_24;
};

class nearest_base_2ee0a0
{
public:
	virtual ~nearest_base_2ee0a0() {}
	float distance;
	nearest_base_2ee0a0() : distance(3.40282347e+38f) {}
};

class flipped_2ee0a0 : public nearest_base_2ee0a0
{
public:
	virtual ~flipped_2ee0a0() {}
	__m128 direction;
	long target;
	flipped_2ee0a0(__m128 const &direction_, long target_) : direction(direction_), target(target_) {}
};

class list_2eda30
{
public:
	void function_2edaf0(description_2eda30 const *description, long arg_1, long arg_2, long arg_3, long arg_4);
	void function_2ee0a0(long arg_1, description_2eda30 const *description, query_2ee0a0 const *query, long arg_3, long arg_4);
};

// @retail 0x2ee0a0
void list_2eda30::function_2ee0a0(long arg_1, description_2eda30 const *description, query_2ee0a0 const *query, long arg_3, long arg_4)
{
	query_2ee0a0 flipped;
	flipped.head = query->head;
	flipped.field_20 = query->field_20;
	flipped.field_24 = query->field_24;
	flipped.direction = _mm_xor_ps(query->direction, *(const __m128 *)g_4124a0);
	flipped_2ee0a0 first(query->direction, arg_3);
	if (arg_4)
	{
		flipped_2ee0a0 second(query->direction, arg_4);
		function_2edaf0(description, arg_1, (long)&flipped, (long)&first, (long)&second);
	}
	else
	{
		function_2edaf0(description, arg_1, (long)&flipped, (long)&first, 0);
	}
}
