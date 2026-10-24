// @flags /O2 /GL-
// Havok (third-party physics library): the rest of the six joint constraints
// port/recovered/havok_joints_312920.cpp builds (their vtables' slots 5, 7,
// 9 and 10, and the writers that take a snapshot back in), the class
// objects' functions for each of them (and for the spatial deactivator),
// the constraint base's destructor and setters, and the contact constraint's
// destructor and the two calls that turn it into a contact modifier.
//
//   the joints' vtables (slot 0 the deleting destructor 0x3142c0, whose
//   destructor is the base's: 0x314b40 -> 0x31b9e0)
//     kind 2 hkRagdollConstraint       0x4146fc  slot 5 0x312a20, 7 0x312bf0, 9 0x312fe0 (10 0x312b30)
//     kind 1 hkLimitedHingeConstraint  0x414860  slot 5 0x313410, 9 0x313810, 10 0x313520
//     kind 0 hkHingeConstraint         0x41490c  slot 5 0x313c40, 9 0x313ef0, 10 0x313c90
//     kind 4 hkBallAndSocketConstraint 0x41498c  slot 5 0x3141b0, 9 0x314220, 10 0x3141f0
//     kind 3 hkStiffSpringConstraint   0x414b4c  slot 5 0x314950, 7 0x314960, 9 0x3149e0, 10 0x3149b0
//     kind 5 hkPrismaticConstraint     0x414ca4  slot 5 0x314cc0, 9 0x314fc0, 10 0x314dd0
//   slot 5 gives the joint's runtime records, slot 7 whether it is valid,
//   slot 9 builds its Jacobians (port/recovered/havok_jacobian_builders_
//   31e740.cpp), slot 10 the solver room they take.
//
//   the class objects' function tables (0x2d9750 builds the class objects at
//   0x480658.., the tables at 0x4145f8, 0x4147c8, 0x4148e0, 0x414958,
//   0x414a08, 0x414b0c, 0x414c08): create from a snapshot, construct a
//   snapshot in place, (0x175f40), read a constraint into a snapshot, write
//   a snapshot into a constraint
//
//   the constraint base (vtable 0x415360, its atom at 8 vtable 0x415350)
//     0x31b960  takes a snapshot's priority
//     0x31b990  the atom's slot 2: tells the owner
//     0x31b9b0  the atom's destructor
//     0x31b9e0  destructor: lets both entities go
//   the contact constraint (vtable 0x4153c8)
//     0x315510  destructor: its three arrays freed, then the base's
//     0x3156c0  the modifiers' destructor (the same)
//     0x315340  makes a contact constraint the mass changer (0x480770's)
//     0x3153c0  makes it the surface velocity (0x480880's)
//
// The vtables are the retail ones, stored by address; the joints' own
// constructors, readers and setters (havok_joints_312920.cpp) and the
// builders are reached through h2_retail_<address>.
#include <xmmintrin.h>
#include "reference_counted_2d88a0.h"

// ---------------------------------------------------------------------------
// recovered code this file calls

// the joints' constructors from a snapshot, the snapshots' constructors,
// the readers (a joint into a snapshot) and the setters (a snapshot into a
// joint) (port/recovered/havok_joints_312920.cpp)
extern "C" void *h2_retail_312d50(void *joint, void const *snapshot);
extern "C" void *h2_retail_313550(void *joint, void const *snapshot);
extern "C" void *h2_retail_313de0(void *joint, void const *snapshot);
extern "C" void *h2_retail_314270(void *joint, void const *snapshot);
extern "C" void *h2_retail_314b00(void *joint, void const *snapshot);
extern "C" void *h2_retail_314ef0(void *joint, void const *snapshot);
extern "C" void *h2_retail_312920(void *snapshot);
extern "C" void *h2_retail_313320(void *snapshot);
extern "C" void *h2_retail_314bb0(void *snapshot);
extern "C" void *h2_retail_31b950(void *snapshot);
extern "C" void h2_retail_312ab0(void *joint, void *snapshot);
extern "C" void h2_retail_3134a0(void *joint, void *snapshot);
extern "C" void h2_retail_313c50(void *joint, void *snapshot);
extern "C" void h2_retail_3141c0(void *joint, void *snapshot);
extern "C" void h2_retail_31b970(void *joint, void *snapshot);
extern "C" void h2_retail_312a30(void *joint, void const *snapshot);
extern "C" void h2_retail_313420(void *joint, void const *snapshot);
extern "C" void h2_retail_314cd0(void *joint, void const *snapshot);

// the spatial deactivator's settings: base constructor, read and write
// (port/recovered/havok_deactivator_314710.cpp)
extern "C" void *h2_retail_31bc60(void *settings);
extern "C" void h2_retail_314450(void *deactivator, void *settings);
extern "C" void h2_retail_314480(void *deactivator, void const *settings);

// the Jacobian builders (port/recovered/havok_jacobian_builders_31e740.cpp)
extern "C" void h2_retail_31e770(void const *query, void *writer, void *records, long record_size);
extern "C" void h2_retail_31e740(float value, void const *query, void *writer, long back);
extern "C" void h2_retail_31e7b0(void const *info, void const *query, void *writer);
extern "C" void h2_retail_31e900(void const *info, void const *query, void *writer);
extern "C" void h2_retail_31f000(void const *info, void const *query, void *writer);
extern "C" void h2_retail_31ea90(void const *info, void const *query, void *writer);
extern "C" void h2_retail_31ec30(void const *info, void const *query, void *writer);
extern "C" void h2_retail_31ee10(void const *info, void const *query, void *writer);
extern "C" void h2_retail_31f160(void const *transform_a, void const *transform_b, __m128 const *pivot_a,
	__m128 const *pivot_b, void const *query, void *writer);
extern "C" void h2_retail_31f3e0(__m128 const *point_a, __m128 const *point_b, void const *query, void *writer);
extern "C" float h2_retail_3725e0(float y, float x);

// an entity dropping a constraint from its list (src/unknown_30bc60.cpp),
// whether an address lies in the loaded cache file (src/unknown_279740.cpp),
// and the reference base's destructor (src/unknown_0a9f20.cpp)
extern "C" void h2_retail_30bcc0(void *entity, void *constraint);
extern "C" bool h2_retail_279740(long address);
extern "C" void h2_retail_dc370(void *object);

// ---------------------------------------------------------------------------
// shared pieces

