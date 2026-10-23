#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1cec30.h"
// @flags /O2 /arch:SSE /Gr

class c_278372
{
public:
	virtual void function_278371() {}
	virtual void function_278372() {}
	virtual void function_278373() {}
	virtual void function_278374() {}
	virtual void function_278375() {}
	virtual long function_278376() { return 0; }
	byte field_4[0x1a - 4];
	byte field_1a;
	byte field_1b;
};

struct s_278370
{
	c_278372 *field_0;
	byte field_4[8];
	s_278370 *field_c;
	byte field_10[8];
	long field_18;
	long field_1c;
	hkEntity *field_20;
	hkEntity *function_27838d();
};

struct s_278371
{
	long field_0;
	void *field_4;
	s_278370 *field_8;
	s_278370 *field_c;
	void *field_10;
};

struct s_278373
{
	real field_0, field_4, field_8, field_c;
	s_278373() : field_0(0.0f), field_4(0.0f), field_8(0.0f), field_c(0.0f) {}
};

class c_278370
{
public:
	virtual void function_278370(s_278371 *arg_0);
};

long havok_entity_component_index_get(hkEntity const *entity);
void __cdecl function_3153c0(void *arg_0, void *arg_1, void const *arg_2);

__forceinline hkEntity *s_278370::function_27838d()
{
	s_278370 *local_0 = this;
	for (;;)
	{
		s_278370 *const &next = local_0->field_c;
		if (!next)
			break;
		local_0 = next;
	}
	return local_0->field_18 == 1 ? local_0->field_20 : NULL;
}

extern "C" void _ReadWriteBarrier(void);
#pragma intrinsic(_ReadWriteBarrier)
// @retail 0x278370
void c_278370::function_278370(s_278371 *arg_0)
{
	s_278370 *const &local_6 = arg_0->field_8;
	hkEntity *local_0 = local_6->function_27838d();
	hkEntity *local_1 = arg_0->field_c->function_27838d();
	if (local_0 && local_1 && *((byte *)local_0 + 0x48) == 1 && *((byte *)local_1 + 0x48) == 1 &&
		(local_6->field_0->function_278376() == 0x18 || arg_0->field_c->field_0->function_278376() == 0x18))
	{
		bool local_2 = arg_0->field_8->field_0->function_278376() != 0x18;
		c_278372 *local_3 = local_2 ? arg_0->field_c->field_0 : arg_0->field_8->field_0;
		_ReadWriteBarrier();
		long local_4 = havok_entity_component_index_get(local_2 ? local_0 : local_1);
		if (!(bool)((havok_component_get(local_4)->unknown04 >> 18) & 1) &&
			(local_3->field_1a & 0x20) && local_3->field_1b)
		{
#ifndef H2_NATIVE_PORT
			static s_278373 local_5;
#else
			// Avoid the C++ thread-safe-static runtime in the freestanding guest.
			// This value is passed as const and is exactly zero-initialized per call.
			s_278373 local_5;
#endif
			function_3153c0(arg_0->field_4, arg_0->field_10, &local_5);
		}
	}
}
#pragma function(_ReadWriteBarrier)
