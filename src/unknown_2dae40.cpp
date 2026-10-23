// @flags /O2 /GL- /GX
// Havok (third-party physics library): creating the object kept for a pair of
// convex shapes, of a kind that adds a cleared word after it (0x2daec0 looks
// for this function in the pair table).
#include "pair_agent_2ed170.h"

// the object as its constructor's file (0x2ee2e0) declares it, with its full
// size
class convex_pair_2ee2e0 : public sized_2ed170
{
public:
	char unknownc[0xf0 - 0xc];

	convex_pair_2ee2e0(description_2ed170 const *first, description_2ed170 const *second, long arg_3);
};

class convex_pair_2dae40 : public convex_pair_2ee2e0
{
public:
	long field_f0;
	char unknownf4[0x100 - 0xf4];

	__forceinline convex_pair_2dae40(description_2ed170 const *first, description_2ed170 const *second, long arg_3) :
		convex_pair_2ee2e0(first, second, arg_3)
	{
		field_f0 = 0;
	}
};

// @retail 0x2dae40
void *__cdecl function_2dae40(long arg_0, long arg_1, long arg_2, long value)
{
	return new convex_pair_2dae40((description_2ed170 const *)arg_0, (description_2ed170 const *)arg_1, value);
}
