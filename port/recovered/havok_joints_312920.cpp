// @flags /O2 /GL-
// Havok (third-party physics library): the six joint constraints the game's
// physics models link their rigid bodies with (built by 0x1d77d0), the
// snapshots ("cinfo" blocks) they are built from and read back into, and the
// constraint base they share.
//
//   the constraint base (0x20 bytes; vtable 0x415360, its atom at 8 vtable
//   0x415350)
//     0x31bbd0  constructor: takes the snapshot's two entities and priority
//     0x31b970  reads them back into a snapshot
//   the snapshot base (0x10 bytes)
//     0x319280, 0x31b950  constructors: the two entities and priority zero
//   the joints, each with its snapshot's constructor, a reader (the joint's
//   state back into a snapshot) and its own constructor
//     kind 0  snapshot 0x313b30, reader 0x313c50, joint 0x313de0 (0xa0 bytes,
//             vtable 0x41490c; slot 7 is_valid 0x313cc0)
//     kind 1  snapshot 0x313320, reader 0x3134a0, joint 0x313550 (0xe0 bytes,
//             vtable 0x414860; 0x313420 takes the snapshot in; slot 7
//             is_valid 0x3135f0)
//     kind 2  snapshot 0x312920, reader 0x312ab0, joint 0x312d50 (0xe0 bytes,
//             vtable 0x4146fc; 0x312a30 takes the snapshot in; slot 10
//             solver_info 0x312b30; 0x312b80 sets the strength)
//     kind 3  snapshot 0x314860, joint 0x314b00 (0x50 bytes, vtable 0x414b4c)
//     kind 4  snapshot 0x3140c0, reader 0x3141c0, joint 0x314270 (0x60 bytes,
//             vtable 0x41498c)
//     kind 5  snapshot 0x314bb0, joint 0x314ef0 (0xf0 bytes, vtable
//             0x414ca4; 0x314cd0 takes the snapshot in; slot 7 is_valid
//             0x314e00)
//   every joint's atom (at 8) has vtable 0x4146ec.
//
// The joints' vtables are their retail ones, stored by address; the calls
// the constructors make through them (the validity check, its result
// unused) and set_strength's go through the slot declarations below.

// ---------------------------------------------------------------------------
// recovered code this file calls

// the contact constraint's entity setters (port/recovered/
// havok_contact_mgr_318a80.cpp): take a reference to the new entity, put the
// constraint on its list, and drop the old one
extern "C" void h2_retail_31bac0(void *constraint, void *entity);
extern "C" void h2_retail_31bb30(void *constraint, void *entity);

// ---------------------------------------------------------------------------
// shared pieces

struct alignas(16) vector_312920
{
	float x, y, z, w;
};

static inline void zero_312920(vector_312920 *v, int count)
{
	for (int i = 0; i < count; i++)
	{
		v[i].x = 0.0f;
		v[i].y = 0.0f;
		v[i].z = 0.0f;
		v[i].w = 0.0f;
	}
}

// the dot product of two vectors' first three terms, in retail's order
static inline float dot3_312920(vector_312920 const &a, vector_312920 const &b)
{
	return (a.y * b.y + a.x * b.x) + a.z * b.z;
}

// 1e-5 (0x3727c5ac at 0x4122dc)
static float const epsilon_312920 = 9.99999975e-06f;

// whether a value is within the tolerance of zero (false for a NaN)
static inline bool near_zero_312920(float value)
{
	float magnitude = value < 0.0f ? -value : value;
	return magnitude < epsilon_312920;
}

// whether a vector's first three terms have unit length
static inline bool unit_312920(vector_312920 const &v)
{
	return near_zero_312920(dot3_312920(v, v) - 1.0f);
}

// the two entities and priority every snapshot starts with
struct snapshot_319280
{
	void *entity_a;
	void *entity_b;
	long priority;

	snapshot_319280();
};

// @retail 0x319280
snapshot_319280::snapshot_319280()
{
	entity_a = 0;
	entity_b = 0;
	priority = 0;
}

