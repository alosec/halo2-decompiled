// @flags /O2 /GL- /GX
// Havok (third-party physics library): creates a kind of bounded object
// (which takes the value as its fourth argument).
#include "bounded_2e9530.h"

class bounded_2f9bb0 : public bounded_2e9530
{
public:
	bounded_2f9bb0(long value_) : bounded_2e9530(value_) {}
};

// @retail 0x2f9bb0
bounded_2f9bb0 *__cdecl function_2f9bb0(long arg_0, long arg_1, long arg_2, long value)
{
	return new bounded_2f9bb0(value);
}
