// @flags /O2 /GL-
// Havok (third-party physics library): a body set from its description: its
// motion's kind; unless the body is fixed, its velocities (the body woken
// first when it sleeps in a group), its mass properties unless it is
// keyframed, two of the motion's words and the kind of its shared object;
// and two values of its own.
#include "motion_description_3115a0.h"
#include "unknown_1cec30.h"

// the motion with the slots this function calls
class motion_312150
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void slot_4() {}
	virtual void slot_5() {}
	// the motion's kind
	virtual int slot_6() { return 0; }
	// sets the mass
	virtual void slot_7(float mass) {}
	virtual void slot_8() {}
	virtual void slot_9() {}
	virtual void slot_10() {}
	// sets the inertia matrix
	virtual void slot_11(void const *matrix) {}
	virtual void slot_12() {}
	virtual void slot_13() {}
	virtual void slot_14() {}
	// sets the centre of mass
	virtual void slot_15(__m128 const *center) {}
	virtual void slot_16() {}
	virtual void slot_17() {}
	virtual void slot_18() {}
	virtual void slot_19() {}
	// set the linear and the angular velocity
	virtual void slot_20(__m128 const *velocity) {}
	virtual void slot_21(__m128 const *velocity) {}

	char unknown4[0x14 - 0x4];
	long field_14;
	long field_18;
};

class c_library_311690
{
public:
	void update(long value);
};

// @retail 0x312150
void class_3115a0::function_312150(description_3115a0 const *description)
{
	function_311e40(description->field_b4);
	motion_312150 *state = (motion_312150 *)motion;
	if (state->slot_6() != 7)
	{
		hkRigidBody *body = (hkRigidBody *)this;
		if (!body->body_is_simulating().m_bool && group)
		{
			body->activate();
		}
		((motion_312150 *)motion)->slot_20(&description->field_40);
		if (!body->body_is_simulating().m_bool && group)
		{
			body->activate();
		}
		((motion_312150 *)motion)->slot_21(&description->field_50);
		if (((motion_312150 *)motion)->slot_6() != 6)
		{
			((motion_312150 *)motion)->slot_11(description->field_60);
			((motion_312150 *)motion)->slot_15(&description->field_90);
			((motion_312150 *)motion)->slot_7(description->field_a0);
		}
		((motion_312150 *)motion)->field_14 = description->field_a4;
		((motion_312150 *)motion)->field_18 = description->field_a8;
		// the query is asked whether it lets the body through: when it does
		// and its answer is the one the description holds, nothing is updated,
		// so the jump leaves the update to the other cases
		if (query)
		{
			int const allowed = description->field_b5;
			if (query->slot_7() == allowed)
			{
				goto after_update;
			}
		}
		((c_library_311690 *)this)->update(description->field_b5);
	}
after_update:
	field_4c = description->field_ac;
	field_50 = description->field_b0;
}
