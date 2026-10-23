// @flags /O2 /GL-
// Havok (third-party physics library): casting a ray against a shape library:
// the cast is an object built on the stack (0x2df670 clears it), run over the
// shape and the tree the library keeps at +0xc and +0x10.
#include <xmmintrin.h>

struct s_244980_ray;
struct s_244980_output;
class c_244c10;

// whether a cast hit, given back by value
struct result_2df6e0
{
	result_2df6e0() {}

	bool hit;
};

struct class_2df670
{
	__m128 unknown0[4];
	long field_40;
	long field_44;
	char unknown48[0x50 - 0x48];
	__m128 unknown50[2];

	class_2df670();
	result_2df6e0 function_301720(void const *shape, void const *tree, void const *ray, void *output);
	void function_301800(void const *shape, void const *tree, s_244980_ray const *ray, s_244980_output *output, c_244c10 *callback);
};

struct c_shape_library_base_a
{
	char unknown0[0xc];
	void const *shape;
	void const *tree;

	result_2df6e0 function_2df6e0(void const *ray, void *output);
	void function_2df720(s_244980_ray const *ray, s_244980_output *output, c_244c10 *callback);
};

// @retail 0x2df6e0
result_2df6e0 c_shape_library_base_a::function_2df6e0(void const *ray, void *output)
{
	class_2df670 cast;
	return cast.function_301720(shape, tree, ray, output);
}

// @retail 0x2df720
void c_shape_library_base_a::function_2df720(s_244980_ray const *ray, s_244980_output *output, c_244c10 *callback)
{
	class_2df670 cast;
	cast.function_301800(shape, tree, ray, output, callback);
}
