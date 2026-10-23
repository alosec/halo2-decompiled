// @flags /O2 /GL-
// Havok (third-party physics library): adds a point mass's contribution to
// a 3x3 inertia tensor stored in rows of four floats; and combines the mass
// properties of several placed parts into one: total volume and mass, the
// mass-weighted centre, and the inertia tensor of every part turned into the
// common frame and shifted to the common centre.
#include <xmmintrin.h>

// @retail 0x2db9e0
void __cdecl function_2db9e0(float const *point, float mass, float *inertia)
{
	inertia[0] += (point[1] * point[1] + point[2] * point[2]) * mass;
	inertia[5] += (point[0] * point[0] + point[2] * point[2]) * mass;
	inertia[10] += (point[0] * point[0] + point[1] * point[1]) * mass;
	inertia[1] -= mass * point[0] * point[1];
	inertia[4] = inertia[1];
	inertia[6] -= mass * point[2] * point[1];
	inertia[9] = inertia[6];
	inertia[8] -= mass * point[0] * point[2];
	inertia[2] = inertia[8];
}

// the 3x3 rotation part of a transform, as the matched callee declares it
struct hkRotation;

// a point transformed by a matrix (0x2da610)
struct c_transformed_point
{
	__m128 value;
	void transform(const void *matrix, const __m128 *point);
};

// a 3x3 matrix turned by a rotation: R * M * R^T (0x2da9a0)
struct s_2da8c0
{
	__m128 columns[3];
	void function_2da9a0(hkRotation const *rotation);
};

// one 3x4 matrix added to another (0x2da880)
class class_2da880
{
public:
	__m128 rows[3];

	void add(const class_2da880 &other);
};

struct s_physics_mass
{
	float volume;
	float mass;
	char unknown08[8];
	__m128 center;
	__m128 inertia[3];
};

struct s_physics_mass_entry
{
	s_physics_mass mass;
	__m128 rotation[3];
	__m128 translation;
};

struct s_physics_mass_array
{
	s_physics_mass_entry *data;
	long size;
	long capacity;
};

static inline __m128 splat_2dba80(float value)
{
	__m128 single = _mm_set_ss(value);
	return _mm_shuffle_ps(single, single, 0);
}

// 1 when the parts have no positive total mass, otherwise 0 with the result
// filled in. (The point-mass function above must stay ahead of it in this
// file: compiled first, it lets the compiler order the stores and pushes
// around that call as retail does; on its own this scored 5.)
// @retail 0x2dba80
long __cdecl function_2dba80(s_physics_mass_array const *array, s_physics_mass *result)
{
	__m128 center = _mm_setzero_ps();
	float total_mass = 0.0f;
	float total_volume = 0.0f;
	for (int index = 0; index < array->size; index++)
	{
		c_transformed_point centre_in_world;
		centre_in_world.transform(array->data[index].rotation, &array->data[index].mass.center);
		center = _mm_add_ps(center, _mm_mul_ps(splat_2dba80(array->data[index].mass.mass), centre_in_world.value));
		total_mass += array->data[index].mass.mass;
		total_volume += array->data[index].mass.volume;
	}
	if (total_mass <= 0.0f)
	{
		return 1;
	}
	center = _mm_mul_ps(splat_2dba80(1.0f / total_mass), center);
	result->center = center;
	result->mass = total_mass;
	result->volume = total_volume;
	class_2da880 *sum = (class_2da880 *)result->inertia;
	sum->rows[0] = _mm_setzero_ps();
	sum->rows[1] = _mm_setzero_ps();
	sum->rows[2] = _mm_setzero_ps();
	for (int part = 0; part < array->size; part++)
	{
		s_2da8c0 inertia;
		inertia.columns[0] = array->data[part].mass.inertia[0];
		inertia.columns[1] = array->data[part].mass.inertia[1];
		inertia.columns[2] = array->data[part].mass.inertia[2];
		inertia.function_2da9a0((hkRotation const *)array->data[part].rotation);
		c_transformed_point offset;
		offset.transform(array->data[part].rotation, &array->data[part].mass.center);
		offset.value = _mm_sub_ps(offset.value, center);
		function_2db9e0((float const *)&offset, array->data[part].mass.mass, (float *)&inertia);
		sum->add(*(class_2da880 const *)&inertia);
	}
	return 0;
}
