// @flags /O2 /GL-
// Havok (third-party physics library): an object with a motion set from its
// description, and two free functions that have objects fill their
// descriptions.
#include "motion_description_3115a0.h"

// sets the object from the description: the base part, then the motion
// @retail 0x312260
void class_3115a0::function_312260(description_3115a0 const *description)
{
	function_30be80(description);
	function_312150(description);
}

struct description_30afc0;

class class_30afc0
{
public:
	void function_30afc0(description_30afc0 *description) const;
};

// @retail 0x312280
void __cdecl function_312280(class_30afc0 const *object, description_30afc0 *description)
{
	object->function_30afc0(description);
}

class class_30aff0
{
public:
	void function_30aff0(void const *description);
};

// @retail 0x312290
void __cdecl function_312290(class_30aff0 *object, void const *description)
{
	object->function_30aff0(description);
}