// FLT_EPSILON (0x34000000 at 0x4146e4)
static float const epsilon_3128d0 = 1.1920929e-07f;
// 1e-5 (0x3727c5ac at 0x4122dc)
static float const unit_tolerance_3128d0 = 9.99999975e-06f;
static float const pi_3128d0 = 3.14159274f; // 0x41485c
static float const two_pi_3128d0 = 6.28318548f; // 0x4123e8

static __declspec(align(16)) const unsigned long sign_mask_3128d0[4] = {0x80000000, 0x80000000, 0x80000000, 0x80000000};

static inline __m128 splat_3128d0(float value)
{
	__m128 const single = _mm_set_ss(value);
	return _mm_shuffle_ps(single, single, 0);
}

static inline float lane_3128d0(__m128 const &v, int index)
{
	return ((float const *)&v)[index];
}

// (y + x) + z
static inline float dot3_3128d0(__m128 const &a, __m128 const &b)
{
	__m128 const product = _mm_mul_ps(a, b);
	return (lane_3128d0(product, 1) + lane_3128d0(product, 0)) + lane_3128d0(product, 2);
}

static inline __m128 cross_3128d0(__m128 const &a, __m128 const &b)
{
	return _mm_sub_ps(
		_mm_mul_ps(_mm_shuffle_ps(a, a, 0xc9), _mm_shuffle_ps(b, b, 0xd2)),
		_mm_mul_ps(_mm_shuffle_ps(a, a, 0xd2), _mm_shuffle_ps(b, b, 0xc9)));
}

// rsqrtss refined by one Newton step
static inline float inverse_sqrt_3128d0(float value)
{
	float const estimate = _mm_cvtss_f32(_mm_rsqrt_ss(_mm_set_ss(value)));
	return (0.5f * estimate) * (3.0f - (value * estimate) * estimate);
}

// whether a vector's first three terms have unit length (false for a NaN)
static inline bool unit_3128d0(__m128 const &v)
{
	float const error = dot3_3128d0(v, v) - 1.0f;
	return (error < 0.0f ? -error : error) < unit_tolerance_3128d0;
}

// a motion's transform (class_3114d0's at 0x80): three axes and a translation
struct transform_3128d0
{
	__m128 axes[3];
	__m128 translation;
};

// (axis0 x + axis1 y) + axis2 z
static inline __m128 rotate_3128d0(transform_3128d0 const *transform, __m128 const &v)
{
	return _mm_add_ps(_mm_add_ps(
		_mm_mul_ps(transform->axes[0], _mm_shuffle_ps(v, v, 0x00)),
		_mm_mul_ps(transform->axes[1], _mm_shuffle_ps(v, v, 0x55))),
		_mm_mul_ps(transform->axes[2], _mm_shuffle_ps(v, v, 0xaa)));
}

// (axis0 x + axis1 y) + (axis2 z + translation), as 0x2da610
static inline __m128 transform_point_3128d0(transform_3128d0 const *transform, __m128 const &p)
{
	__m128 const z = _mm_add_ps(_mm_mul_ps(transform->axes[2], _mm_shuffle_ps(p, p, 0xaa)), transform->translation);
	__m128 const xy = _mm_add_ps(
		_mm_mul_ps(transform->axes[0], _mm_shuffle_ps(p, p, 0x00)),
		_mm_mul_ps(transform->axes[1], _mm_shuffle_ps(p, p, 0x55)));
	return _mm_add_ps(xy, z);
}

// an entity: its motion (at 0x3c) holds the transform at 0x80
struct entity_3128d0
{
	void const *vtable;
	unsigned short memory_size;
	unsigned short reference_count;
	char unknown8[0x3c - 0x8];
	char *motion;
};

static inline transform_3128d0 const *entity_transform_3128d0(void *entity)
{
	return (transform_3128d0 const *)(((entity_3128d0 *)entity)->motion + 0x80);
}

// a slot 7's answer (hkBool, returned by value through a hidden pointer)
struct hkbool_3128d0
{
	char value;
	hkbool_3128d0() {}
	hkbool_3128d0(bool value) : value(value) {}
};

// how much solver room a constraint takes (slot 10)
struct solver_info_3128d0
{
	long max_jacobian_size;
	long jacobians_size;
	long schemas_size;
	long jacobian_count;
};

static inline void set_solver_info_3128d0(solver_info_3128d0 *info, long jacobians_size, long schemas_size, long count)
{
	info->max_jacobian_size = 0;
	info->schemas_size = schemas_size;
	info->jacobians_size = jacobians_size;
	info->jacobian_count = count;
}

// the builders' infos (as havok_jacobian_builders_31e740.cpp lays them out)
struct angular_info_3128d0
{
	__m128 a;
	__m128 b;
	__m128 axis;
};

struct limit_info_3128d0
{
	__m128 axis;
	float minimum;
	float maximum;
	float value;
	long unknown1c;
};

struct motor_info_3128d0
{
	__m128 const *axes;
	void const *errors;
	float strength;
	long count;
};

struct linear_info_3128d0
{
	__m128 point_a;
	__m128 point_b;
	__m128 direction;
};

struct linear_limit_info_3128d0
{
	__m128 point_a;
	__m128 point_b;
	__m128 direction;
	float minimum;
	float maximum;
	long unknown38[2];
};

struct linear_motor_info_3128d0
{
	__m128 point;
	__m128 axis;
	float strength;
	void const *record;
	long unknown28[2];
};

// ---------------------------------------------------------------------------
// the constraint base (as havok_joints_312920.cpp's constraint_31bbd0)

struct snapshot_3128d0
{
	void *entity_a;
	void *entity_b;
	long priority;
	long unknownc;
};

// an object's slot 0, its deleting destructor
class deletable_3128d0
{
public:
	virtual void *deleting_destructor(unsigned int flags) = 0;
};

// the owner a constraint tells of itself
class owner_3128d0
{
public:
	virtual void slot_0() = 0;
	virtual void constraint_changed(void *constraint) = 0;
	virtual void add_solver_info(void *constraint, solver_info_3128d0 *info) = 0;
	virtual void remove_solver_info(void *constraint, solver_info_3128d0 *info) = 0;
};

class constraint_3128d0
{
public:
	void take_priority(snapshot_3128d0 const *snapshot);
	void destruct();
	void destruct_314b40();

	void const *vtable;
	unsigned short memory_size;
	unsigned short reference_count;
	void const *atom_vtable;
	void *entity_a;
	void *entity_b;
	owner_3128d0 *owner;
	long priority;
	long unknown1c;

