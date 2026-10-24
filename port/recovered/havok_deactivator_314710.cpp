// @flags /O2 /GL-
// Havok (third-party physics library): the spatial deactivator an entity of
// deactivation kind 2 keeps (0x311690 builds it), the settings block it reads
// and writes, and a world object's listener list.
//
//   the deactivator (0x60 bytes, vtable 0x414abc)
//     0x314710  constructor: thresholds, reference poses unset, and the
//               entity's size from its shape's box (0x314650)
//     slot 5 0x3144b0  high-frequency check: the entity stayed within a small
//                      distance and turn of the pose last kept
//     slot 6 0x314580  the same, low frequency, with its own pose and limits
//   its settings (s_314320, 0x14 bytes): constructor 0x314320 (base
//   0x31bc60); 0x314450 reads a deactivator's into one, 0x314480 writes them
//   0x315910  a world object's listener appended to its list at 0x3c
//
// The deactivator's vtable is its retail one, stored by address.
#include "growable_array_2d91d0.h"

class hkEntity;

// gives back the object it is passed (src/unknown_31bc70.cpp)
void *__fastcall function_31bc70(void *arg_0);

struct alignas(16) vector_314710
{
	float x, y, z, w;
};

// the identity transform (retail data at 0x461ea0)
extern float const g_461ea0[16];

// a box given by its two corners
struct bounds_314650
{
	vector_314710 minimum;
	vector_314710 maximum;
};

// an entity's shape as the deactivator sees it
class shape_314650
{
public:
	virtual void slot_0() = 0;
	virtual void slot_1() = 0;
	virtual void slot_2() = 0;
	virtual void slot_3() = 0;
	virtual void slot_4() = 0;
	virtual void slot_5() = 0;
	// the shape's bounds under a transform, widened by a tolerance
	virtual void bounds(void const *transform, float tolerance, bounds_314650 *out) const = 0;
};

// an entity's motion: its rotation (a quaternion) and position
struct motion_3144b0
{
	char unknown0[0x30];
	vector_314710 rotation;
	char unknown40[0xb0 - 0x40];
	vector_314710 position;
};

struct entity_3144b0
{
	char unknown0[0xc];
	shape_314650 *shape;
	char unknown10[0x3c - 0x10];
	motion_3144b0 *motion;
};

// a returned hkBool
struct s_1d1870
{
	char field_0;
	s_1d1870() {}
	s_1d1870(bool arg_0) : field_0(arg_0) {}
};

// ---------------------------------------------------------------------------
// the deactivator

class c_314710
{
public:
	c_314710(hkEntity *arg_0);
	s_1d1870 function_3144b0(void *arg_0);
	s_1d1870 function_314580(void *arg_0);
	float function_314650(shape_314650 *shape);

	void const *vtable;
	unsigned short memory_size;
	unsigned short reference_count;
	// the square of half the shape's box's diagonal (-1 without a shape)
	float size_squared;
	long unknownc;
	vector_314710 high_position;
	vector_314710 high_rotation;
	vector_314710 low_position;
	vector_314710 low_rotation;
	float high_distance;
	float high_turn;
	float low_distance;
	float low_turn;
};

typedef char assert_c_314710_size[sizeof(c_314710) == 0x60 ? 1 : -1];

// @retail 0x314710
c_314710::c_314710(hkEntity *arg_0)
{
	reference_count = 1;
	vtable = (void const *)0x414abc;
	high_distance = 0.01f; // 0x3c23d70a
	high_turn = 0.005f; // 0x3ba3d70a
	low_distance = 0.1f; // 0x3dcccccd
	low_turn = 0.2f; // 0x3e4ccccd
	float const largest = 3.40282347e+38f; // 0x7f7fffff
	high_position.x = high_position.y = high_position.z = high_position.w = largest;
	low_position.x = low_position.y = low_position.z = low_position.w = largest;
	high_rotation.x = high_rotation.y = high_rotation.z = 0.0f;
	high_rotation.w = 1.0f;
	low_rotation.x = low_rotation.y = low_rotation.z = 0.0f;
	low_rotation.w = 1.0f;
	function_314650(((entity_3144b0 *)arg_0)->shape);
}

