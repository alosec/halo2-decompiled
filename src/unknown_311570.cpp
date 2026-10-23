// @flags /O2 /GL-
// Havok (third-party physics library): the deleting destructor of an object
// given back with tag 0x28, and an object with a motion filling its
// description.
#include "tagged_objects_2e4ff0.h"
#include "motion_description_3115a0.h"

// @retail 0x311570 deleting class_311410

// fills the description from the object, then from its motion: its vectors,
// its yes-or-no answer and whatever its ninth slot writes, the inverse of its
// mass (none when the mass is 0), its counters, and whether the query lets it
// (yes when there is none)
// @retail 0x3115a0
void class_3115a0::function_3115a0(description_3115a0 *description) const
{
	function_30bd90(description);
	description->field_20 = motion->field_b0;
	description->field_30 = motion->field_30;
	description->field_40 = motion->field_40;
	description->field_50 = motion->field_50;
	description->field_b4 = motion->slot_6();
	motion->slot_9(description->field_60);
	float mass = motion->field_20.m128_f32[3];
	description->field_a0 = mass == 0.0f ? 0.0f : 1.0f / mass;
	description->field_90 = motion->field_20;
	description->field_ac = field_4c;
	description->field_a4 = motion->field_14;
	description->field_a8 = motion->field_18;
	description->field_b0 = field_50;
	query_3115a0 *const asker = query;
	int allowed;
	if (asker)
	{
		allowed = asker->slot_7();
	}
	else
	{
		allowed = 1;
	}
	description->field_b5 = allowed;
}