	transform_3128d0 const *transform_a() const { return entity_transform_3128d0(entity_a); }
	transform_3128d0 const *transform_b() const { return entity_transform_3128d0(entity_b); }
};

typedef char assert_constraint_3128d0_size[sizeof(constraint_3128d0) == 0x20 ? 1 : -1];

// @retail 0x31b960
void constraint_3128d0::take_priority(snapshot_3128d0 const *snapshot)
{
	priority = snapshot->priority;
}

// the constraint's atom (at 8)
class constraint_atom_3128d0
{
public:
	void changed(void *unused);
	void destruct();

	void const *vtable;
	char unknown4[0xc - 0x4];
	owner_3128d0 *owner;
};

// @retail 0x31b990
void constraint_atom_3128d0::changed(void *unused)
{
	if (owner)
	{
		owner->constraint_changed((char *)this - 8);
	}
}

// @retail 0x31b9b0
void constraint_atom_3128d0::destruct()
{
	vtable = (void const *)0x415340;
}

// takes the constraint off an entity's list and drops the entity's
// reference (an entity in the cache file is kept at one instead of deleted)
static inline void release_entity_3128d0(constraint_3128d0 *constraint, void *entity)
{
	h2_retail_30bcc0(entity, constraint);
	entity_3128d0 *held = (entity_3128d0 *)entity;
	if (--held->reference_count == 0)
	{
		if (h2_retail_279740((long)held))
		{
			held->reference_count = 1;
		}
		else
		{
			((deletable_3128d0 *)held)->deleting_destructor(1);
		}
	}
}

// @retail 0x31b9e0
void constraint_3128d0::destruct()
{
	vtable = (void const *)0x415360;
	atom_vtable = (void const *)0x415350;
	if (entity_a)
	{
		release_entity_3128d0(this, entity_a);
	}
	if (entity_b)
	{
		release_entity_3128d0(this, entity_b);
	}
	atom_vtable = (void const *)0x415340;
	h2_retail_dc370(this);
}

// the joints' destructor is the base's
// @retail 0x314b40
void constraint_3128d0::destruct_314b40()
{
	destruct();
}

// ---------------------------------------------------------------------------
// the class objects' functions, common shapes

static inline void *allocate_3128d0(long size, long tag)
{
	referenced_2d88a0 *object = (referenced_2d88a0 *)g_memory_2d88a0->allocate_block(size, tag);
	object->memory_size = (unsigned short)size;
	return object;
}

// the joints' operator delete (reached when a constructor throws)
// @retail 0x312900
void __cdecl joint_delete_312900(void *object)
{
	g_memory_2d88a0->release_block(object, ((referenced_2d88a0 *)object)->memory_size, 0x27);
}

// ---------------------------------------------------------------------------
// kind 2: the ragdoll (0xe0 bytes)

class ragdoll_3128d0 : public constraint_3128d0
{
public:
	void *records();
	hkbool_3128d0 is_valid();
	void build_jacobians(void const *query, void *writer);
	void cone_and_plane(__m128 const *axis_a, __m128 const *axis_b, __m128 const *plane_b,
		void const *query, void *writer);
	void write(snapshot_3128d0 const *snapshot);

	// the pivots and the axes in A's frame and B's (the snapshot's blocks 0,
	// 2, 4 and 1, 3, 5)
	__m128 pivot_a;
	__m128 axis_30;
	__m128 axis_40;
	__m128 pivot_b;
	__m128 axis_60;
	__m128 axis_70;
	// the cone's cosine, the plane's two limits, the twist's two limits, the
	// motors' strength
	float cone;
	float plane_minimum;
	float plane_maximum;
	float twist_minimum;
	float twist_maximum;
	float strength;
	long records_98[9][2];
};

typedef char assert_ragdoll_3128d0_size[sizeof(ragdoll_3128d0) == 0xe0 ? 1 : -1];
typedef char assert_ragdoll_3128d0_values[__builtin_offsetof(ragdoll_3128d0, cone) == 0x80 ? 1 : -1];

// @retail 0x312980
void *__cdecl ragdoll_create_312980(void const *snapshot)
{
	return h2_retail_312d50(allocate_3128d0(0xe0, 0x27), snapshot);
}

// @retail 0x3129e0
void *__cdecl ragdoll_snapshot_3129e0(void *snapshot)
{
	if (snapshot)
	{
		h2_retail_312920(snapshot);
	}
	return snapshot;
}

// @retail 0x3128d0
void __cdecl ragdoll_read_3128d0(void *joint, void *snapshot)
{
	h2_retail_312ab0(joint, snapshot);
}

// @retail 0x3128e0
void __cdecl ragdoll_write_3128e0(void *joint, void const *snapshot)
{
	((ragdoll_3128d0 *)joint)->write((snapshot_3128d0 const *)snapshot);
}

// @retail 0x312de0
void ragdoll_3128d0::write(snapshot_3128d0 const *snapshot)
{
	take_priority(snapshot);
	h2_retail_312a30(this, snapshot);
}

// @retail 0x312a20
void *ragdoll_3128d0::records()
{
	return records_98;
}

// the axes of unit length, the cone's cosine at most one and both limit
// pairs in order
// @retail 0x312bf0
hkbool_3128d0 ragdoll_3128d0::is_valid()
{
	if (!unit_3128d0(axis_30) || !unit_3128d0(axis_60) || !unit_3128d0(axis_40) || !unit_3128d0(axis_70))
	{
		return hkbool_3128d0(false);
	}
	if ((cone < 0.0f ? -cone : cone) > 1.0f)
	{
		return hkbool_3128d0(false);
	}
	if (plane_minimum > plane_maximum)
	{
		return hkbool_3128d0(false);
	}
	return hkbool_3128d0(!(twist_minimum > twist_maximum));
}

// the cone (the angle between A's axis and B's) and the plane (A's axis
// against B's other axis) as angular limits about the axes perpendicular to
// both, skipped when the axes are (nearly) parallel
// @retail 0x312e00
void ragdoll_3128d0::cone_and_plane(__m128 const *axis_a_world, __m128 const *axis_b_world,
	__m128 const *plane_b_world, void const *query, void *writer)
{
	__m128 const a = *axis_a_world;
	__m128 const across = cross_3128d0(a, *axis_b_world);
	float const across_squared = dot3_3128d0(across, across);
	if (!(across_squared < epsilon_3128d0))
	{
		limit_info_3128d0 info;
		info.axis = _mm_mul_ps(splat_3128d0(1.0f / __builtin_sqrtf(across_squared)), across);
		info.minimum = cone;
		info.maximum = 100.0f; // 0x42c80000
		info.value = dot3_3128d0(a, *axis_b_world);
		h2_retail_31f000(&info, query, writer);
	}
	__m128 const across_plane = cross_3128d0(a, *plane_b_world);
	float const across_plane_squared = dot3_3128d0(across_plane, across_plane);
	if (!(across_plane_squared < epsilon_3128d0))
	{
		limit_info_3128d0 info;
		info.axis = _mm_mul_ps(splat_3128d0(1.0f / __builtin_sqrtf(across_plane_squared)), across_plane);
		info.minimum = plane_minimum;
		info.maximum = plane_maximum;
		info.value = dot3_3128d0(a, *plane_b_world);
		h2_retail_31f000(&info, query, writer);
	}
}

