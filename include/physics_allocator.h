/* PHYSICS_ALLOCATOR.H: the allocator Havok code reaches through g_480118, seen
   from Havok's side (its slots as Havok's own code calls them) */

#ifndef PHYSICS_ALLOCATOR_H
#define PHYSICS_ALLOCATOR_H

class c_allocator;
extern c_allocator *g_480118;

/* slot 0 and slot 4 hand out a block of `size` bytes for a usage tag; slot 5
   gives a block of `size` bytes back */
class c_havok_allocator
{
public:
	virtual void *allocate(long size, long tag) = 0;
	virtual void slot1() = 0;
	virtual void slot2() = 0;
	virtual void slot3() = 0;
	virtual void *allocate_4(long size, long tag) = 0;
	virtual void release(void *block, long size, long tag) = 0;
};

inline c_havok_allocator *havok_allocator()
{
	return (c_havok_allocator *)g_480118;
}

#endif
