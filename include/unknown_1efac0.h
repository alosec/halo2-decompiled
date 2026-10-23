/* UNKNOWN_1EFAC0.H: a small class hierarchy (c_b <- c_a <- c_d); c_a's
   enable/disable virtual lives in code not decompiled here (stubbed) */

#ifndef UNKNOWN_1EFAC0_H
#define UNKNOWN_1EFAC0_H

#include "unknown_11c920.h"
#include "globals.h"

struct c_b
{
	virtual ~c_b() {}

	void operator delete(void *block)
	{
		/* slot 5 of the allocator at 0x480118 also frees: (block, size, tag) */
		g_480118->allocate((long)block, 0xc, 0x22);
	}
};

struct c_a : c_b
{
	word flags;
	word unknown06;

	virtual ~c_a()
	{
		if (flags & 0x8000)
		{
			flags &= 0x7fff;
			v2(false);
		}
	}

	virtual long v1(long, long) { return 0; }
	virtual void v2(bool enable);
	virtual void *v3() { return 0; }
	virtual void v4(dword unused) {}
};

/* the object's tag chain (0x1efb40) */
struct s_tag_ref_data;
struct s_lookup
{
	long handle;
	s_tag_ref_data *tag_a;
	s_tag_ref_data *tag_b;
	void *pointer_a;
	void *pointer_b;

	bool initialize(long handle);
};

#endif
