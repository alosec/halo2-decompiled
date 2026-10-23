// @flags /O2 /GL- /GX
// Havok (third-party physics library): a list of child objects' queries with
// a collector of their own: one that stops at the first hit, one that keeps
// the nearest distance, and one that flips the vectors the children added.
#include <xmmintrin.h>

static __declspec(align(16)) const unsigned long g_4124a0[4] = {0x80000000, 0x80000000, 0x80000000, 0x80000000};

struct description_2eda30;

struct collector_2edb60
{
	virtual ~collector_2edb60() {}

	bool done;

	collector_2edb60() : done(false) {}
};

class first_hit_2eded0 : public collector_2edb60
{
public:
	virtual ~first_hit_2eded0() {}

	long target;

	first_hit_2eded0(long target_) : target(target_) {}
};

class nearest_base_2edf30
{
public:
	virtual ~nearest_base_2edf30() {}

	float distance;

	nearest_base_2edf30() : distance(3.40282347e+38f) {}
};

class nearest_2edf30 : public nearest_base_2edf30
{
public:
	virtual ~nearest_2edf30() {}

	long target;

	nearest_2edf30(long target_) : target(target_) {}
};

struct hit_2edf90
{
	char field_0[0x10];
	__m128 normal;
	char field_20[0x10];
};

struct hits_2edf90
{
	hit_2edf90 *data;
	long count;
};

class list_2eda30
{
public:
	char unknown0[0xc];
	void *children;
	long count;

	void function_2eda30(description_2eda30 const *description, long arg_1, long arg_2, long arg_3);
	void function_2eda90(description_2eda30 const *description, long arg_1, long arg_2, long arg_3);
	void function_2edb60(description_2eda30 const *description, long arg_1, long arg_2, collector_2edb60 *collector);
	void function_2eded0(long arg_1, description_2eda30 const *description, long arg_2, long arg_3);
	void function_2edf30(long arg_1, description_2eda30 const *description, long arg_2, long arg_3);
	void function_2edf90(long arg_1, description_2eda30 const *description, long arg_2, hits_2edf90 **hits);
};

// @retail 0x2eded0
void list_2eda30::function_2eded0(long arg_1, description_2eda30 const *description, long arg_2, long arg_3)
{
	first_hit_2eded0 collector(arg_3);
	function_2edb60(description, arg_1, arg_2, &collector);
}

// @retail 0x2edf30
void list_2eda30::function_2edf30(long arg_1, description_2eda30 const *description, long arg_2, long arg_3)
{
	nearest_2edf30 nearest(arg_3);
	function_2eda90(description, arg_1, arg_2, (long)&nearest);
}

// @retail 0x2edf90
void list_2eda30::function_2edf90(long arg_1, description_2eda30 const *description, long arg_2, hits_2edf90 **hits)
{
	long first = (*hits)->count;
	function_2eda30(description, arg_1, arg_2, (long)hits);
	for (; first < (*hits)->count; first++)
	{
		__m128 &normal = (*hits)->data[first].normal;
		normal = _mm_xor_ps(normal, *(const __m128 *)g_4124a0);
	}
}
