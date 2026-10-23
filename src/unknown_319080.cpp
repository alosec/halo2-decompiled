// @flags /O2 /GL-
// Havok (third-party physics library): an object that keeps the object it
// was made for, with one reference.
#include "owned_reference_318b90.h"

// @retail 0x319080
class_319080::class_319080(void *owner)
	: owner(owner)
{
}
