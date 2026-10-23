// @flags /O2 /GL-
// Havok (third-party physics library): setting a value to the smallest of
// three in a description.

struct info_319ab0
{
	float first;
	char unknown4[0x10];
	float second;
	char unknown18[0x10];
	float third;
};

static inline float minimum_319ab0(float a, float b)
{
	return a < b ? a : b;
}

class class_319ab0
{
public:
	char unknown0[0x10];
	float field_10;

	void function_319ab0(info_319ab0 const *info);
};

// @retail 0x319ab0
void class_319ab0::function_319ab0(info_319ab0 const *info)
{
	field_10 = minimum_319ab0(minimum_319ab0(info->first, info->second), info->third);
}