// @retail 0x314650
float c_314710::function_314650(shape_314650 *shape)
{
	if (!shape)
	{
		size_squared = -1.0f;
		return size_squared;
	}
	bounds_314650 box;
	shape->bounds(g_461ea0, 0.0f, &box);
	float const x = 0.5f * (box.maximum.x - box.minimum.x);
	float const y = 0.5f * (box.maximum.y - box.minimum.y);
	float const z = 0.5f * (box.maximum.z - box.minimum.z);
	size_squared = (y * y + x * x) + z * z;
	return size_squared;
}

// true when the entity has moved less than `distance` from the kept
// position and turned less than `turn` (scaled by its size) from the kept
// rotation; otherwise its pose is kept and false returned. The squares of
// the limits are taken exactly, as retail's x87 code does.
static inline bool still_3144b0(c_314710 *deactivator, entity_3144b0 *entity, vector_314710 *position,
	vector_314710 *rotation, float distance, float turn)
{
	motion_3144b0 *motion = entity->motion;
	float const dx = position->x - motion->position.x;
	float const dy = position->y - motion->position.y;
	float const dz = position->z - motion->position.z;
	float const moved = (dy * dy + dx * dx) + dz * dz;
	vector_314710 const current = motion->rotation;
	if ((double)moved < (double)distance * (double)distance)
	{
		float const qx = rotation->x - current.x;
		float const qy = rotation->y - current.y;
		float const qz = rotation->z - current.z;
		float const qw = rotation->w - current.w;
		float const turned = (qz * qz + qx * qx) + (qw * qw + qy * qy);
		if ((double)turn * (double)turn > (double)turned * (double)deactivator->size_squared)
		{
			return true;
		}
	}
	*rotation = current;
	*position = entity->motion->position;
	return false;
}

// @retail 0x3144b0
s_1d1870 c_314710::function_3144b0(void *arg_0)
{
	return s_1d1870(still_3144b0(this, (entity_3144b0 *)arg_0, &high_position, &high_rotation, high_distance, high_turn));
}

// @retail 0x314580
s_1d1870 c_314710::function_314580(void *arg_0)
{
	return s_1d1870(still_3144b0(this, (entity_3144b0 *)arg_0, &low_position, &low_rotation, low_distance, low_turn));
}

// ---------------------------------------------------------------------------
// the settings

struct base_31bc60
{
	base_31bc60();
};

// @retail 0x31bc60
base_31bc60::base_31bc60()
{
	function_31bc70(this);
}

struct s_314320 : base_31bc60
{
	float size_squared;
	float high_distance;
	float high_turn;
	float low_distance;
	float low_turn;

	s_314320();
};

// @retail 0x314320
s_314320::s_314320()
{
	size_squared = -1.0f; // 0xbf800000
	high_distance = 0.01f; // 0x3c23d70a
	high_turn = 0.005f; // 0x3ba3d70a
	low_distance = 0.1f; // 0x3dcccccd
	low_turn = 0.2f; // 0x3e4ccccd
}

// called on the deactivator
struct s_314450
{
	void function_314450(s_314320 *arg_0);
	void function_314480(s_314320 const *arg_0);
};

// @retail 0x314450
void s_314450::function_314450(s_314320 *arg_0)
{
	c_314710 *deactivator = (c_314710 *)this;
	arg_0->high_distance = deactivator->high_distance;
	arg_0->high_turn = deactivator->high_turn;
	arg_0->low_distance = deactivator->low_distance;
	arg_0->low_turn = deactivator->low_turn;
	arg_0->size_squared = deactivator->size_squared;
}

// @retail 0x314480
void s_314450::function_314480(s_314320 const *arg_0)
{
	c_314710 *deactivator = (c_314710 *)this;
	deactivator->high_distance = arg_0->high_distance;
	deactivator->high_turn = arg_0->high_turn;
	deactivator->low_distance = arg_0->low_distance;
	deactivator->low_turn = arg_0->low_turn;
	deactivator->size_squared = arg_0->size_squared;
}

// ---------------------------------------------------------------------------
// a world object's listeners

ELEMENTS_2D91D0(listeners_315910, void *);

class c_2784c5
{
public:
	void function_315910(void *arg_0);

	char unknown0[0x3c];
	listeners_315910 listeners;
};

// @retail 0x315910
void c_2784c5::function_315910(void *arg_0)
{
	listeners.append_copy(arg_0);
}