// @retail 0x312fe0
void ragdoll_3128d0::build_jacobians(void const *query, void *writer)
{
	h2_retail_31e770(query, writer, records_98, 8);
	transform_3128d0 const *transform_a_ = transform_a();
	__m128 world_a[3];
	world_a[0] = rotate_3128d0(transform_a_, pivot_a);
	world_a[1] = rotate_3128d0(transform_a_, axis_30);
	world_a[2] = rotate_3128d0(transform_a_, axis_40);
	world_a[0] = _mm_add_ps(world_a[0], transform_a_->translation);
	transform_3128d0 const *transform_b_ = transform_b();
	__m128 world_b[3];
	world_b[0] = rotate_3128d0(transform_b_, pivot_b);
	world_b[1] = rotate_3128d0(transform_b_, axis_60);
	world_b[2] = rotate_3128d0(transform_b_, axis_70);
	world_b[0] = _mm_add_ps(world_b[0], transform_b_->translation);

	cone_and_plane(&world_a[2], &world_b[2], &world_b[1], query, writer);

	// the twist about the mean of the two frames' axes
	__m128 const sum = _mm_add_ps(world_b[2], world_a[2]);
	__m128 const twist = _mm_mul_ps(splat_3128d0(inverse_sqrt_3128d0(dot3_3128d0(sum, sum))), sum);
	__m128 const perpendicular = cross_3128d0(twist, world_b[1]);
	__m128 const other = cross_3128d0(perpendicular, twist);
	limit_info_3128d0 info;
	info.axis = twist;
	info.minimum = twist_minimum;
	info.maximum = twist_maximum;
	info.value = h2_retail_3725e0(dot3_3128d0(perpendicular, world_a[1]), dot3_3128d0(other, world_a[1]));
	h2_retail_31f000(&info, query, writer);

	h2_retail_31f3e0(&world_a[0], &world_b[0], query, writer);

	if (strength > 0.0f)
	{
		motor_info_3128d0 motors;
		motors.axes = transform_a_->axes;
		motors.errors = records_98[6];
		motors.strength = strength;
		motors.count = 3;
		h2_retail_31e900(&motors, query, writer);
	}
}

// ---------------------------------------------------------------------------
// kind 1: the limited hinge (0xe0 bytes)

class limited_hinge_3128d0 : public constraint_3128d0
{
public:
	void *records();
	void solver_info(solver_info_3128d0 *info);
	void build_jacobians(void const *query, void *writer);
	void write(snapshot_3128d0 const *snapshot);

	__m128 pivot_a;
	__m128 axis_a;
	__m128 perpendicular_a;
	__m128 perpendicular2_a;
	__m128 pivot_b;
	__m128 perpendicular_b;
	__m128 perpendicular2_b;
	long records_90[6][2];
	long motor_record[2];
	float minimum_angle;
	float maximum_angle;
	float strength;
	// the angle last measured, to keep it continuous past a half turn
	float last_angle;
	long unknownd8[2];
};

typedef char assert_limited_hinge_3128d0_size[sizeof(limited_hinge_3128d0) == 0xe0 ? 1 : -1];
typedef char assert_limited_hinge_3128d0_angles[__builtin_offsetof(limited_hinge_3128d0, minimum_angle) == 0xc8 ? 1 : -1];

// @retail 0x313370
void *__cdecl limited_hinge_create_313370(void const *snapshot)
{
	return h2_retail_313550(allocate_3128d0(0xe0, 0x27), snapshot);
}

// @retail 0x3133d0
void *__cdecl limited_hinge_snapshot_3133d0(void *snapshot)
{
	if (snapshot)
	{
		h2_retail_313320(snapshot);
	}
	return snapshot;
}

// @retail 0x3132f0
void __cdecl limited_hinge_read_3132f0(void *joint, void *snapshot)
{
	h2_retail_3134a0(joint, snapshot);
}

// @retail 0x313300
void __cdecl limited_hinge_write_313300(void *joint, void const *snapshot)
{
	((limited_hinge_3128d0 *)joint)->write((snapshot_3128d0 const *)snapshot);
}

// @retail 0x3137f0
void limited_hinge_3128d0::write(snapshot_3128d0 const *snapshot)
{
	take_priority(snapshot);
	h2_retail_313420(this, snapshot);
}

// @retail 0x313410
void *limited_hinge_3128d0::records()
{
	return records_90;
}

// @retail 0x313520
void limited_hinge_3128d0::solver_info(solver_info_3128d0 *info)
{
	set_solver_info_3128d0(info, 0x110, 0x44, 7);
}

