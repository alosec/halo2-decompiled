// @flags /O2 /GL-
// Havok (third-party physics library): the destructor of an object built on
// the reference object and two interfaces; it does nothing of its own.
#include "unknown_0a9f20.h"

class valued_30d280 : public c_reference_a9f20
{
public:
	long field_8;
};

class first_30d280
{
public:
	virtual ~first_30d280() {}
};

class second_30d280
{
public:
	virtual ~second_30d280() {}
};

class class_30d280 : public valued_30d280, public first_30d280, public second_30d280
{
};

// runs the destructors of the object's bases one by one, without its own
// @retail 0x30d280
void __fastcall function_30d280(class_30d280 *object)
{
	static_cast<second_30d280 *>(object)->second_30d280::~second_30d280();
	static_cast<first_30d280 *>(object)->first_30d280::~first_30d280();
	static_cast<c_reference_a9f20 *>(object)->c_reference_a9f20::~c_reference_a9f20();
}
