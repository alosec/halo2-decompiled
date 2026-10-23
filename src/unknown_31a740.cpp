// @flags /O2 /GL-
// Havok (third-party physics library): the destructor of an object that adds
// nothing to destroy to the reference-counted base (0x31a710 calls it).
#include "reference_base_318970.h"

class class_31a740 : public reference_base_318970
{
};

// @retail 0x31a740 destructor class_31a740