// @retail 0x313810
void limited_hinge_3128d0::build_jacobians(void const *query, void *writer)
{
	h2_retail_31e770(query, writer, records_90, 8);
	transform_3128d0 const *transform_a_ = transform_a();
	__m128 world_a[4];
	world_a[0] = rotate_3128d0(transform_a_, pivot_a);
	world_a[1] = rotate_3128d0(transform_a_, axis_a);
	world_a[2] = rotate_3128d0(transform_a_, perpendicular_a);
	world_a[3] = rotate_3128d0(transform_a_, perpendicular2_a);
	world_a[0] = _mm_add_ps(world_a[0], transform_a_->translation);
	transform_3128d0 const *transform_b_ = transform_b();
	__m128 world_b[3];
	world_b[0] = rotate_3128d0(transform_b_, pivot_b);
	world_b[1] = rotate_3128d0(transform_b_, perpendicular_b);
	world_b[2] = rotate_3128d0(transform_b_, perpendicular2_b);
	world_b[0] = _mm_add_ps(world_b[0], transform_b_->translation);

	// the hinge axes kept together: B's perpendicular stays perpendicular to
	// A's two
	angular_info_3128d0 angular;
	angular.a = world_a[3];
	angular.b = world_b[1];
	angular.axis = world_a[2];
	h2_retail_31e7b0(&angular, query, writer);
	angular.a = world_a[2];
	angular.b = _mm_xor_ps(world_b[1], *(__m128 const *)sign_mask_3128d0);
	angular.axis = world_a[3];
	h2_retail_31e7b0(&angular, query, writer);

	// the hinge's angle, kept within a half turn of the last one
	float angle = h2_retail_3725e0(dot3_3128d0(world_b[2], world_a[2]),
		dot3_3128d0(cross_3128d0(world_b[2], world_b[1]), world_a[2]));
	if (last_angle - angle > pi_3128d0)
	{
		angle = angle + two_pi_3128d0;
	}
	else if (angle - last_angle > pi_3128d0)
	{
		angle = angle - two_pi_3128d0;
	}
	last_angle = angle;
	limit_info_3128d0 limit;
	limit.axis = world_a[1];
	limit.minimum = minimum_angle;
	limit.maximum = maximum_angle;
	limit.value = angle;
	h2_retail_31f000(&limit, query, writer);

	h2_retail_31f3e0(&world_a[0], &world_b[0], query, writer);

	if (!(strength == 0.0f))
	{
		motor_info_3128d0 motor;
		motor.axes = &world_a[1];
		motor.errors = motor_record;
		motor.strength = strength;
		motor.count = 1;
		h2_retail_31e900(&motor, query, writer);
	}
}

// ---------------------------------------------------------------------------
// kind 0: the hinge (0xa0 bytes)

class hinge_3128d0 : public constraint_3128d0
{
public:
	void *records();
	void solver_info(solver_info_3128d0 *info);
	void build_jacobians(void const *query, void *writer);
	void write(snapshot_3128d0 const *snapshot);

	__m128 pivot_a;
	__m128 perpendicular_a;
	__m128 perpendicular2_a;
	__m128 pivot_b;
	__m128 axis_b;
	long records_70[5][2];
	long unknown98[2];
};

typedef char assert_hinge_3128d0_size[sizeof(hinge_3128d0) == 0xa0 ? 1 : -1];

// the hinge's own slot 7 (0x313cc0, havok_joints_312920.cpp), declared as
// that file defines it
class hinge_slots_3128d0
{
public:
	virtual void slot_0() = 0;
	virtual void slot_1() = 0;
	virtual void slot_2() = 0;
	virtual void slot_3() = 0;
	virtual void slot_4() = 0;
	virtual void slot_5() = 0;
	virtual void slot_6() = 0;
	virtual void is_valid(bool *result) = 0;
};

// a hinge snapshot (c_component_joint_snapshot_0)
struct hinge_snapshot_3128d0 : snapshot_3128d0
{
	__m128 data[5];
};

// @retail 0x313b60
void *__cdecl hinge_create_313b60(void const *snapshot)
{
	return h2_retail_313de0(allocate_3128d0(0xa0, 0x27), snapshot);
}

// @retail 0x313bc0
void *__cdecl hinge_snapshot_313bc0(void *snapshot)
{
	if (snapshot)
	{
		h2_retail_31b950(snapshot);
		hinge_snapshot_3128d0 *hinge = (hinge_snapshot_3128d0 *)snapshot;
		for (int i = 0; i < 5; i++)
		{
			hinge->data[i] = _mm_setzero_ps();
		}
	}
	return snapshot;
}

// @retail 0x313b00
void __cdecl hinge_read_313b00(void *joint, void *snapshot)
{
	h2_retail_313c50(joint, snapshot);
}

// @retail 0x313b10
void __cdecl hinge_write_313b10(void *joint, void const *snapshot)
{
	((hinge_3128d0 *)joint)->write((snapshot_3128d0 const *)snapshot);
}

// the snapshot's blocks in, then the validity check (its result unused)
// @retail 0x313ea0
void hinge_3128d0::write(snapshot_3128d0 const *snapshot)
{
	take_priority(snapshot);
	hinge_snapshot_3128d0 const *hinge = (hinge_snapshot_3128d0 const *)snapshot;
	pivot_a = hinge->data[0];
	perpendicular_a = hinge->data[3];
	perpendicular2_a = hinge->data[4];
	pivot_b = hinge->data[1];
	axis_b = hinge->data[2];
	bool valid;
	((hinge_slots_3128d0 *)this)->is_valid(&valid);
}

// @retail 0x313c40
void *hinge_3128d0::records()
{
	return records_70;
}

// @retail 0x313c90
void hinge_3128d0::solver_info(solver_info_3128d0 *info)
{
	set_solver_info_3128d0(info, 0xd0, 0x2c, 5);
}

// @retail 0x313ef0
void hinge_3128d0::build_jacobians(void const *query, void *writer)
{
	h2_retail_31e770(query, writer, records_70, 8);
	transform_3128d0 const *transform_a_ = transform_a();
	__m128 world_a[3];
	world_a[0] = rotate_3128d0(transform_a_, pivot_a);
	world_a[1] = rotate_3128d0(transform_a_, perpendicular_a);
	world_a[2] = rotate_3128d0(transform_a_, perpendicular2_a);
	world_a[0] = _mm_add_ps(world_a[0], transform_a_->translation);
	transform_3128d0 const *transform_b_ = transform_b();
	__m128 world_b[2];
	world_b[0] = rotate_3128d0(transform_b_, pivot_b);
	world_b[1] = rotate_3128d0(transform_b_, axis_b);
	world_b[0] = _mm_add_ps(world_b[0], transform_b_->translation);

	angular_info_3128d0 angular;
	angular.a = world_a[2];
	angular.b = world_b[1];
	angular.axis = world_a[1];
	h2_retail_31e7b0(&angular, query, writer);
	angular.a = world_a[1];
	angular.b = _mm_xor_ps(world_b[1], *(__m128 const *)sign_mask_3128d0);
	angular.axis = world_a[2];
	h2_retail_31e7b0(&angular, query, writer);

	h2_retail_31f3e0(&world_a[0], &world_b[0], query, writer);
}

// ---------------------------------------------------------------------------
// kind 4: the ball and socket (0x60 bytes)

class ball_socket_3128d0 : public constraint_3128d0
{
public:
	void *records();
	void solver_info(solver_info_3128d0 *info);
	void build_jacobians(void const *query, void *writer);
	void write(snapshot_3128d0 const *snapshot);

	__m128 pivot_a;
	__m128 pivot_b;
	long records_40[3][2];
	long unknown58[2];
};