struct snapshot_31b950 : snapshot_319280
{
	long unknownc;

	snapshot_31b950();
};

// @retail 0x31b950
snapshot_31b950::snapshot_31b950()
{
}

// how much solver room a constraint takes (as havok_contact_mgr_318a80.cpp's
// constraint_info_31be80)
struct solver_info_312b30
{
	long field_0;
	long field_4;
	long field_8;
	long field_c;
};

// an hkBool returned by value: a class, so through the hidden pointer
// retail's callers pass (x8 on the port's ARM guest)
struct hkbool_312920
{
	char value;
	hkbool_312920() {}
	hkbool_312920(bool value) : value(value) {}
};

class constraint_31bbd0;

// the world-side owner of a constraint, told how much room it needs
class owner_312b80
{
public:
	virtual void slot_0() = 0;
	virtual void slot_1() = 0;
	virtual void add_solver_info(constraint_31bbd0 *constraint, solver_info_312b30 *info) = 0;
	virtual void remove_solver_info(constraint_31bbd0 *constraint, solver_info_312b30 *info) = 0;
};

// the slots of a joint's (retail) vtable the code here calls
class constraint_slots_312920
{
public:
	virtual void slot_0() = 0;
	virtual void slot_1() = 0;
	virtual void slot_2() = 0;
	virtual void slot_3() = 0;
	virtual void slot_4() = 0;
	virtual void slot_5() = 0;
	virtual void slot_6() = 0;
	virtual hkbool_312920 is_valid() = 0;
	virtual void slot_8() = 0;
	virtual void slot_9() = 0;
	virtual void solver_info(solver_info_312b30 *info) = 0;
};

// the constraint base: vtable, block size, reference count, an atom of its
// own, the two entities, the owner and the priority
class constraint_31bbd0
{
public:
	constraint_31bbd0(snapshot_31b950 const *info);

	void read_base(snapshot_31b950 *output);

	constraint_slots_312920 *slots()
	{
		return (constraint_slots_312920 *)this;
	}

	void const *vtable;
	unsigned short memory_size;
	unsigned short reference_count;
	void const *atom_vtable;
	void *entity_a;
	void *entity_b;
	owner_312b80 *owner;
	long priority;
	long unknown1c;
};

// @retail 0x31bbd0
constraint_31bbd0::constraint_31bbd0(snapshot_31b950 const *info)
{
	reference_count = 1;
	// (retail stores the atom's base vtable, 0x415340, first)
	atom_vtable = (void const *)0x415350;
	vtable = (void const *)0x415360;
	owner = 0;
	entity_a = 0;
	entity_b = 0;
	h2_retail_31bac0(this, info->entity_a);
	h2_retail_31bb30(this, info->entity_b);
	priority = info->priority;
}

// @retail 0x31b970
void constraint_31bbd0::read_base(snapshot_31b950 *output)
{
	output->entity_a = entity_a;
	output->entity_b = entity_b;
	output->priority = priority;
}

// the arrays of eight-byte runtime records each joint ends with, zeroed
static inline void zero_records_312920(void *records, int count)
{
	long *words = (long *)records;
	for (int i = 0; i < count * 2; i++)
	{
		words[i] = 0;
	}
}

// ---------------------------------------------------------------------------
// the snapshots (layouts as src/unknown_1d77d0.cpp fills them: its data[i]
// is the block at 0x10 * i, so data[1] here is its data[2])

struct c_component_joint_snapshot_0 : snapshot_31b950
{
	vector_312920 data[5];
	c_component_joint_snapshot_0();
};

struct c_component_joint_snapshot_1 : snapshot_31b950
{
	vector_312920 data[7];
	float minimum_angle;
	float maximum_angle;
	float strength;
	long unknown8c;
	c_component_joint_snapshot_1();
};

struct c_component_joint_snapshot_2 : snapshot_31b950
{
	vector_312920 data[6];
	float values[6];
	long unknown88[2];
	c_component_joint_snapshot_2();
};

