#ifndef PAIR_FILTER_CLASS_H
#define PAIR_FILTER_CLASS_H
#include "unknown_244980.h"
#include "unknown_1efac0.h"
#include "unknown_1cec30.h"
#include "physics_allocator.h"

class keyed_2df8f0;
struct kinds_2dfa20;
struct chain_2dfa20;
struct pair_filter_settings_2dfb30;
struct object_48039c;
extern object_48039c g_48039c;

class pair_object_filter
{
public:
	virtual hkBool function_2df9d0(void const *, void const *) = 0;
};
class pair_collection_filter
{
public:
	virtual hkBool function_2dfa20(kinds_2dfa20 const *const *, chain_2dfa20 const *, long, keyed_2df8f0 const *, long) = 0;
	virtual ~pair_collection_filter() {}
};
class pair_ray_collection_filter
{
public:
	virtual hkBool function_2df8f0(void const *, void const *, long) = 0;
	virtual ~pair_ray_collection_filter() {}
};
class pair_ray_filter
{
public:
	virtual hkBool function_2dfae0(void const *, void const *) = 0;
};

class c_contact_rule_fallback : public c_244980_base
{
public:
	c_contact_rule_fallback() { unknown06 = 1; }
	hkBool accepts(long a, long b);
};

// The intermediate collision-filter owns the interface subobjects. Its
// destructor unwinds them before the reference-counted c_a subobject.
class pair_filter_base : public c_contact_rule_fallback,
	public pair_object_filter, public pair_collection_filter,
	public pair_ray_collection_filter, public pair_ray_filter
{
public:
	virtual hkBool function_2df9d0(void const *, void const *) = 0;
	virtual hkBool function_2dfa20(kinds_2dfa20 const *const *, chain_2dfa20 const *, long, keyed_2df8f0 const *, long) = 0;
	virtual hkBool function_2df8f0(void const *, void const *, long) = 0;
	virtual hkBool function_2dfae0(void const *, void const *) = 0;
	static void __cdecl operator delete(void *block)
	{
		g_480118->allocate((long)block, ((pair_filter_base *)block)->flags, 0x22);
	}
};

class c_2dfbf0 : public pair_filter_base
{
public:
	long field_18;
	unsigned long enabled[32];
	c_2dfbf0();
	c_2dfbf0(pair_filter_settings_2dfb30 const &settings);
	static void *operator new(unsigned int size)
	{
		c_2dfbf0 *object = (c_2dfbf0 *)havok_allocator()->allocate_4(size, 0x22);
		object->flags = (unsigned short)size;
		return object;
	}
	void function_2df950(long arg_0, long arg_1);
	void function_2df990(long arg_0, long arg_1);
	virtual void *v3() { return &g_48039c; }
	virtual hkBool function_2df9d0(void const *, void const *);
	virtual hkBool function_2df8f0(void const *, void const *, long);
	virtual hkBool function_2dfae0(void const *, void const *);
	virtual hkBool function_2dfa20(kinds_2dfa20 const *const *, chain_2dfa20 const *, long, keyed_2df8f0 const *, long);
	__forceinline hkBool test(long a, long b) const
	{
		hkBool result;
		if (((a ^ b) & 0xffff0000) == 0 && (a & 0xffff0000) != 0)
		{
			result.m_bool = false;
			return result;
		}
		result.m_bool = (enabled[a & 31] & (1 << (b & 31))) != 0;
		return result;
	}
};
#endif