typedef char assert_ball_socket_3128d0_size[sizeof(ball_socket_3128d0) == 0x60 ? 1 : -1];

struct two_vector_snapshot_3128d0 : snapshot_3128d0
{
	__m128 data[2];
	float value;
	long unknown34[3];
};

// @retail 0x3140e0
void *__cdecl ball_socket_create_3140e0(void const *snapshot)
{
	return h2_retail_314270(allocate_3128d0(0x60, 0x27), snapshot);
}

// @retail 0x314140
void *__cdecl ball_socket_snapshot_314140(void *snapshot)
{
	if (snapshot)
	{
		h2_retail_31b950(snapshot);
		two_vector_snapshot_3128d0 *ball = (two_vector_snapshot_3128d0 *)snapshot;
		ball->data[0] = _mm_setzero_ps();
		ball->data[1] = _mm_setzero_ps();
	}
	return snapshot;
}

// @retail 0x314090
void __cdecl ball_socket_read_314090(void *joint, void *snapshot)
{
	h2_retail_3141c0(joint, snapshot);
}

// @retail 0x3140a0
void __cdecl ball_socket_write_3140a0(void *joint, void const *snapshot)
{
	((ball_socket_3128d0 *)joint)->write((snapshot_3128d0 const *)snapshot);
}

// @retail 0x3142f0
void ball_socket_3128d0::write(snapshot_3128d0 const *snapshot)
{
	take_priority(snapshot);
	two_vector_snapshot_3128d0 const *ball = (two_vector_snapshot_3128d0 const *)snapshot;
	pivot_a = ball->data[0];
	pivot_b = ball->data[1];
}

// @retail 0x3141b0
void *ball_socket_3128d0::records()
{
	return records_40;
}

// @retail 0x3141f0
void ball_socket_3128d0::solver_info(solver_info_3128d0 *info)
{
	set_solver_info_3128d0(info, 0x90, 0x24, 3);
}

// @retail 0x314220
void ball_socket_3128d0::build_jacobians(void const *query, void *writer)
{
	h2_retail_31e770(query, writer, records_40, 8);
	h2_retail_31f160(transform_a(), transform_b(), &pivot_a, &pivot_b, query, writer);
}

// ---------------------------------------------------------------------------
// kind 3: the stiff spring (0x50 bytes)

class stiff_spring_3128d0 : public constraint_3128d0
{
public:
	void *records();
	hkbool_3128d0 is_valid();
	void solver_info(solver_info_3128d0 *info);
	void build_jacobians(void const *query, void *writer);
	void read(two_vector_snapshot_3128d0 *snapshot);
	void write(snapshot_3128d0 const *snapshot);

	__m128 pivot_a;
	__m128 pivot_b;
	float rest_length;
	long records_44[1][2];
	long unknown4c;
};

typedef char assert_stiff_spring_3128d0_size[sizeof(stiff_spring_3128d0) == 0x50 ? 1 : -1];

// @retail 0x314880
void *__cdecl stiff_spring_create_314880(void const *snapshot)
{
	return h2_retail_314b00(allocate_3128d0(0x50, 0x27), snapshot);
}

// @retail 0x3148e0
void *__cdecl stiff_spring_snapshot_3148e0(void *snapshot)
{
	if (snapshot)
	{
		h2_retail_31b950(snapshot);
		two_vector_snapshot_3128d0 *spring = (two_vector_snapshot_3128d0 *)snapshot;
		spring->data[0] = _mm_setzero_ps();
		spring->data[1] = _mm_setzero_ps();
		*(long *)&spring->value = 0;
	}
	return snapshot;
}

// @retail 0x314830
void __cdecl stiff_spring_read_314830(void *joint, void *snapshot)
{
	((stiff_spring_3128d0 *)joint)->read((two_vector_snapshot_3128d0 *)snapshot);
}

// @retail 0x314840
void __cdecl stiff_spring_write_314840(void *joint, void const *snapshot)
{
	((stiff_spring_3128d0 *)joint)->write((snapshot_3128d0 const *)snapshot);
}

// @retail 0x314980
void stiff_spring_3128d0::read(two_vector_snapshot_3128d0 *snapshot)
{
	h2_retail_31b970(this, snapshot);
	snapshot->data[0] = pivot_a;
	snapshot->data[1] = pivot_b;
	snapshot->value = rest_length;
}

// @retail 0x314b50
void stiff_spring_3128d0::write(snapshot_3128d0 const *snapshot)
{
	take_priority(snapshot);
	two_vector_snapshot_3128d0 const *spring = (two_vector_snapshot_3128d0 const *)snapshot;
	pivot_a = spring->data[0];
	pivot_b = spring->data[1];
	rest_length = spring->value;
}

// @retail 0x314950
void *stiff_spring_3128d0::records()
{
	return records_44;
}

// a positive rest length (or a NaN)
// @retail 0x314960
hkbool_3128d0 stiff_spring_3128d0::is_valid()
{
	return hkbool_3128d0(!(rest_length <= 0.0f));
}

// @retail 0x3149b0
void stiff_spring_3128d0::solver_info(solver_info_3128d0 *info)
{
	set_solver_info_3128d0(info, 0x30, 0x1c, 1);
}

// one linear row along the pivots' line, its error the rest length less
// their distance
// @retail 0x3149e0
void stiff_spring_3128d0::build_jacobians(void const *query, void *writer)
{
	h2_retail_31e770(query, writer, records_44, 8);
	linear_info_3128d0 info;
	info.point_a = transform_point_3128d0(transform_a(), pivot_a);
	info.point_b = transform_point_3128d0(transform_b(), pivot_b);
	__m128 const separation = _mm_sub_ps(info.point_a, info.point_b);
	float const length_squared = dot3_3128d0(separation, separation);
	float const inverse_length = inverse_sqrt_3128d0(length_squared);
	float const length = length_squared * inverse_length;
	info.direction = _mm_mul_ps(splat_3128d0(inverse_length), separation);
	h2_retail_31ec30(&info, query, writer);
	h2_retail_31e740(rest_length - length, query, writer, 1);
}

// ---------------------------------------------------------------------------
// kind 5: the prismatic (0xf0 bytes)

class prismatic_3128d0 : public constraint_3128d0
{
public:
	void *records();
	void solver_info(solver_info_3128d0 *info);
	void build_jacobians(void const *query, void *writer);
	void read(void *snapshot);
	void write(snapshot_3128d0 const *snapshot);