struct c_component_joint_snapshot_3 : snapshot_31b950
{
	vector_312920 data[2];
	float value;
	long unknown34[3];
	c_component_joint_snapshot_3();
};

struct c_component_joint_snapshot_4 : snapshot_31b950
{
	vector_312920 data[2];
	c_component_joint_snapshot_4();
};

struct c_component_joint_snapshot_5 : snapshot_31b950
{
	vector_312920 data[8];
	float minimum;
	float maximum;
	float value;
	long unknown9c;
	c_component_joint_snapshot_5();
};

typedef char assert_snapshot_0_size[sizeof(c_component_joint_snapshot_0) == 0x60 ? 1 : -1];
typedef char assert_snapshot_1_size[sizeof(c_component_joint_snapshot_1) == 0x90 ? 1 : -1];
typedef char assert_snapshot_2_size[sizeof(c_component_joint_snapshot_2) == 0x90 ? 1 : -1];
typedef char assert_snapshot_3_size[sizeof(c_component_joint_snapshot_3) == 0x40 ? 1 : -1];
typedef char assert_snapshot_4_size[sizeof(c_component_joint_snapshot_4) == 0x30 ? 1 : -1];
typedef char assert_snapshot_5_size[sizeof(c_component_joint_snapshot_5) == 0xa0 ? 1 : -1];

// @retail 0x313b30
c_component_joint_snapshot_0::c_component_joint_snapshot_0()
{
	zero_312920(data, 5);
}

// @retail 0x313320
c_component_joint_snapshot_1::c_component_joint_snapshot_1()
{
	zero_312920(data, 7);
	minimum_angle = -3.14159274f; // 0xc0490fdb
	maximum_angle = 3.14159274f; // 0x40490fdb
	strength = 0.0f;
}

// @retail 0x312920
c_component_joint_snapshot_2::c_component_joint_snapshot_2()
{
	zero_312920(data, 6);
	values[0] = 1.0f; // 0x3f800000, the cone's cosine
	values[1] = -0.025f; // 0xbccccccd
	values[2] = 0.025f; // 0x3ccccccd
	values[3] = -0.5236f; // 0xbf060aa6
	values[4] = 0.5236f; // 0x3f060aa6
	values[5] = 0.0f; // the strength
}

// @retail 0x314860
c_component_joint_snapshot_3::c_component_joint_snapshot_3()
{
	zero_312920(data, 2);
	value = 0.0f;
}

// @retail 0x3140c0
c_component_joint_snapshot_4::c_component_joint_snapshot_4()
{
	zero_312920(data, 2);
}

// @retail 0x314bb0
c_component_joint_snapshot_5::c_component_joint_snapshot_5()
{
	zero_312920(data, 8);
	// an identity rotation in the blocks at 0x60, 0x70, 0x80
	data[5].x = 1.0f;
	data[6].y = 1.0f;
	data[7].z = 1.0f;
	minimum = -3.40282347e+38f; // 0xff7fffff
	maximum = 3.40282347e+38f; // 0x7f7fffff
	value = 0.0f;
}

// ---------------------------------------------------------------------------
// the joints

class c_component_joint_0 : public constraint_31bbd0
{
public:
	c_component_joint_0(c_component_joint_snapshot_0 const *info);
	hkbool_312920 is_valid();

	vector_312920 v20;
	vector_312920 v30;
	vector_312920 v40;
	vector_312920 v50;
	vector_312920 v60;
	long records[5][2];
	long unknown98[2];
};

class c_component_joint_1 : public constraint_31bbd0
{
public:
	c_component_joint_1(c_component_joint_snapshot_1 const *info);
	void take_info(c_component_joint_snapshot_1 const *info);
	hkbool_312920 is_valid();

	vector_312920 v20;
	vector_312920 v30;
	vector_312920 v40;
	vector_312920 v50;
	vector_312920 v60;
	vector_312920 v70;
	vector_312920 v80;
	long records[6][2];
	long unknownc0[2];
	float minimum_angle;
	float maximum_angle;
	float strength;
	long field_d4;
	long unknownd8[2];
};

