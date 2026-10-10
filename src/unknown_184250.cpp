// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0259d0.h"
#include "unknown_184250.h"
#if !defined(H2_NATIVE_PORT)
#include <xmmintrin.h>
#endif

struct s_type_1e6529;
class c_havok_reference_counted;
extern c_havok_reference_counted *g_4f55b0;
extern byte *g_4ed280;

struct s_184250
{
	long field_0;
	byte field_4[0x30 - 4];
	point3f field_30;
};

struct s_184252
{
	byte field_0[4];
	real field_4;
	byte field_8[0x20 - 8];
	real field_20;
	real field_24;
};

struct __declspec(align(16)) s_184253
{
	real field_0;
	real field_4;
	real field_8;
	real field_c;
};

struct s_184254
{
	long *field_0;
	long field_4;
	dword field_8;
	~s_184254()
	{
		if (!(field_8 & 0x80000000))
			g_480118->allocate((long)field_0, (field_8 & 0x7fffffff) * 4, 0x12);
	}
};

struct s_184257
{
	s_184254 field_0;
	long field_c[0x200];
};

class c_184250
{
public:
	virtual void function_184251() {}
	virtual void function_184252() {}
	virtual void function_184253() {}
	virtual void function_184254() {}
	virtual void function_184255() {}
	virtual void function_184256() {}
	virtual void function_184257() {}
	virtual void function_184258() {}
	virtual void function_184259() {}
	virtual void function_18425a() {}
	virtual void function_18425b(s_184253 const *, s_184254 *) {}
};

struct s_184255
{
	short field_0;
	short field_2;
	long field_4;
	byte field_8[0x20 - 8];
};

struct s_184256
{
	byte field_0[0x210];
	s_184255 *field_210;
};

void voice_fpu_enter(void);
void voice_fpu_leave(void);
long function_184000(long arg_0, long arg_1);
void function_184060(long arg_0, unsigned char arg_1, s_type_1e6529 *arg_2, long arg_3);

// @retail 0x184250
void __stdcall function_184250(s_type_1e6529 const *arg_0)
{
	if (!*g_4ed280)
		return;
	s_184250 const *local_0 = (s_184250 const *)arg_0;
	s_184252 const *local_1 = (s_184252 const *)g_4e3b44[local_0->field_0 & 0xffff].bytes;
	if (local_1->field_20 == 0.0f && local_1->field_24 == 0.0f)
		return;
	real local_2 = local_1->field_4;
	bool local_3 = false;
	if (g_4f55b0)
	{
		s_184253 local_4;
		local_4.field_0 = local_0->field_30.x;
		local_4.field_4 = local_0->field_30.y;
		local_4.field_8 = local_0->field_30.z;
		local_4.field_c = 0.0f;
		{
			s_184257 local_5;
			s_184254 &local_6 = local_5.field_0;
			local_6.field_0 = local_5.field_c;
			local_6.field_4 = 0;
			local_6.field_8 = 0x80000200;
			voice_fpu_enter();
			s_184253 local_7;
#if defined(H2_NATIVE_PORT)
			local_7 = local_4;
#else
			_mm_store_ps(&local_7.field_0, _mm_load_ps(&local_4.field_0));
#endif
			local_7.field_c = local_2 < 0.0f ? 0.0f : local_2;
			((c_184250 *)g_4f55b0)->function_18425b(&local_7, &local_6);
			voice_fpu_leave();
			for (long local_8 = 0; local_8 < local_6.field_4; local_8++)
			{
				s_184255 *local_9 = &((s_184256 *)g_4e0348)->field_210[local_6.field_0[local_8]];
				long local_10 = local_9->field_0;
				long local_11 = local_9->field_2;
				if ((bool)(byte)function_184000(local_10, local_11))
				{
					function_184060(local_10, (byte)local_11, (s_type_1e6529 *)arg_0, local_9->field_4);
					local_3 = true;
				}
			}
		}
		if (local_3)
		{
			s_184251 local_12;
			local_12.field_0 = false;
			function_2e90a0(local_12);
		}
	}
}
