// @flags /O2 /GL-
// Havok (third-party physics library): dropping the contact points whose
// normals have turned away from a manifold's normal, then refreshing the
// manifold's normal in the second body's frame.
#include <xmmintrin.h>

struct c_query_transform
{
	__m128 axes[3];
	__m128 position;
};

struct vector_2da6e0
{
	__m128 value;
	void rotate(const c_query_transform *matrix, const __m128 *direction);
	void inverse_rotate(const c_query_transform *matrix, const __m128 *direction);
};

struct s_308180_entry
{
	unsigned short point_slot;
	unsigned short second;
	unsigned short point_id;
};

// a contact manifold: its normal in world space and in the second body's
// frame, and up to eight entries naming its points
class class_308180
{
public:
	__m128 normal;
	vector_2da6e0 local_normal;
	s_308180_entry entries[8];
	long field_50;
	int count;
	bool changed;
	bool has_normal;

	void remove(int index, long a, long b);
};

// a vector copied as a whole
struct vector_307530
{
	__m128 value;

	vector_307530 &operator=(vector_307530 const &other)
	{
		value = other.value;
		return *this;
	}
};

// a contact point: its position, its normal and an id
struct contact_point_307530
{
	vector_307530 position;
	vector_307530 normal;
	unsigned short id;
	char unknown22[0xe];
};

struct contact_points_307530
{
	contact_point_307530 *data;
	int size;
};

class point_listener_307530
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void point_removed(unsigned short point_id) {}
};

class class_307530
{
public:
	long pair_first;
	long pair_second;
	char unknown8[4];
	point_listener_307530 *listener;
	contact_points_307530 **points;
	c_query_transform const *transform;
	char unknown18[0x58];
	c_query_transform second_transform;

	void function_307530(class_308180 *contacts);
};

// @retail 0x307530
void class_307530::function_307530(class_308180 *contacts)
{
	vector_2da6e0 manifold_normal;
	manifold_normal.rotate(transform, &contacts->normal);
	int const first_point = (*points)->size - contacts->count;
	int point_index = first_point;
	float alignment;
	for (int i = 0; i < contacts->count;)
	{
		__m128 const product = _mm_mul_ps((*points)->data[point_index].normal.value, manifold_normal.value);
		_mm_store_ss(&alignment, _mm_add_ss(_mm_shuffle_ps(product, product, 0xaa), _mm_add_ss(_mm_shuffle_ps(product, product, 0x55), product)));
		if (alignment < 0.95f)
		{
			(*points)->data[point_index] = (*points)->data[(*points)->size - 1];
			if (contacts->entries[i].point_slot <= 5)
			{
				contacts->field_50--;
			}
			listener->point_removed(contacts->entries[i].point_id);
			contacts->remove(i, pair_first, pair_second);
			(*points)->size--;
			continue;
		}
		i++;
		point_index++;
	}
	if (contacts->count < 2)
	{
		contacts->has_normal = false;
	}
	else
	{
		contacts->local_normal.inverse_rotate(&second_transform, &contacts->normal);
		contacts->has_normal = true;
	}
}
