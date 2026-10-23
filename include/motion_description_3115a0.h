#ifndef MOTION_DESCRIPTION_3115A0_H
#define MOTION_DESCRIPTION_3115A0_H
// Havok (third-party physics library): an object built on 0x30bd90's that
// also keeps a motion (its position, rotation and velocities as vectors, an
// inverse mass in the last lane of one, two counters and a yes-or-no query
// through its own table) copies all of it into a description (0x3115a0) and
// back (0x312260).
#include "description_312430.h"
#include <xmmintrin.h>

// the table and the plain fields come first, so that the table does not
// take 16 bytes as it would in a class holding vectors itself
class motion_header_3115a0
{
public:
	virtual void slot_0() = 0;
	virtual void slot_1() = 0;
	virtual void slot_2() = 0;
	virtual void slot_3() = 0;
	virtual void slot_4() = 0;
	virtual void slot_5() = 0;
	virtual bool slot_6() = 0;
	virtual void slot_7() = 0;
	virtual void slot_8() = 0;
	virtual void slot_9(void *out) = 0;

	char unknown4[0x14 - 0x4];
	long field_14;
	long field_18;
	char unknown1c[0x20 - 0x1c];
};

class motion_3115a0 : public motion_header_3115a0
{
public:
	__m128 field_20;
	__m128 field_30;
	__m128 field_40;
	__m128 field_50;
	char unknown60[0xb0 - 0x60];
	__m128 field_b0;
};

class query_3115a0
{
public:
	virtual void slot_0() = 0;
	virtual void slot_1() = 0;
	virtual void slot_2() = 0;
	virtual void slot_3() = 0;
	virtual void slot_4() = 0;
	virtual void slot_5() = 0;
	virtual void slot_6() = 0;
	virtual int slot_7() = 0;
};

struct description_3115a0 : description_30bd90
{
	__m128 field_20;
	__m128 field_30;
	__m128 field_40;
	__m128 field_50;
	__m128 field_60[3];
	__m128 field_90;
	float field_a0;
	long field_a4;
	long field_a8;
	long field_ac;
	long field_b0;
	char field_b4;
	char field_b5;
};

class class_3115a0 : public class_30bd90
{
public:
	// fills the description from the object
	void function_3115a0(description_3115a0 *description) const;
	// sets the object from the description
	void function_312260(description_3115a0 const *description);
	void function_312150(description_3115a0 const *description);
	// changes the motion's kind (not decompiled)
	void function_311e40(int kind);
};

#endif