	__m128 pivot_a;
	__m128 pivot_b;
	__m128 shaft_a;
	// the sliding axis in B's frame, and a perpendicular to it
	__m128 shaft_b;
	__m128 perpendicular_b;
	// B's frame relative to A's
	__m128 rotation[3];
	float minimum;
	float maximum;
	// the friction's strength
	float friction;
	long records_ac[6][2];
	long friction_record[2];
	long unknowne4[3];
};

typedef char assert_prismatic_3128d0_size[sizeof(prismatic_3128d0) == 0xf0 ? 1 : -1];
typedef char assert_prismatic_3128d0_limits[__builtin_offsetof(prismatic_3128d0, minimum) == 0xa0 ? 1 : -1];
typedef char assert_prismatic_3128d0_friction_record[__builtin_offsetof(prismatic_3128d0, friction_record) == 0xdc ? 1 : -1];

// a prismatic snapshot (c_component_joint_snapshot_5)
struct prismatic_snapshot_3128d0 : snapshot_3128d0
{
	__m128 data[8];
	float minimum;
	float maximum;
	float value;
	long unknown9c;
};

// @retail 0x314c20
void *__cdecl prismatic_create_314c20(void const *snapshot)
{
	return h2_retail_314ef0(allocate_3128d0(0xf0, 0x27), snapshot);
}

// @retail 0x314c80
void *__cdecl prismatic_snapshot_314c80(void *snapshot)
{
	if (snapshot)
	{
		h2_retail_314bb0(snapshot);
	}
	return snapshot;
}

// @retail 0x314b80
void __cdecl prismatic_read_314b80(void *joint, void *snapshot)
{
	((prismatic_3128d0 *)joint)->read(snapshot);
}

// @retail 0x314b90
void __cdecl prismatic_write_314b90(void *joint, void const *snapshot)
{
	((prismatic_3128d0 *)joint)->write((snapshot_3128d0 const *)snapshot);
}

// @retail 0x314d50
void prismatic_3128d0::read(void *snapshot)
{
	h2_retail_31b970(this, snapshot);
	prismatic_snapshot_3128d0 *prismatic = (prismatic_snapshot_3128d0 *)snapshot;
	prismatic->data[0] = pivot_a;
	prismatic->data[1] = pivot_b;
	prismatic->data[2] = shaft_a;
	prismatic->data[3] = shaft_b;
	prismatic->minimum = minimum;
	prismatic->maximum = maximum;
	prismatic->data[4] = perpendicular_b;
	prismatic->data[5] = rotation[0];
	prismatic->data[6] = rotation[1];
	prismatic->data[7] = rotation[2];
	prismatic->value = friction;
}

// @retail 0x314fa0
void prismatic_3128d0::write(snapshot_3128d0 const *snapshot)
{
	take_priority(snapshot);
	h2_retail_314cd0(this, snapshot);
}

// @retail 0x314cc0
void *prismatic_3128d0::records()
{
	return records_ac;
}

// @retail 0x314dd0
void prismatic_3128d0::solver_info(solver_info_3128d0 *info)
{
	set_solver_info_3128d0(info, 0x120, 0x40, 7);
}

// the frames locked together (three angular rows), the pivot held on the
// axis (two linear rows), the slide limited and, with a friction, a motor
// along the axis
// @retail 0x314fc0
void prismatic_3128d0::build_jacobians(void const *query, void *writer)
{
	h2_retail_31e770(query, writer, records_ac, 8);
	transform_3128d0 const *transform_a_ = transform_a();
	transform_3128d0 const *transform_b_ = transform_b();

	// B's frame as A holds it, in world space (0x2da8c0)
	__m128 frame[3];
	for (int i = 0; i < 3; i++)
	{
		frame[i] = rotate_3128d0(transform_a_, rotation[i]);
	}
	angular_info_3128d0 angular;
	angular.a = frame[0];
	angular.b = transform_b_->axes[1];
	angular.axis = frame[2];
	h2_retail_31e7b0(&angular, query, writer);
	angular.a = frame[1];
	angular.b = transform_b_->axes[2];
	angular.axis = frame[0];
	h2_retail_31e7b0(&angular, query, writer);
	angular.a = frame[2];
	angular.b = transform_b_->axes[0];
	angular.axis = frame[1];
	h2_retail_31e7b0(&angular, query, writer);

	__m128 const point_a = transform_point_3128d0(transform_a_, pivot_a);
	__m128 const point_b = transform_point_3128d0(transform_b_, pivot_b);
	__m128 const axis = rotate_3128d0(transform_b_, shaft_b);
	// B's pivot moved along the axis to A's
	float const along = dot3_3128d0(_mm_sub_ps(point_a, point_b), axis);
	__m128 const on_axis = _mm_add_ps(point_b, _mm_mul_ps(splat_3128d0(along), axis));
	__m128 const perpendicular = rotate_3128d0(transform_b_, perpendicular_b);
	linear_info_3128d0 linear;
	linear.point_a = point_a;
	linear.point_b = on_axis;
	linear.direction = perpendicular;
	h2_retail_31ec30(&linear, query, writer);
	linear.direction = cross_3128d0(axis, perpendicular);
	h2_retail_31ec30(&linear, query, writer);

	linear_limit_info_3128d0 limit;
	limit.point_a = point_a;
	limit.point_b = point_b;
	limit.direction = axis;
	limit.minimum = minimum;
	limit.maximum = maximum;
	h2_retail_31ee10(&limit, query, writer);

	if (friction > 0.0f)
	{
		linear_motor_info_3128d0 motor;
		motor.point = point_a;
		motor.axis = axis;
		motor.strength = friction;
		motor.record = friction_record;
		h2_retail_31ea90(&motor, query, writer);
	}
}

// ---------------------------------------------------------------------------
// the spatial deactivator's class object (table 0x414a08): its settings
// (havok_deactivator_314710.cpp's s_314320) and the deactivator (0x60 bytes,
// vtable 0x414abc)

struct deactivator_settings_3128d0
{
	float size_squared;
	float high_distance;
	float high_angle;
	float low_distance;
	float low_angle;
};

class deactivator_3128d0
{
public:
	void construct(deactivator_settings_3128d0 const *settings);

	void const *vtable;
	unsigned short memory_size;
	unsigned short reference_count;
	float size_squared;
	long unknownc;
	__m128 high_position;
	__m128 high_rotation;
	__m128 low_position;
	__m128 low_rotation;
	float high_distance;
	float high_angle;
	float low_distance;
	float low_angle;
};