class c_component_joint_2 : public constraint_31bbd0
{
public:
	c_component_joint_2(c_component_joint_snapshot_2 const *info);
	void take_info(c_component_joint_snapshot_2 const *info);
	void solver_info(solver_info_312b30 *info);

	vector_312920 v20;
	vector_312920 v30;
	vector_312920 v40;
	vector_312920 v50;
	vector_312920 v60;
	vector_312920 v70;
	// cone cosine, plane sines, twist limits, then the strength (0x94)
	float values[6];
	long records[9][2];
};

class c_component_joint_3 : public constraint_31bbd0
{
public:
	c_component_joint_3(c_component_joint_snapshot_3 const *info);

	vector_312920 v20;
	vector_312920 v30;
	float value;
	long records[1][2];
	long unknown4c;
};

class c_component_joint_4 : public constraint_31bbd0
{
public:
	c_component_joint_4(c_component_joint_snapshot_4 const *info);

	vector_312920 v20;
	vector_312920 v30;
	long records[3][2];
	long unknown58[2];
};

class c_component_joint_5 : public constraint_31bbd0
{
public:
	c_component_joint_5(c_component_joint_snapshot_5 const *info);
	void take_info(c_component_joint_snapshot_5 const *info);
	hkbool_312920 is_valid();

	vector_312920 v20;
	vector_312920 v30;
	vector_312920 v40;
	vector_312920 v50;
	vector_312920 v60;
	vector_312920 v70;
	vector_312920 v80;
	vector_312920 v90;
	float minimum;
	float maximum;
	float value;
	long records[7][2];
	long unknowne4[3];
};

typedef char assert_joint_0_size[sizeof(c_component_joint_0) == 0xa0 ? 1 : -1];
typedef char assert_joint_1_size[sizeof(c_component_joint_1) == 0xe0 ? 1 : -1];
typedef char assert_joint_2_size[sizeof(c_component_joint_2) == 0xe0 ? 1 : -1];
typedef char assert_joint_3_size[sizeof(c_component_joint_3) == 0x50 ? 1 : -1];
typedef char assert_joint_4_size[sizeof(c_component_joint_4) == 0x60 ? 1 : -1];
typedef char assert_joint_5_size[sizeof(c_component_joint_5) == 0xf0 ? 1 : -1];
typedef char assert_joint_1_records[__builtin_offsetof(c_component_joint_1, records) == 0x90 ? 1 : -1];
typedef char assert_joint_1_angles[__builtin_offsetof(c_component_joint_1, minimum_angle) == 0xc8 ? 1 : -1];
typedef char assert_joint_2_records[__builtin_offsetof(c_component_joint_2, records) == 0x98 ? 1 : -1];
typedef char assert_joint_3_records[__builtin_offsetof(c_component_joint_3, records) == 0x44 ? 1 : -1];
typedef char assert_joint_5_records[__builtin_offsetof(c_component_joint_5, records) == 0xac ? 1 : -1];

// the readers: a joint's state back into a snapshot (retail calls them on
// the joint)
struct c_component_joint_reader_0
{
	void read(c_component_joint_snapshot_0 *output);
};

struct c_component_joint_reader_1
{
	void read(c_component_joint_snapshot_1 *output);
};

struct c_component_joint_reader_2
{
	void read(c_component_joint_snapshot_2 *output);
};

struct c_component_joint_reader_4
{
	void read(c_component_joint_snapshot_4 *output);
};

class c_component_joint_strength
{
public:
	void set_strength(float strength);
};

// kind 0 ---------------------------------------------------------------------

// @retail 0x313de0
c_component_joint_0::c_component_joint_0(c_component_joint_snapshot_0 const *info)
	: constraint_31bbd0(info)
{
	vtable = (void const *)0x41490c;
	atom_vtable = (void const *)0x4146ec;
	zero_records_312920(records, 5);
	v20 = info->data[0];
	v30 = info->data[3];
	v40 = info->data[4];
	v50 = info->data[1];
	v60 = info->data[2];
	bool valid = slots()->is_valid().value != 0;
}

