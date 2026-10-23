// @flags /O2 /GL-
// Havok (third-party physics library): a ray cast against a container of
// parts: every part (or every part a filter on the ray accepts) is cast
// against, and each hit records the part's key; bracketed by two marks in the
// timing stream.
#include "timer_mark_2ebc80.h"

// the one-byte answer of a test, returned through a hidden pointer
class answer_2fbb70
{
public:
	answer_2fbb70() {}

	char value;
};

class container_2fbb70;
struct ray_2fbb70;

// what a cast fills in; the key of the part hit at +0x10
struct hit_2fbb70
{
	char unknown0[0x10];
	long key;
};

// whether one part of a container may be cast against
class ray_filter_2fbb70
{
public:
	virtual answer_2fbb70 accepts(ray_2fbb70 const *ray, container_2fbb70 const *container, long key) const = 0;
};

struct ray_2fbb70
{
	char unknown0[0x24];
	ray_filter_2fbb70 *filter;
};

class part_2fbb70
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void slot_4() {}
	virtual void slot_5() {}
	virtual void slot_6() {}
	virtual void slot_7() {}
	// a ray cast against the part
	virtual answer_2fbb70 cast_ray(ray_2fbb70 const *ray, hit_2fbb70 *hit) const { return answer_2fbb70(); }
};

class container_2fbb70
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void slot_4() {}
	virtual void slot_5() {}
	virtual void slot_6() {}
	virtual void slot_7() {}
	virtual void slot_8() {}
	virtual void slot_9() {}
	virtual void slot_10() {}
	// the part keys, ending with -1
	virtual long first_key() const { return -1; }
	virtual long next_key(long key) const { return -1; }
	// a part, built in the buffer given when it must be
	virtual part_2fbb70 const *part(long key, char *buffer) const { return 0; }

	answer_2fbb70 function_2fbb70(ray_2fbb70 const *ray, hit_2fbb70 *hit) const;
};

// @retail 0x2fbb70
answer_2fbb70 container_2fbb70::function_2fbb70(ray_2fbb70 const *ray, hit_2fbb70 *hit) const
{
	timer_add_2ebc80("TthkShapeCollection::castRay");
	__declspec(align(16)) char buffer[0x100];
	answer_2fbb70 result;
	result.value = 0;
	if (ray->filter == 0)
	{
		for (long key = first_key(); key != -1; key = next_key(key))
		{
			if (part(key, buffer)->cast_ray(ray, hit).value)
			{
				result.value = 1;
				hit->key = key;
			}
		}
	}
	else
	{
		for (long key = first_key(); key != -1; key = next_key(key))
		{
			if (ray->filter->accepts(ray, this, key).value)
			{
				if (part(key, buffer)->cast_ray(ray, hit).value)
				{
					result.value = 1;
					hit->key = key;
				}
			}
		}
	}
	timer_add_2ebc80("Et");
	return result;
}
