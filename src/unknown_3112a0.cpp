// @flags /O2 /GL-
// Havok (third-party physics library): constructing a world description in
// a block, when there is one.
#include <new>
#include "motion_description_3115a0.h"

struct s_3111d0
{
	s_3111d0();
};

// @retail 0x3112a0
void __cdecl function_3112a0(void *block)
{
	new (block) s_3111d0;
}

// @retail 0x3112b0
void __cdecl function_3112b0(class_3115a0 const *object, description_3115a0 *description)
{
	object->function_3115a0(description);
}

// @retail 0x3112c0
void __cdecl function_3112c0(class_3115a0 *object, description_3115a0 const *description)
{
	object->function_312260(description);
}