typedef char assert_deactivator_3128d0_size[sizeof(deactivator_3128d0) == 0x60 ? 1 : -1];

// the poses unset: positions far away, rotations the identity
// @retail 0x3147c0
void deactivator_3128d0::construct(deactivator_settings_3128d0 const *settings)
{
	reference_count = 1;
	vtable = (void const *)0x414abc;
	high_distance = settings->high_distance;
	high_angle = settings->high_angle;
	low_distance = settings->low_distance;
	low_angle = settings->low_angle;
	__m128 const far_away = splat_3128d0(3.40282347e+38f); // 0x7f7fffff
	high_position = far_away;
	low_position = far_away;
	high_rotation = _mm_setzero_ps();
	((float *)&high_rotation)[3] = 1.0f;
	low_rotation = _mm_setzero_ps();
	((float *)&low_rotation)[3] = 1.0f;
	size_squared = settings->size_squared;
}

// @retail 0x314380
void *__cdecl deactivator_create_314380(void const *settings)
{
	deactivator_3128d0 *deactivator = (deactivator_3128d0 *)allocate_3128d0(0x60, 0x26);
	deactivator->construct((deactivator_settings_3128d0 const *)settings);
	return deactivator;
}

// @retail 0x3143e0
void *__cdecl deactivator_settings_3143e0(void *settings)
{
	if (settings)
	{
		h2_retail_31bc60(settings);
		deactivator_settings_3128d0 *values = (deactivator_settings_3128d0 *)settings;
		values->size_squared = -1.0f; // 0xbf800000
		values->high_distance = 0.01f; // 0x3c23d70a
		values->high_angle = 0.005f; // 0x3ba3d70a
		values->low_distance = 0.1f; // 0x3dcccccd
		values->low_angle = 0.2f; // 0x3e4ccccd
	}
	return settings;
}

// @retail 0x314350
void __cdecl deactivator_read_314350(void *deactivator, void *settings)
{
	h2_retail_314450(deactivator, settings);
}

// @retail 0x314360
void __cdecl deactivator_write_314360(void *deactivator, void const *settings)
{
	h2_retail_314480(deactivator, settings);
}

// ---------------------------------------------------------------------------
// the contact constraint (as havok_contact_mgr_318a80.cpp lays it out)

struct array_3128d0
{
	void *data;
	long size;
	long capacity_and_flags;
};

class contact_constraint_3128d0 : public constraint_3128d0
{
public:
	void destruct_contact();
	void destruct_modifier();

	array_3128d0 ids;
	unsigned char ids_storage[8];
	array_3128d0 points;
	// a modifier's factors (the surface velocity keeps its caller's payload
	// in the first)
	union
	{
		float factor_40;
		void const *payload_40;
	};
	float factor_44;
	char unknown48[0x90 - 0x48];
	array_3128d0 properties;
	char unknown9c[0x110 - 0x9c];
};

typedef char assert_contact_constraint_3128d0_size[sizeof(contact_constraint_3128d0) == 0x110 ? 1 : -1];
typedef char assert_contact_constraint_3128d0_points[__builtin_offsetof(contact_constraint_3128d0, points) == 0x34 ? 1 : -1];

// an array's own storage freed (the top bit of the capacity marks storage it
// does not own)
static inline void free_array_3128d0(array_3128d0 *array, long element_size)
{
	if (array->capacity_and_flags >= 0)
	{
		g_memory_2d88a0->release_block(array->data, (array->capacity_and_flags & 0x7fffffff) * element_size, 0x12);
	}
}

// @retail 0x315510
void contact_constraint_3128d0::destruct_contact()
{
	free_array_3128d0(&properties, 0x1c);
	free_array_3128d0(&points, 0x20);
	free_array_3128d0(&ids, 1);
	destruct();
}

// @retail 0x3156c0
void contact_constraint_3128d0::destruct_modifier()
{
	destruct_contact();
}

// the contact constraint inside a contact manager (at 0x10)
struct contact_manager_3128d0
{
	char unknown0[0x10];
	contact_constraint_3128d0 constraint;
};

// whether a constraint is a plain contact constraint: its vtable that of the
// one built at 0x480990
static inline bool plain_contact_3128d0(contact_constraint_3128d0 const *constraint)
{
	return constraint->vtable == *(void const *const *)0x480990;
}

class hkEntity;

// (src/unknown_315340_native.cpp and src/unknown_3153c0_native.cpp define
// these names too, as stand-ins; the build prefers them while they exist)

// makes a manager's contact constraint the mass changer: the inverse
// masses scaled, `factor` for the given entity's body and `other_factor`
// for the other's; the owner told of the room the scaling's schemas take
// @retail 0x315340
void __cdecl function_315340(void *manager_, hkEntity *entity, hkEntity *unused, float factor, float other_factor)
{
	contact_manager_3128d0 *manager = (contact_manager_3128d0 *)manager_;
	contact_constraint_3128d0 *constraint = &manager->constraint;
	if (!plain_contact_3128d0(constraint))
	{
		return;
	}
	if ((void *)entity == constraint->entity_a)
	{
		constraint->factor_40 = factor;
		constraint->factor_44 = other_factor;
	}
	else
	{
		constraint->factor_40 = other_factor;
		constraint->factor_44 = factor;
	}
	solver_info_3128d0 info;
	info.max_jacobian_size = 0;
	info.jacobians_size = 0;
	info.jacobian_count = 0;
	info.schemas_size = 0x88;
	constraint->owner->add_solver_info(constraint, &info);
	constraint->vtable = *(void const *const *)0x480770;
}

// makes it the surface velocity: A's surface velocity, signed for the given
// entity, added to the friction rows; the caller's payload kept at 0x40
// @retail 0x3153c0
void __cdecl function_3153c0(void *manager_, void *entity, void const *payload)
{
	contact_manager_3128d0 *manager = (contact_manager_3128d0 *)manager_;
	float sign = 1.0f;
	if (manager->constraint.entity_a == entity)
	{
		sign = -1.0f;
	}
	contact_constraint_3128d0 *constraint = &manager->constraint;
	if (!plain_contact_3128d0(constraint))
	{
		return;
	}
	constraint->factor_44 = sign;
	solver_info_3128d0 info;
	info.max_jacobian_size = 0;
	info.jacobians_size = 0;
	info.jacobian_count = 0;
	constraint->payload_40 = payload;
	info.schemas_size = 0x28;
	constraint->owner->add_solver_info(constraint, &info);
	constraint->vtable = *(void const *const *)0x480880;
}