// @retail 0x313cc0
hkbool_312920 c_component_joint_0::is_valid()
{
	return hkbool_312920(near_zero_312920(dot3_312920(v30, v40)) && unit_312920(v30) && unit_312920(v40) && unit_312920(v60));
}

// @retail 0x313c50
void c_component_joint_reader_0::read(c_component_joint_snapshot_0 *output)
{
	c_component_joint_0 *joint = (c_component_joint_0 *)this;
	joint->read_base(output);
	output->data[0] = joint->v20;
	output->data[1] = joint->v50;
	output->data[3] = joint->v30;
	output->data[4] = joint->v40;
	output->data[2] = joint->v60;
}

// kind 1 ---------------------------------------------------------------------

// @retail 0x313550
c_component_joint_1::c_component_joint_1(c_component_joint_snapshot_1 const *info)
	: constraint_31bbd0(info)
{
	vtable = (void const *)0x414860;
	atom_vtable = (void const *)0x4146ec;
	zero_records_312920(records, 6);
	take_info(info);
}

// @retail 0x313420
void c_component_joint_1::take_info(c_component_joint_snapshot_1 const *info)
{
	minimum_angle = info->minimum_angle;
	maximum_angle = info->maximum_angle;
	strength = info->strength;
	v20 = info->data[0];
	v30 = info->data[2];
	v40 = info->data[4];
	v50 = info->data[5];
	v60 = info->data[1];
	v70 = info->data[3];
	v80 = info->data[6];
	field_d4 = 0;
	bool valid = slots()->is_valid().value != 0;
}

// @retail 0x3135f0
hkbool_312920 c_component_joint_1::is_valid()
{
	if (!near_zero_312920(dot3_312920(v40, v50)) || !near_zero_312920(dot3_312920(v70, v80)) ||
		!unit_312920(v40) || !unit_312920(v50) || !unit_312920(v70) || !unit_312920(v30))
	{
		return hkbool_312920(false);
	}
	// the angle range, at most a full turn
	float range;
	if (minimum_angle <= 0.0f && maximum_angle <= 0.0f)
	{
		range = (minimum_angle < 0.0f ? -minimum_angle : minimum_angle) + maximum_angle;
	}
	else
	{
		range = maximum_angle - minimum_angle;
	}
	return hkbool_312920(!(range > 6.28318548f)); // 0x40c90fdb at 0x4123e8
}

// @retail 0x3134a0
void c_component_joint_reader_1::read(c_component_joint_snapshot_1 *output)
{
	c_component_joint_1 *joint = (c_component_joint_1 *)this;
	joint->read_base(output);
	output->data[0] = joint->v20;
	output->data[1] = joint->v60;
	output->data[2] = joint->v30;
	output->data[3] = joint->v70;
	output->data[4] = joint->v40;
	output->data[5] = joint->v50;
	output->data[6] = joint->v80;
	output->minimum_angle = joint->minimum_angle;
	output->maximum_angle = joint->maximum_angle;
	output->strength = joint->strength;
}

// kind 2 ---------------------------------------------------------------------

// @retail 0x312d50
c_component_joint_2::c_component_joint_2(c_component_joint_snapshot_2 const *info)
	: constraint_31bbd0(info)
{
	vtable = (void const *)0x4146fc;
	atom_vtable = (void const *)0x4146ec;
	zero_records_312920(records, 9);
	take_info(info);
}

// @retail 0x312a30
void c_component_joint_2::take_info(c_component_joint_snapshot_2 const *info)
{
	v20 = info->data[0];
	v50 = info->data[1];
	v40 = info->data[4];
	v70 = info->data[5];
	v30 = info->data[2];
	v60 = info->data[3];
	for (int i = 0; i < 6; i++)
	{
		values[i] = info->values[i];
	}
}

