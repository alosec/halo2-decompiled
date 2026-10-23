// @flags /O2 /GL-
// Havok (third-party physics library): whether a direction, brought into an
// object's frame once and cached, and then back out, stays within a small
// angle of the original (a cosine under 0.98 reports true); entries with
// fewer than two uses always report true.
#include <xmmintrin.h>

struct bool_305a60
{
	bool value;

	bool_305a60(bool value_)
	{
		value = value_;
	}
};

struct c_query_transform
{
	__m128 axes[3];
	__m128 position;
};

struct vector_2da6e0
{
	__m128 value;
	void inverse_rotate(const c_query_transform *matrix, const __m128 *direction);
};

struct entry_305a60
{
	__m128 direction;
	vector_2da6e0 local;
	char unknown20[0x34];
	long uses;
	char unknown58;
	bool local_valid;
};

class class_305a60
{
public:
	char unknown0[0x70];
	c_query_transform transform;

	bool_305a60 function_305a60(entry_305a60 *entry);
};

// @retail 0x305a60
bool_305a60 class_305a60::function_305a60(entry_305a60 *entry)
{
	if (entry->uses < 2)
	{
		entry->local_valid = false;
		return bool_305a60(true);
	}
	if (!entry->local_valid)
	{
		entry->local.inverse_rotate(&transform, &entry->direction);
		entry->local_valid = true;
	}
	__m128 p = entry->local.value;
	__m128 z = _mm_mul_ps(transform.axes[2], _mm_shuffle_ps(p, p, 0xaa));
	__m128 y = _mm_mul_ps(transform.axes[1], _mm_shuffle_ps(p, p, 0x55));
	__m128 x = _mm_mul_ps(transform.axes[0], _mm_shuffle_ps(p, p, 0x00));
	__m128 back = _mm_add_ps(_mm_add_ps(x, y), z);
	__m128 d = _mm_mul_ps(entry->direction, back);
	float dot;
	_mm_store_ss(&dot, _mm_add_ss(_mm_shuffle_ps(d, d, 0xaa), _mm_add_ss(_mm_shuffle_ps(d, d, 0x55), d)));
	return bool_305a60(dot < 0.98f);
}