// the solver room it needs: more with a strength (a motor) than without
// @retail 0x312b30
void c_component_joint_2::solver_info(solver_info_312b30 *info)
{
	info->field_0 = 0;
	info->field_8 = 0x54;
	info->field_4 = 0xf0;
	info->field_c = 6;
	if (!(values[5] == 0.0f))
	{
		info->field_8 = 0x6c;
		info->field_4 = 0x150;
		info->field_c = 9;
	}
}

// a new strength; the owner, when there is one, is told the room the joint
// needs before and after
// @retail 0x312b80
void c_component_joint_strength::set_strength(float strength)
{
	c_component_joint_2 *joint = (c_component_joint_2 *)this;
	if (joint->owner)
	{
		solver_info_312b30 info;
		joint->slots()->solver_info(&info);
		joint->owner->remove_solver_info(joint, &info);
		joint->values[5] = strength;
		joint->slots()->solver_info(&info);
		joint->owner->add_solver_info(joint, &info);
	}
	else
	{
		joint->values[5] = strength;
	}
}

// @retail 0x312ab0
void c_component_joint_reader_2::read(c_component_joint_snapshot_2 *output)
{
	c_component_joint_2 *joint = (c_component_joint_2 *)this;
	joint->read_base(output);
	output->data[0] = joint->v20;
	output->data[1] = joint->v50;
	output->data[4] = joint->v40;
	output->data[5] = joint->v70;
	output->data[2] = joint->v30;
	output->data[3] = joint->v60;
	for (int i = 0; i < 6; i++)
	{
		output->values[i] = joint->values[i];
	}
}

// kind 3 ---------------------------------------------------------------------

// @retail 0x314b00
c_component_joint_3::c_component_joint_3(c_component_joint_snapshot_3 const *info)
	: constraint_31bbd0(info)
{
	vtable = (void const *)0x414b4c;
	atom_vtable = (void const *)0x4146ec;
	zero_records_312920(records, 1);
	v20 = info->data[0];
	v30 = info->data[1];
	value = info->value;
}

// kind 4 ---------------------------------------------------------------------

// @retail 0x314270
c_component_joint_4::c_component_joint_4(c_component_joint_snapshot_4 const *info)
	: constraint_31bbd0(info)
{
	vtable = (void const *)0x41498c;
	atom_vtable = (void const *)0x4146ec;
	zero_records_312920(records, 3);
	v20 = info->data[0];
	v30 = info->data[1];
}

// @retail 0x3141c0
void c_component_joint_reader_4::read(c_component_joint_snapshot_4 *output)
{
	c_component_joint_4 *joint = (c_component_joint_4 *)this;
	joint->read_base(output);
	output->data[0] = joint->v20;
	output->data[1] = joint->v30;
}

// kind 5 ---------------------------------------------------------------------

// @retail 0x314ef0
c_component_joint_5::c_component_joint_5(c_component_joint_snapshot_5 const *info)
	: constraint_31bbd0(info)
{
	vtable = (void const *)0x414ca4;
	atom_vtable = (void const *)0x4146ec;
	zero_records_312920(records, 7);
	take_info(info);
}

// @retail 0x314cd0
void c_component_joint_5::take_info(c_component_joint_snapshot_5 const *info)
{
	v20 = info->data[0];
	v30 = info->data[1];
	v40 = info->data[2];
	v50 = info->data[3];
	minimum = info->minimum;
	maximum = info->maximum;
	v60 = info->data[4];
	v70 = info->data[5];
	v80 = info->data[6];
	v90 = info->data[7];
	value = info->value;
	bool valid = slots()->is_valid().value != 0;
}

// @retail 0x314e00
hkbool_312920 c_component_joint_5::is_valid()
{
	if (!near_zero_312920(dot3_312920(v50, v60)) || !unit_312920(v60) || !unit_312920(v50))
	{
		return hkbool_312920(false);
	}
	return hkbool_312920(!(minimum > maximum));
}
