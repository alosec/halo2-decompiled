// @flags /O2 /GL-
// Library code: the Rockall memory allocator (Microsoft Research) the game
// links for its network page allocator (src/unknown_081780.cpp), all of it
// from 0x329d30 to 0x330310. Retail's failure strings name the classes:
// ROCKALL_FRONT_END, HEAP, NEW_PAGES, FIND, CACHE, BUCKET, PAGE, SPINLOCK,
// SHARELOCK and THREAD_SAFE. Recovered elsewhere and not repeated: 0x329d20
// and 0x329e10 (rockall_page_heap_329d20.cpp), the deleting destructor
// 0x329e20 (idiom_functions_generated.cpp), 0x32ae90 and 0x32ee20
// (retail_initializers.cpp); those two are called by retail address.
//
//   the back end (vtable 0x41661c: 0x329d30 DeleteArea, 0x329d60
//   NaturalSize, 0x329d80 NewArea, 0x329e20 deleting destructor): pages from
//   VirtualAlloc. The network allocator's back end is the page heap over
//   g_4d87fc (vtable 0x454970, c_page_heap in src/unknown_1a1940.cpp).
//
//   the front end (ROCKALL_FRONT_END, c_library_32a850, 0x34 bytes, vtable
//   0x454980)
//     0x04 the cache pointers (one per cache)
//     0x08 the caches (0x88 bytes each)
//     0x0c the heap (0x58 bytes)        0x10 the new pages (0x50 bytes)
//     0x14 its own find (0x7c bytes)    0x18 the shared find (0x480ef8), when
//                                            0x24 (global delete) is set
//     0x1c the back end                 0x20 the thread lock (0x24 bytes)
//     0x24 global delete                0x28 0xdeadbeef while it is valid
//     0x2c the cache count              0x30 the size of the one block the
//                                            constructor takes from the back end
//   That block holds, in order: the thread lock, the caches, the cache
//   pointers, the find, the new pages and the heap (0x8c per cache + 0x148).
//
//   its vtable 0x454980 (each public slot runs under __try in retail):
//     0 0x329e40 Delete          1 0x329f70 Details      2 0x32a020 KnownArea
//     3 0x32a140 MultipleDelete  4 0x32a1f0 MultipleNew  5 0x32a2c0 New
//     6 0x32a370 Resize          7 0x32a630 Verify       8 0x329ef0 DeleteAll
//     9 0x32a0c0 LockAll        10 0x32a510 Truncate    11 0x32a5b0 UnlockAll
//    12 0x32a6f0 Walk           13 0x32ae00 deleting destructor (0x32ac90)
//    14 0x72c70  Exception (empty, src/unknown_072c70.cpp)
//    15 0x32a460 SpecialNew
//   The __except handlers (filter 1) clear 0x28 and call slot 14 with
//   "(unknown exception type)"; nothing in the port raises a structured
//   exception, so the bodies are recovered without them.
//
//   The heap (0x58 bytes) routes a size to a cache through two tables (one
//   per stride), keeps the caches, the top cache (pages of the largest size)
//   and the external cache (larger allocations). A cache (0x88 bytes) is a
//   bucket (0x58 bytes, the pages of one allocation size and their bit
//   vectors) with a stack of freed and of ready allocations in front. The new
//   pages (0x50 bytes) keep the page descriptions per bit vector size and
//   every block taken from the back end. The find (0x7c bytes) hashes page
//   addresses to their descriptions, with a lookaside of recent pages.
//
//   the locks: the spinlock (0x14 bytes), the share lock (0x20 bytes, shared
//   and exclusive claims) and the thread lock (THREAD_SAFE, 0x24 bytes: a
//   spinlock and the thread that holds every lock at once, which then skips
//   the inner locks). Claiming and releasing them are inlined everywhere.
//
// Retail constructs the members in place with null checks on the
// placement new; those are left out (the pointers are never null there).
// Two prefetch loops (0x32d77e, 0x32e615, on the CPU flag at 0x462cc0) are
// left out. A bucket keeps two of its arithmetic routines as member
// function pointers (0x40, 0x48); the port stores the native functions.
// The tables are the retail ones, stored by address.
#include "unknown_11c920.h"
#include <xtl.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

extern "C" void free(void *block);
extern "C" void __stdcall OutputDebugStringA(char const *text);
extern "C" void __stdcall RaiseException(unsigned long code, unsigned long flags, unsigned long count,
	unsigned long const *arguments);

// recovered in retail_initializers.cpp: the shared state's constructor (the
// first counts the objects at 0x480fbc and sets the page limits after it)
// and the spinlock's (max spins, max users)
extern "C" void h2_retail_32ee20(void *self);
extern "C" void h2_retail_32ae90(void *self, unsigned int arg0, unsigned int arg1);

struct s_rockall_list;
struct s_rockall_thread_lock;
struct c_rockall_bucket;
struct c_rockall_cache;
struct c_rockall_find;
struct c_rockall_heap;
struct c_rockall_new_pages;
struct c_rockall_page;

#define k_rockall_no_parent ((c_rockall_cache *)-1)

// ---------------------------------------------------------------------------
// failures and small helpers

// @retail 0x32ae20
extern "C" void rockall_debug_print_32ae20(char const *format, ...)
{
	char text[0x200];
	va_list arguments;
	va_start(arguments, format);
	_vsnprintf(text, 0x1ff, format, arguments);
	va_end(arguments);
	text[0x1ff] = 0;
	OutputDebugStringA(text);
}

// @retail 0x32ae60
extern "C" void rockall_failure_32ae60(char const *message, bool report)
{
	if (report)
	{
		rockall_debug_print_32ae20("*** Software Failure: %s ***\n", message);
	}
	// non-continuable, the message as its one argument
	RaiseException(1, 0, 1, (unsigned long const *)message);
}

// a power of two (or zero)
// @retail 0x32ee00
extern "C" bool rockall_power_of_two_32ee00(long value)
{
	return ((value - 1) & value) == 0;
}

// the base two logarithm of a power of two
// @retail 0x32edd0
extern "C" bool rockall_log2_32edd0(long value, long *log)
{
	if (value <= 0)
	{
		return false;
	}
	long count = 0;
	while (!(value & 1))
	{
		value >>= 1;
		count++;
	}
	if (value != 1)
	{
		return false;
	}
	*log = count;
	return true;
}

// the shared state's destructor: one object fewer
// @retail 0x32ee60
extern "C" void rockall_environment_destruct_32ee60(void *self)
{
	(void)self;
	__atomic_sub_fetch((volatile long *)0x480fbc, 1, __ATOMIC_SEQ_CST);
}

// the smallest page any back end may give (set by 0x32ee20)
#define g_rockall_minimum_page (*(long *)0x480fcc)

// the page hash (FIND)
static inline unsigned long rockall_hash(unsigned long key, long shift, unsigned long mask)
{
	return ((key * 0xb0b38edbu) >> (byte)shift) & mask;
}

// ---------------------------------------------------------------------------
// the spinlock

struct s_rockall_spinlock
{
	bool wait_for_lock(long sleep);
	void wake_all_sleepers();
	bool update_semaphore();
	void destruct();

	long max_spins;
	long max_users;
	HANDLE semaphore;
	volatile long spinlock;
	volatile long waiting;
};

typedef char assert_s_rockall_spinlock_size[sizeof(s_rockall_spinlock) == 0x14 ? 1 : -1];

// how many threads are making a spinlock's semaphore
#define g_rockall_semaphore_makers (*(volatile long *)0x480fb8)

static inline void rockall_claim_lock(s_rockall_spinlock *lock)
{
	if (!__sync_bool_compare_and_swap(&lock->spinlock, 0, 1))
	{
		lock->wait_for_lock(-1);
	}
}

static inline void rockall_release_lock(s_rockall_spinlock *lock)
{
	__atomic_exchange_n(&lock->spinlock, 0, __ATOMIC_SEQ_CST);
	if (lock->waiting > 0)
	{
		lock->wake_all_sleepers();
	}
}

// @retail 0x32aec0
bool s_rockall_spinlock::update_semaphore()
{
	while (semaphore == 0)
	{
		if (__atomic_add_fetch(&g_rockall_semaphore_makers, 1, __ATOMIC_SEQ_CST) == 1 && semaphore == 0)
		{
			semaphore = CreateSemaphoreA(0, 0, max_users, 0);
			__atomic_sub_fetch(&g_rockall_semaphore_makers, 1, __ATOMIC_SEQ_CST);
			return semaphore != 0;
		}
		__atomic_sub_fetch(&g_rockall_semaphore_makers, 1, __ATOMIC_SEQ_CST);
		Sleep(1);
	}
	return true;
}

// @retail 0x32af40
bool s_rockall_spinlock::wait_for_lock(long sleep)
{
	do
	{
		if (semaphore != 0 || update_semaphore())
		{
			__atomic_add_fetch(&waiting, 1, __ATOMIC_SEQ_CST);
			if (__sync_bool_compare_and_swap(&spinlock, 0, 1))
			{
				__atomic_sub_fetch(&waiting, 1, __ATOMIC_SEQ_CST);
				return true;
			}
			if (WaitForSingleObject(semaphore, (DWORD)sleep) != 0)
			{
				return false;
			}
		}
	} while (!__sync_bool_compare_and_swap(&spinlock, 0, 1));
	return true;
}

// @retail 0x32aff0
void s_rockall_spinlock::wake_all_sleepers()
{
	long wakeup = __atomic_exchange_n(&waiting, 0, __ATOMIC_SEQ_CST);
	if (wakeup > 0)
	{
		// one at a time: the rest stay counted as waiting
		if (wakeup > 1)
		{
			__atomic_fetch_add(&waiting, wakeup - 1, __ATOMIC_SEQ_CST);
			wakeup = 1;
		}
		if (!ReleaseSemaphore(semaphore, wakeup, 0))
		{
			rockall_failure_32ae60("Wakeup failed in WakeAllSleepers()", true);
		}
	}
	else if (wakeup < 0)
	{
		__atomic_fetch_add(&waiting, wakeup, __ATOMIC_SEQ_CST);
	}
}

// @retail 0x32b070
void s_rockall_spinlock::destruct()
{
	if (semaphore != 0 && !CloseHandle(semaphore))
	{
		rockall_failure_32ae60("Close semaphore in destructor for SPINLOCK", true);
	}
	rockall_environment_destruct_32ee60(this);
}

// ---------------------------------------------------------------------------
// the share lock: many sharers or one exclusive user

struct s_rockall_sharelock
{
	s_rockall_sharelock *construct(long new_max_spins, long new_max_users);
	void destruct();
	bool update_semaphore(HANDLE *handle);
	void wake_all_sleepers();
	bool wait_for_exclusive_lock(long sleep);
	bool wait_for_share_lock(long sleep);

	long max_spins;
	long max_users;
	volatile long exclusive;
	volatile long users;
	HANDLE normal_semaphore;
	volatile long normal_waiting;
	HANDLE priority_semaphore;
	volatile long priority_waiting;
};

typedef char assert_s_rockall_sharelock_size[sizeof(s_rockall_sharelock) == 0x20 ? 1 : -1];

// how many threads are making a share lock's semaphore
#define g_rockall_share_semaphore_makers (*(volatile long *)0x480fd0)

static inline void rockall_wake_share(s_rockall_sharelock *lock)
{
	if (lock->priority_waiting > 0 || lock->normal_waiting > 0)
	{
		lock->wake_all_sleepers();
	}
}

static inline void rockall_claim_share(s_rockall_sharelock *lock)
{
	__atomic_add_fetch(&lock->users, 1, __ATOMIC_SEQ_CST);
	if (lock->exclusive > 0 || lock->users > lock->max_users)
	{
		lock->wait_for_share_lock(-1);
	}
}

static inline void rockall_release_share(s_rockall_sharelock *lock)
{
	__atomic_sub_fetch(&lock->users, 1, __ATOMIC_SEQ_CST);
	rockall_wake_share(lock);
}

static inline void rockall_claim_exclusive(s_rockall_sharelock *lock)
{
	__atomic_add_fetch(&lock->users, 1, __ATOMIC_SEQ_CST);
	__atomic_add_fetch(&lock->exclusive, 1, __ATOMIC_SEQ_CST);
	if (lock->users != 1)
	{
		lock->wait_for_exclusive_lock(-1);
	}
}

static inline void rockall_release_exclusive(s_rockall_sharelock *lock)
{
	__atomic_sub_fetch(&lock->exclusive, 1, __ATOMIC_SEQ_CST);
	__atomic_sub_fetch(&lock->users, 1, __ATOMIC_SEQ_CST);
	rockall_wake_share(lock);
}

// a shared claim made exclusive
static inline void rockall_share_to_exclusive(s_rockall_sharelock *lock)
{
	__atomic_add_fetch(&lock->exclusive, 1, __ATOMIC_SEQ_CST);
	if (lock->users != 1)
	{
		lock->wait_for_exclusive_lock(-1);
	}
}

// @retail 0x32fea0
s_rockall_sharelock *s_rockall_sharelock::construct(long new_max_spins, long new_max_users)
{
	h2_retail_32ee20(this);
	exclusive = 0;
	users = 0;
	normal_waiting = 0;
	normal_semaphore = 0;
	priority_semaphore = 0;
	priority_waiting = 0;
	if (new_max_spins > 0)
	{
		max_spins = new_max_spins;
	}
	else
	{
		rockall_failure_32ae60("Maximum spins invalid in constructor for SHARELOCK", true);
	}
	if (new_max_users > 0 && new_max_users <= 0x100)
	{
		max_users = new_max_users;
	}
	else
	{
		rockall_failure_32ae60("Maximum share invalid in constructor for SHARELOCK", true);
	}
	return this;
}

// @retail 0x32ff50
bool s_rockall_sharelock::update_semaphore(HANDLE *handle)
{
	while (*handle == 0)
	{
		if (__atomic_add_fetch(&g_rockall_share_semaphore_makers, 1, __ATOMIC_SEQ_CST) == 1 && *handle == 0)
		{
			*handle = CreateSemaphoreA(0, 0, 0x100, 0);
			__atomic_sub_fetch(&g_rockall_share_semaphore_makers, 1, __ATOMIC_SEQ_CST);
			return *handle != 0;
		}
		__atomic_sub_fetch(&g_rockall_share_semaphore_makers, 1, __ATOMIC_SEQ_CST);
		Sleep(1);
	}
	return true;
}

// @retail 0x32ffd0
void s_rockall_sharelock::wake_all_sleepers()
{
	// the exclusive waiters first
	long priority = __atomic_exchange_n(&priority_waiting, 0, __ATOMIC_SEQ_CST);
	if (priority > 0)
	{
		if (priority > 1)
		{
			__atomic_fetch_add(&priority_waiting, priority - 1, __ATOMIC_SEQ_CST);
			priority = 1;
		}
		if (!ReleaseSemaphore(priority_semaphore, priority, 0))
		{
			rockall_failure_32ae60("Priority wakeup failed in WakeAllSleepers()", true);
		}
		return;
	}
	long normal = __atomic_exchange_n(&normal_waiting, 0, __ATOMIC_SEQ_CST);
	if (normal > 0)
	{
		if (normal > 1)
		{
			__atomic_fetch_add(&normal_waiting, normal - 1, __ATOMIC_SEQ_CST);
			normal = 1;
		}
		if (!ReleaseSemaphore(normal_semaphore, normal, 0))
		{
			rockall_failure_32ae60("Normal wakeup failed in WakeAllSleepers()", true);
		}
	}
	else if (normal < 0)
	{
		__atomic_fetch_add(&normal_waiting, normal, __ATOMIC_SEQ_CST);
	}
	if (priority < 0)
	{
		__atomic_fetch_add(&priority_waiting, priority, __ATOMIC_SEQ_CST);
	}
}

// @retail 0x3300d0
void s_rockall_sharelock::destruct()
{
	if (priority_semaphore != 0 && !CloseHandle(priority_semaphore))
	{
		rockall_failure_32ae60("Close semaphore in destructor for SHARELOCK", true);
	}
	if (normal_semaphore != 0 && !CloseHandle(normal_semaphore))
	{
		rockall_failure_32ae60("Close semaphore in destructor for SHARELOCK", true);
	}
	rockall_environment_destruct_32ee60(this);
}

// entered with the claim counted (users and exclusive)
// @retail 0x330160
bool s_rockall_sharelock::wait_for_exclusive_lock(long sleep)
{
	while (users != 1)
	{
		__atomic_sub_fetch(&users, 1, __ATOMIC_SEQ_CST);
		if (sleep == 0)
		{
			__atomic_sub_fetch(&exclusive, 1, __ATOMIC_SEQ_CST);
			return false;
		}
		__atomic_sub_fetch(&exclusive, 1, __ATOMIC_SEQ_CST);
		if (priority_semaphore != 0 || update_semaphore(&priority_semaphore))
		{
			__atomic_add_fetch(&priority_waiting, 1, __ATOMIC_SEQ_CST);
			if (users > 0)
			{
				// retail leaves the counts as they are when the wait fails
				if (WaitForSingleObject(priority_semaphore, (DWORD)sleep) != 0)
				{
					return false;
				}
			}
			else
			{
				__atomic_sub_fetch(&priority_waiting, 1, __ATOMIC_SEQ_CST);
			}
		}
		__atomic_add_fetch(&exclusive, 1, __ATOMIC_SEQ_CST);
		__atomic_add_fetch(&users, 1, __ATOMIC_SEQ_CST);
	}
	return true;
}

// entered with the claim counted (users)
// @retail 0x330260
bool s_rockall_sharelock::wait_for_share_lock(long sleep)
{
	while (exclusive > 0 || users > max_users)
	{
		__atomic_sub_fetch(&users, 1, __ATOMIC_SEQ_CST);
		if (sleep == 0)
		{
			return false;
		}
		if (normal_semaphore != 0 || update_semaphore(&normal_semaphore))
		{
			__atomic_add_fetch(&normal_waiting, 1, __ATOMIC_SEQ_CST);
			if (users > 0)
			{
				if (WaitForSingleObject(normal_semaphore, (DWORD)sleep) != 0)
				{
					return false;
				}
			}
			else
			{
				__atomic_sub_fetch(&normal_waiting, 1, __ATOMIC_SEQ_CST);
			}
		}
		__atomic_add_fetch(&users, 1, __ATOMIC_SEQ_CST);
	}
	return true;
}

// ---------------------------------------------------------------------------
// the thread lock (THREAD_SAFE)

struct s_rockall_thread_lock
{
	s_rockall_thread_lock *construct(bool new_thread_safe);
	bool claim_global_lock();
	void engage_global_lock();
	bool release_global_lock(bool force);
	void destruct();

	// the thread claiming every lock, and the one that holds them
	long owner;
	long locked_by;
	// further claims by the owner
	long count;
	s_rockall_spinlock lock;
	bool thread_safe;
};

typedef char assert_s_rockall_thread_lock_size[sizeof(s_rockall_thread_lock) == 0x24 ? 1 : -1];

// a lock is taken unless this thread holds every lock (LockAll)
static inline bool rockall_must_lock(s_rockall_thread_lock *thread_lock)
{
	return thread_lock->thread_safe && GetCurrentThreadId() != (DWORD)thread_lock->locked_by;
}

// @retail 0x32ec10
s_rockall_thread_lock *s_rockall_thread_lock::construct(bool new_thread_safe)
{
	h2_retail_32ae90(&lock, 0x1000, 0x100);
	thread_safe = new_thread_safe;
	owner = -1;
	locked_by = -1;
	count = 0;
	return this;
}

// @retail 0x32ec50
bool s_rockall_thread_lock::claim_global_lock()
{
	if (!thread_safe)
	{
		return false;
	}
	long thread = (long)GetCurrentThreadId();
	if (thread == owner)
	{
		count++;
		return false;
	}
	rockall_claim_lock(&lock);
	owner = thread;
	return true;
}

// @retail 0x32ecb0
void s_rockall_thread_lock::engage_global_lock()
{
	if (thread_safe)
	{
		long thread = (long)GetCurrentThreadId();
		if (thread == owner)
		{
			locked_by = thread;
		}
		else
		{
			rockall_failure_32ae60("No claim before engage in EngageGlobalLock", true);
		}
	}
}

// @retail 0x32ece0
bool s_rockall_thread_lock::release_global_lock(bool force)
{
	if (!thread_safe)
	{
		return false;
	}
	long thread = (long)GetCurrentThreadId();
	if (thread != owner && (!force || owner == -1))
	{
		rockall_failure_32ae60("No claim before release in ReleaseGlobalLock", true);
		return false;
	}
	if (count > 0)
	{
		count--;
		return false;
	}
	owner = -1;
	locked_by = -1;
	rockall_release_lock(&lock);
	return true;
}

// @retail 0x32ed60
void s_rockall_thread_lock::destruct()
{
	if (owner != -1 || locked_by != -1 || count != 0)
	{
		rockall_failure_32ae60("Global lock busy in destructor for THREAD_SAFE", true);
	}
	lock.destruct();
}

// ---------------------------------------------------------------------------
// the lists: a list's next is its first node and its previous its last; a
// node's next is at 4

struct s_rockall_list
{
	s_rockall_list *construct();
	void destruct();
	void destruct_32efb0();
	void remove(s_rockall_list *list);
	void insert(s_rockall_list *list);
	void insert_before(s_rockall_list *list, s_rockall_list *node);
	void insert_after(s_rockall_list *list, s_rockall_list *node);

	s_rockall_list *previous;
	s_rockall_list *next;
};

// @retail 0x32ee70
s_rockall_list *s_rockall_list::construct()
{
	next = 0;
	previous = 0;
	return this;
}

// @retail 0x32ee80
void s_rockall_list::remove(s_rockall_list *list)
{
	if (next != 0)
	{
		next->previous = previous;
	}
	else
	{
		list->previous = previous;
	}
	if (previous != 0)
	{
		previous->next = next;
	}
	else
	{
		list->next = next;
	}
	next = 0;
	previous = 0;
}

// at the front
// @retail 0x32eed0
void s_rockall_list::insert(s_rockall_list *list)
{
	s_rockall_list *first = list->next;
	next = first;
	if (first == 0)
	{
		list->previous = this;
		list->next = this;
		previous = 0;
	}
	else
	{
		list->next->previous = this;
		list->next = this;
		previous = 0;
	}
}

// @retail 0x32ef00
void s_rockall_list::insert_before(s_rockall_list *list, s_rockall_list *node)
{
	if (list->next == 0)
	{
		list->next = node;
		list->previous = node;
		node->next = 0;
		node->previous = 0;
		return;
	}
	if (previous != 0)
	{
		previous->next = node;
	}
	else
	{
		list->next = node;
	}
	node->previous = previous;
	node->next = this;
	previous = node;
}

// @retail 0x32ef50
void s_rockall_list::insert_after(s_rockall_list *list, s_rockall_list *node)
{
	if (list->next == 0)
	{
		list->next = node;
		list->previous = node;
		node->next = 0;
		node->previous = 0;
		return;
	}
	if (next != 0)
	{
		next->previous = node;
	}
	else
	{
		list->previous = node;
	}
	node->next = next;
	node->previous = this;
	next = node;
}

// @retail 0x32efa0
void s_rockall_list::destruct()
{
	next = 0;
	previous = 0;
}

// the same for another class (only exception unwinding reaches it)
// @retail 0x32efb0
void s_rockall_list::destruct_32efb0()
{
	destruct();
}

// two lists (only exception unwinding reaches its destructor)
struct s_rockall_list_pair
{
	void destruct();

	s_rockall_list first;
	s_rockall_list second;
};

// @retail 0x32efc0
void s_rockall_list_pair::destruct()
{
	second.destruct();
	first.destruct();
}

// a page's links: its bucket's list of pages with free space, the new
// pages' lists of its size, and the private find's hash chain
struct s_rockall_page_links
{
	s_rockall_page_links *construct();
	void destruct();

	s_rockall_list bucket;
	s_rockall_list pages;
	s_rockall_list find;
};

// @retail 0x32f010
s_rockall_page_links *s_rockall_page_links::construct()
{
	bucket.construct();
	pages.construct();
	find.construct();
	return this;
}

// @retail 0x32f070
void s_rockall_page_links::destruct()
{
	find.destruct();
	pages.destruct();
	bucket.destruct();
}

// ---------------------------------------------------------------------------
// the back end

class c_rockall_back_end
{
public:
	virtual void delete_area(void *memory, long size, bool user) = 0;
	virtual long natural_size() = 0;
	virtual void *new_area(long align_mask, long size, bool user) = 0;
};

// the page size, set on first use
#define g_rockall_natural_size (*(long *)0x480ef0)

struct s_rockall_virtual_memory
{
	void delete_area(void *memory, long size, bool user);
	long natural_size();
	void *new_area(long align_mask, long size, bool user);

	void const *vtable;
};

// @retail 0x329d30
void s_rockall_virtual_memory::delete_area(void *memory, long size, bool user)
{
	if (!VirtualFree(memory, 0, MEM_RELEASE))
	{
		rockall_failure_32ae60("Delete fails in DeleteArea", true);
	}
}

// @retail 0x329d60
long s_rockall_virtual_memory::natural_size()
{
	if (g_rockall_natural_size <= 0)
	{
		g_rockall_natural_size = 0x1000;
	}
	return g_rockall_natural_size;
}

// @retail 0x329d80
void *s_rockall_virtual_memory::new_area(long align_mask, long size, bool user)
{
	if (align_mask > ((c_rockall_back_end *)this)->natural_size())
	{
		// reserve enough to align inside, give it back and take the aligned
		// part, until no other thread takes the range in between
		long total = align_mask + size;
		void *reserved = VirtualAlloc(0, total, MEM_RESERVE, PAGE_READWRITE);
		while (reserved != 0)
		{
			if (!VirtualFree(reserved, 0, MEM_RELEASE))
			{
				break;
			}
			void *aligned = VirtualAlloc((void *)(((unsigned long)reserved + align_mask) & ~(unsigned long)align_mask),
				size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
			if (aligned != 0)
			{
				return aligned;
			}
			reserved = VirtualAlloc(0, total, MEM_RESERVE, PAGE_READWRITE);
		}
		return 0;
	}
	return VirtualAlloc(0, size, MEM_COMMIT, PAGE_READWRITE);
}

// ---------------------------------------------------------------------------
// the declarations

// where an address lies in its page (SEARCH_PAGE)
struct s_rockall_search
{
	void *address;
	c_rockall_cache *cache;
	// the address is the start of an element
	bool found;
	c_rockall_page *page;
	// the element's two bits: allocated, and handed on as a smaller page
	unsigned long mask;
	unsigned long sub_mask;
	unsigned long *vector;
	long index;
	long word;
	long shift;
};

typedef char assert_s_rockall_search_size[sizeof(s_rockall_search) == 0x28 ? 1 : -1];

// a page description (PAGE): the bit vector follows, two bits per element
struct c_rockall_page
{
	c_rockall_page *construct(void *new_address, c_rockall_cache *new_cache, long new_size,
		c_rockall_cache *new_parent, long new_version);
	long actual_size();
	bool delete_memory(s_rockall_search *search);
	void delete_all();
	c_rockall_page *find_page(void *find_address, s_rockall_search *search, c_rockall_find *find, bool recursive);
	bool multiple_new(long *actual, void **array, long requested);
	void *new_memory(bool sub_page);
	static bool walk(s_rockall_search *search, c_rockall_find *find);
	void destruct();

	s_rockall_page_links links;
	// the shared find's hash chain
	s_rockall_list shared_find;
	void *address;
	// the memory's size, -1 when it came from the parent cache
	long size;
	long version;
	short allocated;
	short available;
	// the first word of the bit vector with a free element
	short first_word;
	short unknown32;
	c_rockall_cache *cache;
	c_rockall_cache *parent;
	unsigned long vector[1];
};

static inline c_rockall_page *rockall_page_of(s_rockall_list *node)
{
	return node ? (c_rockall_page *)((byte *)node - 8) : 0;
}

static inline c_rockall_page *rockall_page_of_find(s_rockall_list *node)
{
	return node ? (c_rockall_page *)((byte *)node - 0x10) : 0;
}

static inline c_rockall_page *rockall_page_of_shared_find(s_rockall_list *node)
{
	return node ? (c_rockall_page *)((byte *)node - 0x18) : 0;
}

// a page the find's lookaside remembers for an address
struct s_rockall_lookaside
{
	unsigned long address;
	c_rockall_page *page;
};

// the find (FIND)
struct c_rockall_find
{
	c_rockall_find *construct(long hash_size, long new_max_lookaside, long new_threshold, bool new_shared_links,
		bool new_resize, c_rockall_back_end *new_back_end, s_rockall_thread_lock *new_thread_lock);
	void delete_page(c_rockall_page *page);
	bool known_area(void *address, c_rockall_cache *cache);
	void update_find(void *address, c_rockall_page *page, long version);
	void update_lookaside(long new_top_mask, unsigned long new_min_mask);
	bool walk(bool *active, void **address, c_rockall_cache *cache, long *space);
	void destruct();
	bool delete_memory(void *address, c_rockall_cache *cache);
	bool details(void *address, s_rockall_search *search, c_rockall_cache *cache, long *space);
	c_rockall_page *find_page(void *address, c_rockall_cache *parent);
	void resize_hash_table();
	void insert_page(c_rockall_page *page);
	bool lookaside_hit(unsigned long key, c_rockall_page **page);

	long unknown00;
	long max_hash;
	unsigned long hash_mask;
	long hash_shift;
	// the pages link into the shared find's chain (the global find)
	bool shared_links;
	bool resize;
	short unknown12;
	long max_lookaside;
	long top_mask;
	unsigned long min_mask;
	// lookups so far, until the lookaside is used
	long used;
	unsigned long lookaside_mask;
	long lookaside_shift;
	long lookaside_threshold;
	s_rockall_list *hash;
	s_rockall_lookaside *lookaside;
	c_rockall_back_end *back_end;
	s_rockall_thread_lock *thread_lock;
	long unknown40;
	s_rockall_sharelock sharelock;
	s_rockall_spinlock lock;
	long pages;
};

typedef char assert_c_rockall_find_size[sizeof(c_rockall_find) == 0x7c ? 1 : -1];

// the bucket's address and element arithmetic
typedef void *(*t_rockall_compute_address)(c_rockall_bucket *self, void *page_address, long index);
typedef long (*t_rockall_compute_offset)(c_rockall_bucket *self, long offset, bool *exact);

// the links every bucket keeps (CONNECTIONS)
struct c_rockall_connections
{
	c_rockall_connections *construct_connections();
	void destruct_connections();
	void update_connections(c_rockall_heap *new_heap, c_rockall_new_pages *new_new_pages, c_rockall_cache *new_parent,
		c_rockall_find *new_find, c_rockall_find *new_public_find);
	void delete_from_find_list(c_rockall_page *page);
	void insert_in_find_list(c_rockall_page *page);

	byte unknown00;
	bool active;
	short unknown02;
	c_rockall_heap *heap;
	c_rockall_new_pages *new_pages;
	c_rockall_cache *parent;
	c_rockall_find *find;
	c_rockall_find *public_find;
};

typedef char assert_c_rockall_connections_size[sizeof(c_rockall_connections) == 0x18 ? 1 : -1];

// the pages of one allocation size (BUCKET)
struct c_rockall_bucket : c_rockall_connections
{
	c_rockall_bucket *construct_bucket(long new_allocation_size, long new_chunk_size, long new_page_size);
	void destruct_bucket();
	bool delete_bucket_memory(void *address, c_rockall_page *page, long page_version);
	void delete_from_bucket_list(c_rockall_page *page);
	void insert_in_bucket_list(c_rockall_page *page);
	bool multiple_delete_bucket(struct s_rockall_cache_entry *entries, long *deleted, long count);
	bool multiple_new_bucket(long *actual, void **array, long requested);
	void *new_bucket_memory(bool sub_page, long new_size);
	void release_space(long max_free_pages);
	void update_bucket(c_rockall_heap *new_heap, c_rockall_new_pages *new_new_pages, c_rockall_cache *new_parent,
		c_rockall_find *new_find, c_rockall_find *new_public_find);

	void *address_of(void *page_address, long index)
	{
		return compute_address((c_rockall_bucket *)((byte *)this + compute_address_adjust), page_address, index);
	}
	long index_of(long offset, bool *exact)
	{
		return compute_offset((c_rockall_bucket *)((byte *)this + compute_offset_adjust), offset, exact);
	}

	long unknown18;
	long allocation_size;
	long chunk_size;
	long page_size;
	// the pages with free space, kept in address order
	long active_pages;
	s_rockall_list bucket_list;
	// the end of the first of them, -1 without one
	unsigned long current_end;
	long allocation_shift;
	long chunk_shift;
	t_rockall_compute_address compute_address;
	long compute_address_adjust;
	t_rockall_compute_offset compute_offset;
	long compute_offset_adjust;
	short elements;
	short chunk_elements;
	short vector_size;
	short size_key;
};

typedef char assert_c_rockall_bucket_size[sizeof(c_rockall_bucket) == 0x58 ? 1 : -1];

// a cached allocation: the address, its page and the page's version
struct s_rockall_cache_entry
{
	void *address;
	c_rockall_page *page;
	long version;
};

// a cache (CACHE): freed allocations are stacked and handed out again,
// ready ones are filled in from the bucket in growing batches
struct c_rockall_cache : c_rockall_bucket
{
	c_rockall_cache *construct(long new_allocation_size, long new_cache_size, long new_chunk_size,
		long new_page_size, bool new_recycle, s_rockall_thread_lock *new_thread_lock);
	void create_cache_stack();
	void *create_data_page();
	bool delete_memory(void *address, c_rockall_page *page, long page_version);
	void delete_all();
	c_rockall_page *find_child_page(void *address, c_rockall_find *the_find);
	c_rockall_page *find_parent_page(void *address, c_rockall_find *the_find);
	bool multiple_new(long *actual, void **array, long requested);
	void *new_memory();
	void *new_external(bool sub_page, long new_size);
	void release_cache_space(long max_free_pages);
	bool search_cache(void *address);
	bool truncate();
	void update_cache(c_rockall_heap *new_heap, c_rockall_new_pages *new_new_pages, c_rockall_cache *new_parent,
		c_rockall_find *new_find, c_rockall_find *new_public_find);
	bool delete_data_page(void *address);
	void destruct();

	bool cache_active;
	bool recycle;
	short cache_size;
	// the next batch to fill in
	short fill_size;
	// the caches whose pages come from this one
	short children;
	s_rockall_cache_entry *stack;
	void **fill;
	s_rockall_thread_lock *thread_lock;
	long stack_top;
	long fill_top;
	s_rockall_spinlock lock;
};

typedef char assert_c_rockall_cache_size[sizeof(c_rockall_cache) == 0x88 ? 1 : -1];

// the new pages (NEW_PAGES)
struct s_rockall_page_size
{
	// the size class's element count (from the table, times 16)
	long elements;
	// pages with their memory from the back end, unused descriptions, pages
	// with their memory from a parent cache
	s_rockall_list external;
	s_rockall_list free;
	s_rockall_list internal;
	// the bit vector's size (the table's entry times 4)
	long vector_size;
};

typedef char assert_s_rockall_page_size_size[sizeof(s_rockall_page_size) == 0x20 ? 1 : -1];

struct c_rockall_new_pages
{
	c_rockall_new_pages *construct(long *page_sizes, c_rockall_back_end *new_back_end, long new_size_count,
		s_rockall_thread_lock *new_thread_lock);
	void delete_page(c_rockall_page *page);
	short find_size_index(short size);
	void resize_stack();
	c_rockall_page *create_page(c_rockall_cache *cache, long size);
	void delete_all(bool recycle);
	void *special_new(long size);
	bool walk(s_rockall_search *search, c_rockall_find *find);
	void destruct();
	void *verify_new_area(long align_mask, long size, bool user);

	long unknown00;
	// the used part of the current special block
	long special_offset;
	long size_count;
	long stack_capacity;
	long natural_size;
	// the size of the block holding the size table (and the first stack)
	long table_size;
	// the size of the stack's own block, 0 while it shares the table's
	long stack_size;
	long stack_top;
	long version;
	byte *special_block;
	s_rockall_page_size *sizes;
	// every block taken from the back end
	void **stack;
	c_rockall_back_end *back_end;
	c_rockall_cache *top_cache;
	s_rockall_thread_lock *thread_lock;
	s_rockall_spinlock lock;
};

typedef char assert_c_rockall_new_pages_size[sizeof(c_rockall_new_pages) == 0x50 ? 1 : -1];

// the heap (HEAP)
struct c_rockall_heap
{
	bool details(void *address, s_rockall_search *search, long *space);
	c_rockall_cache *find_cache(long size);
	bool known_area(void *address);
	void lock_all();
	bool multiple_delete(long actual, void **array, long size);
	void *new_memory(long size, long *space, bool zero);
	bool truncate(long max_free_space);
	void unlock_all(bool partial);
	bool verify(void *address, long *space);
	bool walk(bool *walking, void **address, long *space);
	void destruct();
	c_rockall_heap *construct(c_rockall_cache **caches1, c_rockall_cache **caches2, long max_free_space,
		c_rockall_new_pages *pages, c_rockall_find *the_find, c_rockall_find *the_public_find,
		c_rockall_back_end *the_back_end, long size1, long size2, long stride1, long stride2,
		s_rockall_thread_lock *the_thread_lock);
	bool delete_memory(void *address, long size);
	void delete_all(bool recycle);
	bool multiple_new(long *actual, void **array, long requested, long size, long *space, bool zero);
	void *resize(void *address, long new_size, long move, long *space, bool no_delete, bool zero);

	byte unknown00;
	bool active;
	short unknown02;
	// the free pages the top cache keeps
	long max_free_pages;
	// the smallest page's mask
	unsigned long min_mask;
	long cache_count;
	long min_size;
	// the sizes below this come from the first table
	long threshold;
	long max_size;
	long table1_size;
	long table2_size;
	long shift1;
	long shift2;
	c_rockall_cache **table1;
	c_rockall_cache **table2;
	c_rockall_cache **caches;
	// the cache for allocations above the largest size
	c_rockall_cache *external_cache;
	c_rockall_new_pages *new_pages;
	long unknown40;
	c_rockall_find *find;
	c_rockall_find *public_find;
	c_rockall_back_end *back_end;
	c_rockall_cache *top_cache;
	s_rockall_thread_lock *thread_lock;
};

typedef char assert_c_rockall_heap_size[sizeof(c_rockall_heap) == 0x58 ? 1 : -1];

// the find's lock, unless this thread holds every lock
static inline void rockall_find_claim_share(c_rockall_find *find)
{
	if (rockall_must_lock(find->thread_lock))
	{
		rockall_claim_share(&find->sharelock);
	}
}

static inline void rockall_find_release_share(c_rockall_find *find)
{
	if (rockall_must_lock(find->thread_lock))
	{
		rockall_release_share(&find->sharelock);
	}
}

static inline void rockall_find_claim_exclusive(c_rockall_find *find)
{
	if (rockall_must_lock(find->thread_lock))
	{
		rockall_claim_exclusive(&find->sharelock);
	}
}

static inline void rockall_find_release_exclusive(c_rockall_find *find)
{
	if (rockall_must_lock(find->thread_lock))
	{
		rockall_release_exclusive(&find->sharelock);
	}
}

static inline void rockall_cache_claim(c_rockall_cache *cache)
{
	if (rockall_must_lock(cache->thread_lock))
	{
		rockall_claim_lock(&cache->lock);
	}
}

static inline void rockall_cache_release(c_rockall_cache *cache)
{
	if (rockall_must_lock(cache->thread_lock))
	{
		rockall_release_lock(&cache->lock);
	}
}

static inline void rockall_pages_claim(c_rockall_new_pages *pages)
{
	if (rockall_must_lock(pages->thread_lock))
	{
		rockall_claim_lock(&pages->lock);
	}
}

static inline void rockall_pages_release(c_rockall_new_pages *pages)
{
	if (rockall_must_lock(pages->thread_lock))
	{
		rockall_release_lock(&pages->lock);
	}
}

// a cache's batch size doubles up to the cache's size
static inline void rockall_grow_fill_size(c_rockall_cache *cache)
{
	short limit = cache->cache_size;
	short size = cache->fill_size;
	if (size < limit)
	{
		size = (short)(size + size);
		cache->fill_size = size;
		if (size > limit)
		{
			cache->fill_size = limit;
		}
	}
}

// ---------------------------------------------------------------------------
// the new pages

// VerifyNewArea, inlined everywhere in retail
inline void *c_rockall_new_pages::verify_new_area(long align_mask, long size, bool user)
{
	void *memory = back_end->new_area(align_mask, size, user);
	if (memory != 0)
	{
		if (((unsigned long)memory & (unsigned long)align_mask) == 0)
		{
			return memory;
		}
		back_end->delete_area(memory, size, user);
		rockall_failure_32ae60("Alignment of allocation in VerifyNewArea", true);
	}
	return 0;
}

// @retail 0x32b0e0
c_rockall_new_pages *c_rockall_new_pages::construct(long *page_sizes, c_rockall_back_end *new_back_end,
	long new_size_count, s_rockall_thread_lock *new_thread_lock)
{
	h2_retail_32ee20(this);
	h2_retail_32ae90(&lock, 0x1000, 0x100);
	long page = new_back_end->natural_size();
	long capacity = (long)((unsigned long)(page - (new_size_count << 5)) >> 2);
	if (rockall_power_of_two_32ee00(page) && page >= g_rockall_minimum_page && new_size_count >= 1 &&
		(page_sizes[new_size_count - 1] << 4) <= 0xffff)
	{
		byte *block = (byte *)new_back_end->new_area(page - 1, page, false);
		if (block != 0)
		{
			long previous = 0;
			special_offset = 0;
			stack_capacity = capacity;
			natural_size = page;
			table_size = page;
			back_end = new_back_end;
			size_count = new_size_count;
			stack_size = 0;
			thread_lock = new_thread_lock;
			stack_top = 0;
			version = 0;
			special_block = 0;
			sizes = (s_rockall_page_size *)block;
			stack = (void **)(block + (new_size_count << 5));
			top_cache = 0;
			for (long index = 0; index < new_size_count; index++)
			{
				long words = page_sizes[index];
				if (words > previous)
				{
					s_rockall_page_size *entry = &sizes[index];
					entry->elements = words << 4;
					entry->external.construct();
					entry->internal.construct();
					entry->free.construct();
					entry->vector_size = words * 4;
					previous = words;
				}
				else
				{
					rockall_failure_32ae60("Sizes in constructor for NEW_PAGES", true);
				}
			}
		}
		else
		{
			rockall_failure_32ae60("No memory in constructor for NEW_PAGES", true);
		}
	}
	else
	{
		rockall_failure_32ae60("Setup of pages in constructor for NEW_PAGES", true);
	}
	return this;
}

// @retail 0x32b2c0
void c_rockall_new_pages::delete_page(c_rockall_page *page)
{
	c_rockall_cache *cache = page->cache;
	s_rockall_page_size *entry = &sizes[cache->size_key];
	void *address = page->address;
	long size = page->size;
	rockall_pages_claim(this);
	cache->delete_from_bucket_list(page);
	cache->delete_from_find_list(page);
	page->links.pages.remove(cache->parent == k_rockall_no_parent ? &entry->external : &entry->internal);
	page->destruct();
	page->links.pages.insert(&entry->free);
	rockall_pages_release(this);
	if (size == -1)
	{
		if (!cache->parent->delete_data_page(address))
		{
			rockall_failure_32ae60("Deleting data page in DeletePage", true);
		}
	}
	else
	{
		back_end->delete_area(address, size, true);
	}
}

// @retail 0x32b400
short c_rockall_new_pages::find_size_index(short size)
{
	for (long index = 0; index < size_count; index++)
	{
		if (size <= sizes[index].elements)
		{
			return (short)index;
		}
	}
	return -1;
}

// @retail 0x32b430
void c_rockall_new_pages::resize_stack()
{
	long new_size = (stack_size > 0 ? stack_size : natural_size) * 2;
	if (stack_top >= stack_capacity)
	{
		void **new_stack = (void **)back_end->new_area(natural_size - 1, new_size, false);
		if (new_stack != 0)
		{
			bool owned = stack_size > 0;
			long old_size = stack_capacity * 4;
			void **old_stack = stack;
			stack_capacity = (long)((unsigned long)new_size >> 2);
			stack_size = new_size;
			stack = new_stack;
			memcpy(new_stack, old_stack, old_size);
			if (owned)
			{
				back_end->delete_area(old_stack, old_size, false);
			}
		}
	}
}

// @retail 0x32b4c0
c_rockall_page *c_rockall_new_pages::create_page(c_rockall_cache *cache, long size)
{
	c_rockall_page *page = 0;
	short index = cache->size_key;
	rockall_pages_claim(this);
	s_rockall_page_size *entry = &sizes[index];
	if (entry->free.next == 0)
	{
		// a new page of descriptions
		long description_size = ((entry->vector_size - 1) * 4 + 0x40 + 0x1f) & ~0x1f;
		long count = natural_size / description_size;
		if (stack_top >= stack_capacity)
		{
			resize_stack();
			entry = &sizes[index];
		}
		if (count > 0 && stack_top < stack_capacity)
		{
			byte *block = (byte *)verify_new_area(natural_size - 1, natural_size, false);
			if (block != 0)
			{
				stack[stack_top] = block;
				stack_top++;
				for (; count != 0; count--)
				{
					c_rockall_page *description = (c_rockall_page *)block;
					description->construct(0, cache, 0, 0, 0);
					description->destruct();
					description->links.pages.insert(&entry->free);
					block += description_size;
				}
			}
		}
	}
	if (entry->free.next != 0)
	{
		page = rockall_page_of(entry->free.next);
		c_rockall_cache *parent = cache->parent;
		page->links.pages.remove(&entry->free);
		rockall_pages_release(this);
		void *address;
		if (cache->parent != k_rockall_no_parent && size == -1)
		{
			address = cache->parent->create_data_page();
		}
		else
		{
			long align_mask = top_cache->page_size - 1;
			parent = k_rockall_no_parent;
			if (size == -1)
			{
				size = cache->page_size;
			}
			address = verify_new_area(align_mask, size, true);
		}
		rockall_pages_claim(this);
		if (address != 0)
		{
			version += 2;
			page->construct(address, cache, size, parent, version);
			cache->insert_in_bucket_list(page);
			cache->insert_in_find_list(page);
			page->links.pages.insert(cache->parent == k_rockall_no_parent ? &entry->external : &entry->internal);
		}
		else
		{
			page->links.pages.insert(&entry->free);
			page = 0;
		}
	}
	rockall_pages_release(this);
	return page;
}

// @retail 0x32b850
void c_rockall_new_pages::delete_all(bool recycle)
{
	rockall_pages_claim(this);
	for (long index = 0; index < size_count; index++)
	{
		s_rockall_page_size *entry = &sizes[index];
		c_rockall_page *page = rockall_page_of(entry->internal.next);
		while (page != 0)
		{
			c_rockall_page *next;
			void *address = page->address;
			c_rockall_cache *cache = page->cache;
			long size = page->size;
			if (page->allocated > 0)
			{
				if (size == -1)
				{
					if (recycle)
					{
						// kept, only reset
						page->delete_all();
						page = rockall_page_of(page->links.pages.next);
						continue;
					}
				}
				else
				{
					back_end->delete_area(address, size, true);
				}
			}
			else if (size == -1)
			{
				if (recycle && !cache->parent->delete_data_page(address))
				{
					rockall_failure_32ae60("Reset data page in DeleteAll", true);
				}
			}
			else
			{
				back_end->delete_area(address, size, true);
			}
			next = rockall_page_of(page->links.pages.next);
			if (page->available > 0)
			{
				cache->delete_from_bucket_list(page);
			}
			cache->delete_from_find_list(page);
			page->links.pages.remove(&entry->internal);
			page->destruct();
			page->links.pages.insert(&entry->free);
			page = next;
		}
		if (!recycle)
		{
			for (page = rockall_page_of(entry->external.next); page != 0; page = rockall_page_of(entry->external.next))
			{
				c_rockall_cache *cache = page->cache;
				back_end->delete_area(page->address, page->size, true);
				if (page->available > 0)
				{
					cache->delete_from_bucket_list(page);
				}
				cache->delete_from_find_list(page);
				page->links.pages.remove(&entry->external);
				page->destruct();
				page->links.pages.insert(&entry->free);
			}
		}
	}
	rockall_pages_release(this);
}

// @retail 0x32ba90
void *c_rockall_new_pages::special_new(long size)
{
	rockall_pages_claim(this);
	if (special_block == 0 || special_offset + size > natural_size)
	{
		if (stack_top >= stack_capacity)
		{
			resize_stack();
		}
		long page = natural_size;
		// retail returns from these failures with the lock still held
		if (size >= page || stack_top >= stack_capacity)
		{
			return 0;
		}
		void *block = verify_new_area(page - 1, page, false);
		if (block == 0)
		{
			return 0;
		}
		stack[stack_top] = block;
		stack_top++;
		special_block = (byte *)block;
		special_offset = 0;
	}
	byte *result = special_block + special_offset;
	special_offset = (special_offset + size + 0x1f) & ~0x1f;
	rockall_pages_release(this);
	return result;
}

// @retail 0x32bbb0
bool c_rockall_new_pages::walk(s_rockall_search *search, c_rockall_find *find)
{
	rockall_pages_claim(this);
	if (search->address == 0)
	{
		// the first page of the first size with one
		for (long index = 0; index < size_count; index++)
		{
			search->page = rockall_page_of(sizes[index].internal.next);
			if (search->page != 0)
			{
				search->address = search->page->address;
				break;
			}
		}
	}
	else
	{
		c_rockall_page *current = search->page;
		search->page = rockall_page_of(current->links.pages.next);
		if (search->page != 0)
		{
			search->address = search->page->address;
		}
		else
		{
			// the first page of the next size with one after the size the
			// current page ended
			search->address = 0;
			bool found = false;
			for (long index = 0; index < size_count; index++)
			{
				if (!found)
				{
					if (current == rockall_page_of(sizes[index].internal.previous))
					{
						found = true;
					}
				}
				else
				{
					search->page = rockall_page_of(sizes[index].internal.next);
					if (search->page != 0)
					{
						search->address = search->page->address;
						break;
					}
				}
			}
		}
	}
	if (search->address != 0)
	{
		search->page->find_page(search->address, search, find, false);
	}
	rockall_pages_release(this);
	return search->address != 0;
}

// @retail 0x32bd20
void c_rockall_new_pages::destruct()
{
	delete_all(false);
	special_offset = 0;
	special_block = 0;
	for (long index = 0; index < stack_top; index++)
	{
		back_end->delete_area(stack[index], natural_size, false);
	}
	stack_top = 0;
	if (stack_size > 0)
	{
		back_end->delete_area(stack, stack_size, false);
	}
	for (long index = 0; index < size_count; index++)
	{
		s_rockall_page_size *entry = &sizes[index];
		entry->external.destruct();
		entry->internal.destruct();
		entry->free.destruct();
	}
	back_end->delete_area(sizes, table_size, false);
	lock.destruct();
	rockall_environment_destruct_32ee60(this);
}

// ---------------------------------------------------------------------------
// the heap

// @retail 0x32be10
bool c_rockall_heap::details(void *address, s_rockall_search *search, long *space)
{
	if (!active)
	{
		return false;
	}
	long unused;
	if (space == 0)
	{
		space = &unused;
	}
	if (find->details(address, search, top_cache, space))
	{
		return true;
	}
	if (public_find != 0 && public_find->details(address, search, top_cache, space))
	{
		return true;
	}
	return false;
}

// @retail 0x32be80
c_rockall_cache *c_rockall_heap::find_cache(long size)
{
	if (size < threshold)
	{
		return table1[(size - 1) >> (byte)shift1];
	}
	return table2[(size - 1) >> (byte)shift2];
}

// @retail 0x32beb0
bool c_rockall_heap::known_area(void *address)
{
	if (!active)
	{
		return false;
	}
	if (find->known_area(address, top_cache))
	{
		return true;
	}
	if (public_find != 0)
	{
		return public_find->known_area(address, top_cache);
	}
	return false;
}

// @retail 0x32bf00
void c_rockall_heap::lock_all()
{
	if (!active || !thread_lock->claim_global_lock())
	{
		return;
	}
	for (long index = 0; index < cache_count; index++)
	{
		rockall_cache_claim(caches[index]);
	}
	rockall_find_claim_exclusive(find);
	rockall_pages_claim(new_pages);
	thread_lock->engage_global_lock();
}

// @retail 0x32c020
bool c_rockall_heap::multiple_delete(long actual, void **array, long size)
{
	if (!active)
	{
		return false;
	}
	bool result = true;
	// retail does not check the shared find here
	if (size > 0 && size <= max_size)
	{
		c_rockall_cache *parent = find_cache(size)->parent;
		for (long index = 0; index < actual; index++)
		{
			void *address = array[index];
			if (!find->delete_memory(address, parent) && !find->delete_memory(address, top_cache) &&
				!public_find->delete_memory(address, top_cache))
			{
				result = false;
			}
		}
		return result;
	}
	for (long index = 0; index < actual; index++)
	{
		void *address = array[index];
		if (!find->delete_memory(address, top_cache) && !public_find->delete_memory(address, top_cache))
		{
			result = false;
		}
	}
	return result;
}

// @retail 0x32c110
void *c_rockall_heap::new_memory(long size, long *space, bool zero)
{
	if (!active || size <= 0)
	{
		return 0;
	}
	void *memory;
	long actual;
	if (size <= max_size)
	{
		c_rockall_cache *cache = find_cache(size);
		memory = cache->new_memory();
		actual = cache->allocation_size;
	}
	else
	{
		memory = external_cache->new_external(false, size);
		actual = size;
	}
	if (memory != 0)
	{
		if (space != 0)
		{
			*space = actual;
		}
		if (zero)
		{
			memset(memory, 0, actual);
		}
	}
	return memory;
}

// @retail 0x32c180
bool c_rockall_heap::truncate(long max_free_space)
{
	bool result = true;
	if (active)
	{
		for (long index = 0; index < cache_count; index++)
		{
			if (!caches[index]->truncate())
			{
				result = false;
			}
		}
		top_cache->release_cache_space(max_free_space / top_cache->allocation_size);
	}
	return result;
}

// @retail 0x32c1e0
void c_rockall_heap::unlock_all(bool partial)
{
	if (!active || !thread_lock->release_global_lock(true))
	{
		return;
	}
	rockall_pages_release(new_pages);
	rockall_find_release_exclusive(find);
	if (!partial)
	{
		for (long index = 0; index < cache_count; index++)
		{
			rockall_cache_release(caches[index]);
		}
	}
}

// @retail 0x32c300
bool c_rockall_heap::verify(void *address, long *space)
{
	if (!active)
	{
		return false;
	}
	s_rockall_search search;
	long size;
	if (!details(address, &search, &size) || !(*search.vector & search.mask) || !search.found)
	{
		return false;
	}
	// still waiting in its cache: not allocated
	if (size > 0 && size <= max_size && search.cache->search_cache(address))
	{
		return false;
	}
	if (space != 0)
	{
		*space = size;
	}
	return true;
}

// @retail 0x32c380
bool c_rockall_heap::walk(bool *walking, void **address, long *space)
{
	if (walking == 0 || !find->walk(walking, address, top_cache, space))
	{
		return false;
	}
	long size = *space;
	if (size > 0 && size <= max_size && find_cache(size)->search_cache(*address))
	{
		*walking = false;
	}
	return true;
}

// @retail 0x32c3e0
void c_rockall_heap::destruct()
{
	active = false;
	// no longer active, so these two return at once (as in retail)
	lock_all();
	for (long index = 0; index < cache_count; index++)
	{
		caches[index]->delete_all();
	}
	new_pages->delete_all(false);
	unlock_all(true);
	if (table1 != 0)
	{
		back_end->delete_area(table1, table1_size + table2_size, false);
	}
	rockall_environment_destruct_32ee60(this);
}

// the checks of a cache against the next one
static inline void rockall_check_cache(c_rockall_cache *cache, c_rockall_cache *next, long stride, long top_size)
{
	long size = cache->allocation_size;
	long page = cache->page_size;
	long chunk = cache->chunk_size;
	if (size % stride != 0)
	{
		rockall_failure_32ae60("Cache size not multiple of stride", true);
	}
	if (size >= next->allocation_size)
	{
		rockall_failure_32ae60("Cache sizes not in ascending order", true);
	}
	if (size > chunk || chunk > page)
	{
		rockall_failure_32ae60("Chunk size not suitable for cache", true);
	}
	if (size >= page)
	{
		rockall_failure_32ae60("Cache size larger than parent size", true);
	}
	if (page > top_size)
	{
		rockall_failure_32ae60("Parent size exceeds 'TopCache' size", true);
	}
}

// @retail 0x32c490
c_rockall_heap *c_rockall_heap::construct(c_rockall_cache **caches1, c_rockall_cache **caches2, long max_free_space,
	c_rockall_new_pages *pages, c_rockall_find *the_find, c_rockall_find *the_public_find,
	c_rockall_back_end *the_back_end, long size1, long size2, long stride1, long stride2,
	s_rockall_thread_lock *the_thread_lock)
{
	h2_retail_32ee20(this);
	if (size1 < 1 || size2 < 3)
	{
		rockall_failure_32ae60("A heap size in constructor for HEAP", true);
		return this;
	}
	cache_count = size1 + size2;
	min_size = caches1[0]->allocation_size;
	threshold = caches2[0]->allocation_size;
	max_size = caches2[size2 - 3]->allocation_size;
	caches = caches1;
	external_cache = caches2[size2 - 1];
	public_find = the_public_find;
	find = the_find;
	new_pages = pages;
	back_end = the_back_end;
	top_cache = caches2[size2 - 2];
	long minimum = g_rockall_minimum_page;
	if (external_cache->page_size != top_cache->page_size || !rockall_power_of_two_32ee00(back_end->natural_size()) ||
		back_end->natural_size() < minimum || top_cache->page_size < g_rockall_minimum_page ||
		!rockall_power_of_two_32ee00(top_cache->page_size) || stride1 <= 0 || !rockall_power_of_two_32ee00(stride1) ||
		stride2 < stride1 || !rockall_power_of_two_32ee00(stride2) || !rockall_log2_32edd0(stride1, &shift1) ||
		!rockall_log2_32edd0(stride2, &shift2))
	{
		rockall_failure_32ae60("The allocation sizes in constructor for HEAP", true);
		return this;
	}
	long top_size = top_cache->page_size;
	long count1 = threshold / stride1;
	long count2 = top_size / stride2;
	max_free_pages = max_free_space / top_cache->allocation_size;
	min_mask = (unsigned long)(top_cache->allocation_size - 1);
	thread_lock = the_thread_lock;
	table1_size = count1 * 4;
	table2_size = count2 * 4;
	for (long index = 0; index < size1; index++)
	{
		rockall_check_cache(caches1[index], caches1[index + 1], stride1, top_size);
	}
	for (long index = 0; index < size2 - 2; index++)
	{
		rockall_check_cache(caches2[index], caches2[index + 1], stride2, top_size);
	}
	for (long index = size2 - 2; index < size2; index++)
	{
		c_rockall_cache *cache = caches2[index];
		long size = cache->allocation_size;
		if (size % stride2 != 0)
		{
			rockall_failure_32ae60("Top cache size not multiple of minimum", true);
		}
		if (size != cache->chunk_size)
		{
			rockall_failure_32ae60("Chunk size not suitable for top cache", true);
		}
		if (size != cache->page_size)
		{
			rockall_failure_32ae60("Page size not suitable for top cache", true);
		}
		if (cache->cache_size != 0)
		{
			rockall_failure_32ae60("Cache size not zero for top cache", true);
		}
	}
	table1 = (c_rockall_cache **)back_end->new_area(back_end->natural_size() - 1, table1_size + table2_size, false);
	if (table1 == 0)
	{
		rockall_failure_32ae60("Mapping table in constructor for HEAP", true);
		return this;
	}
	// each size to the smallest cache that holds it
	c_rockall_cache **source = caches1;
	long size = stride1;
	for (long index = 0; index < count1; index++)
	{
		if (size > (*source)->allocation_size)
		{
			source++;
		}
		table1[index] = *source;
		size += stride1;
	}
	table2 = (c_rockall_cache **)((byte *)table1 + table1_size);
	source = caches2;
	size = stride2;
	for (long index = 0; index < count2; index++)
	{
		if (size > (*source)->allocation_size)
		{
			source++;
		}
		table2[index] = *source;
		size += stride2;
	}
	// each cache's pages come from the cache of their size (the top cache's
	// from the back end)
	for (long index = 0; index < cache_count - 1; index++)
	{
		c_rockall_cache *cache = caches[index];
		long page = cache->page_size;
		c_rockall_cache *parent = page < threshold ? table1[(page - 1) >> (byte)shift1] : table2[(page - 1) >> (byte)shift2];
		bool top = cache == parent;
		if (rockall_power_of_two_32ee00(page) && page >= 0x20 && page == parent->allocation_size)
		{
			if ((unsigned long)page < min_mask)
			{
				min_mask = (unsigned long)(page - 1);
			}
			if (top)
			{
				parent = k_rockall_no_parent;
			}
			cache->update_cache(this, pages, parent, find, public_find);
		}
		else
		{
			rockall_failure_32ae60("Parent bucket is invalid", true);
		}
	}
	external_cache->update_cache(this, pages, top_cache, find, public_find);
	new_pages->top_cache = top_cache;
	find->update_lookaside(top_cache->allocation_size - 1, min_mask);
	if (public_find != 0)
	{
		public_find->update_lookaside(top_cache->allocation_size - 1, min_mask);
	}
	active = true;
	return this;
}

// @retail 0x32ca20
bool c_rockall_heap::delete_memory(void *address, long size)
{
	if (!active)
	{
		return false;
	}
	if (size > 0 && size <= max_size && find->delete_memory(address, find_cache(size)))
	{
		return true;
	}
	if (find->delete_memory(address, top_cache))
	{
		return true;
	}
	if (public_find == 0)
	{
		return false;
	}
	if (size > 0 && size <= max_size && public_find->delete_memory(address, find_cache(size)))
	{
		return true;
	}
	return public_find->delete_memory(address, top_cache);
}

// @retail 0x32cab0
void c_rockall_heap::delete_all(bool recycle)
{
	if (!active)
	{
		return;
	}
	lock_all();
	find->used = 0;
	for (long index = 0; index < cache_count; index++)
	{
		caches[index]->delete_all();
	}
	new_pages->delete_all(recycle);
	unlock_all(false);
	if (recycle)
	{
		top_cache->release_cache_space(max_free_pages);
	}
}

// @retail 0x32cb20
bool c_rockall_heap::multiple_new(long *actual, void **array, long requested, long size, long *space, bool zero)
{
	if (!active)
	{
		return false;
	}
	long unused;
	if (actual == 0)
	{
		actual = &unused;
	}
	*actual = 0;
	if (size <= 0)
	{
		return false;
	}
	if (size <= max_size)
	{
		c_rockall_cache *cache = find_cache(size);
		long actual_size = cache->allocation_size;
		cache->multiple_new(actual, array, requested);
		if (space != 0)
		{
			*space = actual_size;
		}
		if (zero)
		{
			for (long index = *actual - 1; index >= 0; index--)
			{
				memset(array[index], 0, actual_size);
			}
		}
		return *actual == requested;
	}
	// one at a time (New inlined, without its space and zeroing)
	while (*actual < requested)
	{
		void *memory = 0;
		if (active)
		{
			memory = size <= max_size ? find_cache(size)->new_memory() : external_cache->new_external(false, size);
		}
		array[*actual] = memory;
		if (array[*actual] == 0)
		{
			break;
		}
		(*actual)++;
	}
	if (space != 0)
	{
		*space = size;
	}
	return *actual == requested;
}

// @retail 0x32cc40
void *c_rockall_heap::resize(void *address, long new_size, long move, long *space, bool no_delete, bool zero)
{
	if (!active)
	{
		return 0;
	}
	long old_space;
	if (!details(address, 0, &old_space))
	{
		return 0;
	}
	long copy = old_space < new_size ? old_space : new_size;
	if (copy <= 0)
	{
		return 0;
	}
	c_rockall_cache *cache = 0;
	long actual;
	if (new_size <= max_size)
	{
		cache = find_cache(new_size);
		actual = cache->allocation_size;
	}
	else
	{
		actual = new_size;
	}
	// move when the size changes by at least 'move' (or, for a negative
	// move, grows or shrinks by more than it), never for a zero move
	long delta = actual - old_space;
	bool moving;
	if (move > 0)
	{
		moving = (delta < 0 ? -delta : delta) >= move;
	}
	else if (move == 0)
	{
		moving = false;
	}
	else
	{
		moving = delta > 0 || delta <= move;
	}
	if (!moving)
	{
		if (delta > 0)
		{
			return 0;
		}
		if (space != 0)
		{
			*space = old_space;
		}
		return address;
	}
	byte *memory = cache != 0 ? (byte *)cache->new_memory() : (byte *)external_cache->new_external(false, new_size);
	if (memory != 0)
	{
		memcpy(memory, address, copy);
		if (space != 0)
		{
			*space = actual;
		}
		if (!no_delete && !delete_memory(address, old_space))
		{
			rockall_failure_32ae60("Deleting allocation in Resize", true);
		}
		if (zero && actual - copy > 0)
		{
			memset(memory + copy, 0, actual - copy);
		}
	}
	return memory;
}

// ---------------------------------------------------------------------------
// the find

// @retail 0x32cdf0
c_rockall_find *c_rockall_find::construct(long hash_size, long new_max_lookaside, long new_threshold,
	bool new_shared_links, bool new_resize, c_rockall_back_end *new_back_end, s_rockall_thread_lock *new_thread_lock)
{
	h2_retail_32ee20(this);
	sharelock.construct(0x1000, 0x100);
	h2_retail_32ae90(&lock, 0x1000, 0x100);
	long align_mask = new_back_end->natural_size() - 1;
	long log_hash;
	long log_lookaside;
	if (!rockall_power_of_two_32ee00(align_mask + 1) || new_threshold < 0 || hash_size < 0x400 ||
		!rockall_log2_32edd0(hash_size, &log_hash) || new_max_lookaside < 0x80 ||
		!rockall_log2_32edd0(new_max_lookaside, &log_lookaside))
	{
		rockall_failure_32ae60("Hash table size in constructor for FIND", true);
		return this;
	}
	max_hash = hash_size;
	long size = new_max_lookaside * 8 + hash_size * 8;
	hash_shift = 32 - log_hash;
	hash_mask = (1u << log_hash) - 1;
	shared_links = new_shared_links;
	max_lookaside = new_max_lookaside;
	resize = new_resize;
	top_mask = -1;
	min_mask = (unsigned long)-1;
	lookaside_shift = 32 - log_lookaside;
	lookaside_mask = (1u << log_lookaside) - 1;
	lookaside_threshold = new_threshold;
	used = 0;
	back_end = new_back_end;
	thread_lock = new_thread_lock;
	byte *memory = (byte *)back_end->new_area(align_mask, size, false);
	hash = (s_rockall_list *)memory;
	lookaside = (s_rockall_lookaside *)(memory + max_hash * 8);
	if (memory == 0)
	{
		rockall_failure_32ae60("Create hash fails in constructor for FIND", true);
		return this;
	}
	for (long index = 0; index < hash_size; index++)
	{
		hash[index].construct();
	}
	for (long index = 0; index < max_lookaside; index++)
	{
		lookaside[index].address = (unsigned long)-1;
		lookaside[index].page = (c_rockall_page *)-1;
	}
	pages = 0;
	return this;
}

// the lookaside's page for an address's key (counting the lookups until the
// lookaside is used)
inline bool c_rockall_find::lookaside_hit(unsigned long key, c_rockall_page **page)
{
	if (used >= lookaside_threshold)
	{
		s_rockall_lookaside *entry = &lookaside[rockall_hash(key, lookaside_shift, lookaside_mask)];
		if (key == entry->address)
		{
			*page = entry->page;
			return true;
		}
		return false;
	}
	used++;
	return false;
}

// @retail 0x32cff0
void c_rockall_find::delete_page(c_rockall_page *page)
{
	unsigned long address = (unsigned long)page->address;
	rockall_find_claim_exclusive(this);
	s_rockall_list *bucket = &hash[rockall_hash(address, hash_shift, hash_mask)];
	(shared_links ? &page->shared_find : &page->links.find)->remove(bucket);
	if (used >= lookaside_threshold)
	{
		// forget the page wherever the lookaside remembers it
		c_rockall_cache *cache = page->cache;
		long step = cache->allocation_size;
		if (step <= (long)min_mask)
		{
			step = (long)min_mask + 1;
		}
		for (long offset = 0; offset < cache->page_size; offset += step)
		{
			unsigned long key = (offset + address) & ~min_mask;
			s_rockall_lookaside *entry = &lookaside[rockall_hash(key, lookaside_shift, lookaside_mask)];
			if (key == entry->address)
			{
				entry->address = (unsigned long)-1;
				entry->page = (c_rockall_page *)-1;
			}
		}
	}
	pages--;
	rockall_find_release_exclusive(this);
}

// @retail 0x32d140
bool c_rockall_find::known_area(void *address, c_rockall_cache *cache)
{
	rockall_find_claim_share(this);
	c_rockall_page *page = cache->find_parent_page(address, this);
	rockall_find_release_share(this);
	return page != 0;
}

// entered with the lock claimed shared: remembers the page, and releases it
// @retail 0x32d1f0
void c_rockall_find::update_find(void *address, c_rockall_page *page, long page_version)
{
	if (used >= lookaside_threshold)
	{
		if (rockall_must_lock(thread_lock))
		{
			rockall_share_to_exclusive(&sharelock);
		}
		if (page_version == page->version)
		{
			unsigned long key = (unsigned long)address & ~min_mask;
			s_rockall_lookaside *entry = &lookaside[rockall_hash(key, lookaside_shift, lookaside_mask)];
			entry->address = key;
			entry->page = page;
		}
		rockall_find_release_exclusive(this);
	}
	else
	{
		rockall_find_release_share(this);
	}
}

// @retail 0x32d300
void c_rockall_find::update_lookaside(long new_top_mask, unsigned long new_min_mask)
{
	if (top_mask != -1 && top_mask != new_top_mask)
	{
		rockall_failure_32ae60("Different 'TopCache' sizes with 'SingleImage'", true);
		return;
	}
	rockall_find_claim_exclusive(this);
	if (min_mask > new_min_mask)
	{
		min_mask = new_min_mask;
	}
	top_mask = new_top_mask;
	rockall_find_release_exclusive(this);
}

// @retail 0x32d3f0
bool c_rockall_find::walk(bool *active, void **address, c_rockall_cache *cache, long *space)
{
	void *current = *address;
	bool result = false;
	s_rockall_search search;
	rockall_find_claim_share(this);
	if (current == 0)
	{
		search.address = 0;
		search.cache = cache;
		search.page = 0;
		// a page walk without a page: the new pages give the first one
		result = c_rockall_page::walk(&search, this);
		if (result)
		{
			*active = (search.mask & *search.vector) != 0;
			*address = search.address;
			*space = search.page->actual_size();
		}
		rockall_find_release_share(this);
		return result;
	}
	unsigned long key = (unsigned long)current & ~min_mask;
	c_rockall_page *page = 0;
	bool miss = !lookaside_hit(key, &page);
	if (miss)
	{
		page = cache->find_parent_page(current, this);
	}
	if (page != 0 && (page = page->find_page(current, &search, this, true)) != 0 && search.found)
	{
		result = c_rockall_page::walk(&search, this);
		if (result)
		{
			*active = (search.mask & *search.vector) != 0;
			*address = search.address;
			*space = search.page->actual_size();
			if (miss)
			{
				miss = ((unsigned long)search.address & ~min_mask) == ((unsigned long)current & ~min_mask);
			}
		}
	}
	if (miss && result)
	{
		update_find(current, page, page->version);
		return result;
	}
	rockall_find_release_share(this);
	return result;
}

// @retail 0x32d600
void c_rockall_find::destruct()
{
	long size = max_lookaside * 8 + max_hash * 8;
	for (long index = 0; index < max_hash; index++)
	{
		hash[index].destruct();
	}
	back_end->delete_area(hash, size, false);
	lock.destruct();
	sharelock.destruct();
	rockall_environment_destruct_32ee60(this);
}

// @retail 0x32d6a0
bool c_rockall_find::delete_memory(void *address, c_rockall_cache *cache)
{
	rockall_find_claim_share(this);
	unsigned long key = (unsigned long)address & ~min_mask;
	c_rockall_page *page = 0;
	bool miss = !lookaside_hit(key, &page);
	if (miss)
	{
		c_rockall_page *parent_page = cache->find_parent_page(address, this);
		page = parent_page != 0 ? parent_page->find_page(address, 0, this, true) : 0;
	}
	if (page == 0)
	{
		rockall_find_release_share(this);
		return false;
	}
	c_rockall_cache *owner = page->cache;
	long page_version = page->version;
	if (miss)
	{
		update_find(address, page, page_version);
	}
	else
	{
		rockall_find_release_share(this);
	}
	return owner->delete_memory(address, page, page_version);
}

// @retail 0x32d870
bool c_rockall_find::details(void *address, s_rockall_search *search, c_rockall_cache *cache, long *space)
{
	rockall_find_claim_share(this);
	unsigned long key = (unsigned long)address & ~min_mask;
	c_rockall_page *page = 0;
	bool miss = !lookaside_hit(key, &page);
	if (miss)
	{
		page = cache->find_parent_page(address, this);
		if (page != 0)
		{
			page = page->find_page(address, search, this, true);
		}
	}
	else if (search != 0)
	{
		page = page->find_page(address, search, this, true);
	}
	bool result = page != 0;
	if (result)
	{
		*space = page->actual_size();
	}
	if (miss && result)
	{
		update_find(address, page, page->version);
		return result;
	}
	rockall_find_release_share(this);
	return result;
}

// @retail 0x32d9c0
c_rockall_page *c_rockall_find::find_page(void *address, c_rockall_cache *parent)
{
	unsigned long key = (unsigned long)address;
	s_rockall_list *bucket = &hash[rockall_hash(key, hash_shift, hash_mask)];
	if (!shared_links)
	{
		for (c_rockall_page *page = rockall_page_of_find(bucket->next); page != 0;
			page = rockall_page_of_find(page->links.find.next))
		{
			if (address == page->address && parent == page->parent)
			{
				return page;
			}
		}
		return 0;
	}
	for (c_rockall_page *page = rockall_page_of_shared_find(bucket->next); page != 0;
		page = rockall_page_of_shared_find(page->shared_find.next))
	{
		if (address == page->address && parent == page->parent)
		{
			return page;
		}
	}
	return 0;
}

// entered with the lock claimed exclusive: doubles the hash table and the
// lookaside once the table is three quarters full (giving up the lock while
// it takes the memory)
// @retail 0x32da70
void c_rockall_find::resize_hash_table()
{
	if (!resize || !__sync_bool_compare_and_swap(&lock.spinlock, 0, 1))
	{
		return;
	}
	long align_mask = back_end->natural_size() - 1;
	long new_max_hash = max_hash * 2;
	long new_max_lookaside = max_lookaside * 2;
	long hash_bytes = new_max_hash * 8;
	long old_size = max_lookaside * 8 + max_hash * 8;
	long log_hash;
	long log_lookaside;
	if (rockall_power_of_two_32ee00(align_mask + 1) && new_max_hash > 0 && rockall_log2_32edd0(new_max_hash, &log_hash) &&
		new_max_lookaside > 0 && rockall_log2_32edd0(new_max_lookaside, &log_lookaside) && max_hash / 4 + pages > max_hash)
	{
		rockall_find_release_exclusive(this);
		byte *memory = (byte *)back_end->new_area(align_mask, hash_bytes + new_max_lookaside * 8, false);
		s_rockall_list *new_hash = (s_rockall_list *)memory;
		s_rockall_lookaside *new_lookaside = (s_rockall_lookaside *)(memory + hash_bytes);
		if (memory != 0)
		{
			for (long index = 0; index < new_max_hash; index++)
			{
				new_hash[index].construct();
			}
			for (long index = 0; index < new_max_lookaside; index++)
			{
				new_lookaside[index].address = (unsigned long)-1;
				new_lookaside[index].page = (c_rockall_page *)-1;
			}
		}
		rockall_find_claim_exclusive(this);
		if (memory != 0)
		{
			long old_max_hash = max_hash;
			s_rockall_list *old_hash = hash;
			max_hash = new_max_hash;
			hash_shift = 32 - log_hash;
			hash_mask = (1u << log_hash) - 1;
			lookaside_shift = 32 - log_lookaside;
			lookaside_mask = (1u << log_lookaside) - 1;
			max_lookaside = new_max_lookaside;
			hash = new_hash;
			lookaside = new_lookaside;
			// rehash every page
			for (long index = 0; index < old_max_hash; index++)
			{
				s_rockall_list *old_bucket = &old_hash[index];
				if (!shared_links)
				{
					while (old_bucket->next != 0)
					{
						c_rockall_page *page = rockall_page_of_find(old_bucket->next);
						s_rockall_list *bucket = &hash[rockall_hash((unsigned long)page->address, hash_shift, hash_mask)];
						page->links.find.remove(old_bucket);
						page->links.find.insert(bucket);
					}
				}
				else
				{
					while (old_bucket->next != 0)
					{
						c_rockall_page *page = rockall_page_of_shared_find(old_bucket->next);
						s_rockall_list *bucket = &hash[rockall_hash((unsigned long)page->address, hash_shift, hash_mask)];
						page->shared_find.remove(old_bucket);
						page->shared_find.insert(bucket);
					}
				}
			}
			rockall_find_release_exclusive(this);
			for (long index = 0; index < old_max_hash; index++)
			{
				old_hash[index].destruct();
			}
			back_end->delete_area(old_hash, old_size, false);
			rockall_find_claim_exclusive(this);
		}
		else
		{
			resize = false;
		}
	}
	rockall_release_lock(&lock);
}

// @retail 0x32deb0
void c_rockall_find::insert_page(c_rockall_page *page)
{
	unsigned long address = (unsigned long)page->address;
	rockall_find_claim_exclusive(this);
	s_rockall_list *bucket = &hash[rockall_hash(address, hash_shift, hash_mask)];
	(shared_links ? &page->shared_find : &page->links.find)->insert(bucket);
	if (used >= lookaside_threshold)
	{
		// remember the page for each lookaside key it covers
		c_rockall_cache *cache = page->cache;
		long step = cache->allocation_size;
		if (step <= (long)min_mask)
		{
			step = (long)min_mask + 1;
		}
		for (long offset = 0; offset < cache->page_size; offset += step)
		{
			unsigned long key = (offset + address) & ~min_mask;
			s_rockall_lookaside *entry = &lookaside[rockall_hash(key, lookaside_shift, lookaside_mask)];
			entry->address = key;
			entry->page = page;
		}
	}
	pages++;
	if (max_hash / 4 + pages > max_hash)
	{
		resize_hash_table();
	}
	rockall_find_release_exclusive(this);
}

// ---------------------------------------------------------------------------
// the cache

// @retail 0x32e020
c_rockall_cache *c_rockall_cache::construct(long new_allocation_size, long new_cache_size, long new_chunk_size,
	long new_page_size, bool new_recycle, s_rockall_thread_lock *new_thread_lock)
{
	construct_bucket(new_allocation_size, new_chunk_size, new_page_size);
	h2_retail_32ae90(&lock, 0x1000, 0x100);
	if (new_cache_size < 0 || new_cache_size >= 0x1ffff)
	{
		rockall_failure_32ae60("Cache size in constructor for CACHE", true);
		return this;
	}
	recycle = new_recycle;
	cache_size = (short)new_cache_size;
	cache_active = false;
	children = 0;
	stack = 0;
	fill = 0;
	stack_top = 0;
	fill_top = 0;
	thread_lock = new_thread_lock;
	fill_size = 1;
	return this;
}

// @retail 0x32e0f0
void c_rockall_cache::create_cache_stack()
{
	if (stack == 0)
	{
		stack = (s_rockall_cache_entry *)new_pages->special_new(cache_size * 12);
	}
	if (fill == 0)
	{
		fill = (void **)new_pages->special_new(cache_size * 4);
		if (fill == 0)
		{
			return;
		}
	}
	if (stack != 0)
	{
		cache_active = true;
		fill_size = 1;
		stack_top = 0;
		fill_top = 0;
	}
}

// a page's memory for a child cache
// @retail 0x32e160
void *c_rockall_cache::create_data_page()
{
	rockall_cache_claim(this);
	void *memory = new_bucket_memory(true, -1);
	rockall_cache_release(this);
	return memory;
}

// @retail 0x32e1f0
bool c_rockall_cache::delete_memory(void *address, c_rockall_page *page, long page_version)
{
	bool result;
	rockall_cache_claim(this);
	if (cache_active)
	{
		if (recycle && (unsigned long)address < current_end && fill_top < cache_size)
		{
			// straight back into the ready allocations
			fill[fill_top] = address;
			fill_top++;
			result = true;
		}
		else
		{
			s_rockall_cache_entry *entry = &stack[stack_top];
			stack_top++;
			entry->address = address;
			entry->page = page;
			entry->version = page_version;
			result = true;
			if (stack_top >= cache_size)
			{
				long deleted;
				result = multiple_delete_bucket(stack, &deleted, stack_top);
				stack_top = 0;
			}
		}
	}
	else
	{
		result = delete_bucket_memory(address, page, page_version);
	}
	rockall_cache_release(this);
	return result;
}

// @retail 0x32e300
void c_rockall_cache::delete_all()
{
	cache_active = false;
	fill_size = 1;
	stack_top = 0;
	fill_top = 0;
}

// @retail 0x32e320
c_rockall_page *c_rockall_cache::find_child_page(void *address, c_rockall_find *the_find)
{
	return the_find->find_page((void *)((unsigned long)address & ~(unsigned long)(allocation_size - 1)), this);
}

// @retail 0x32e340
c_rockall_page *c_rockall_cache::find_parent_page(void *address, c_rockall_find *the_find)
{
	return the_find->find_page((void *)((unsigned long)address & ~(unsigned long)(page_size - 1)), parent);
}

// @retail 0x32e360
bool c_rockall_cache::multiple_new(long *actual, void **array, long requested)
{
	bool result;
	rockall_cache_claim(this);
	if (cache_active)
	{
		if (requested > fill_top)
		{
			if (recycle)
			{
				// the freed allocations become ready ones
				while (stack_top > 0 && fill_top < cache_size)
				{
					stack_top--;
					fill[fill_top] = stack[stack_top].address;
					fill_top++;
				}
			}
			if (requested > fill_top)
			{
				result = multiple_new_bucket(actual, array, requested);
				long limit = cache_size;
				if (fill_top <= limit / 2)
				{
					// refill the ready allocations
					long room = limit - fill_top;
					rockall_grow_fill_size(this);
					long count = fill_size < room ? fill_size : room;
					long filled;
					multiple_new_bucket(&filled, &fill[fill_top], count);
					fill_top += filled;
				}
				rockall_cache_release(this);
				return result;
			}
		}
		for (long index = 0; index < requested; index++)
		{
			fill_top--;
			array[index] = fill[fill_top];
		}
		*actual = requested;
		result = true;
	}
	else
	{
		if (cache_size > 1)
		{
			create_cache_stack();
		}
		result = multiple_new_bucket(actual, array, requested);
	}
	rockall_cache_release(this);
	return result;
}

// @retail 0x32e500
void *c_rockall_cache::new_memory()
{
	void *memory;
	rockall_cache_claim(this);
	if (cache_active)
	{
		if (fill_top > 0)
		{
			fill_top--;
			memory = fill[fill_top];
		}
		else if (stack_top > 0 && recycle)
		{
			stack_top--;
			memory = stack[stack_top].address;
		}
		else
		{
			rockall_grow_fill_size(this);
			if (multiple_new_bucket(&fill_top, fill, fill_size))
			{
				fill_top--;
				memory = fill[fill_top];
			}
			else
			{
				memory = 0;
			}
		}
	}
	else
	{
		if (cache_size > 1)
		{
			create_cache_stack();
		}
		memory = new_bucket_memory(false, -1);
	}
	rockall_cache_release(this);
	return memory;
}

// @retail 0x32e630
void *c_rockall_cache::new_external(bool sub_page, long new_size)
{
	rockall_cache_claim(this);
	void *memory = new_bucket_memory(sub_page, new_size);
	rockall_cache_release(this);
	return memory;
}

// @retail 0x32e6d0
void c_rockall_cache::release_cache_space(long max_free_pages)
{
	rockall_cache_claim(this);
	release_space(max_free_pages);
	rockall_cache_release(this);
}

// @retail 0x32e760
bool c_rockall_cache::search_cache(void *address)
{
	bool found = false;
	if (!cache_active)
	{
		return false;
	}
	rockall_cache_claim(this);
	if (cache_active)
	{
		for (long index = fill_top - 1; index >= 0 && !found; index--)
		{
			if (address == fill[index])
			{
				found = true;
			}
		}
		for (long index = stack_top - 1; index >= 0 && !found; index--)
		{
			if (address == stack[index].address)
			{
				found = true;
			}
		}
	}
	rockall_cache_release(this);
	return found;
}

// gives every cached allocation back to the bucket
// @retail 0x32e840
bool c_rockall_cache::truncate()
{
	bool result = true;
	long deleted;
	rockall_cache_claim(this);
	cache_active = false;
	fill_size = 1;
	if (stack_top > 0)
	{
		result = multiple_delete_bucket(stack, &deleted, stack_top);
		stack_top = 0;
	}
	if (fill_top > 0)
	{
		c_rockall_find *the_find = find;
		rockall_find_claim_share(the_find);
		for (fill_top--; fill_top >= 0; fill_top--)
		{
			void *address = fill[fill_top];
			c_rockall_page *page = find->find_page(
				(void *)((unsigned long)address & ~(unsigned long)(parent->allocation_size - 1)), parent);
			if (page != 0)
			{
				s_rockall_cache_entry *entry = &stack[stack_top];
				stack_top++;
				entry->address = address;
				entry->page = page;
				entry->version = page->version;
			}
			else
			{
				result = false;
			}
		}
		rockall_find_release_share(find);
	}
	if (stack_top > 0)
	{
		bool deleted_all = multiple_delete_bucket(stack, &deleted, stack_top);
		result = deleted_all && result;
		stack_top = 0;
	}
	rockall_cache_release(this);
	return result;
}

// @retail 0x32ea10
void c_rockall_cache::update_cache(c_rockall_heap *new_heap, c_rockall_new_pages *new_new_pages,
	c_rockall_cache *new_parent, c_rockall_find *new_find, c_rockall_find *new_public_find)
{
	if (new_parent != k_rockall_no_parent)
	{
		new_parent->children++;
	}
	update_bucket(new_heap, new_new_pages, new_parent, new_find, new_public_find);
}

// @retail 0x32ea30
void c_rockall_cache::destruct()
{
	if (cache_active)
	{
		rockall_failure_32ae60("Cache active in destructor for CACHE", true);
	}
	lock.destruct();
	destruct_bucket();
}

// a page's memory given back by a child cache
// @retail 0x32eaa0
bool c_rockall_cache::delete_data_page(void *address)
{
	bool result;
	s_rockall_search search;
	rockall_cache_claim(this);
	rockall_find_claim_share(find);
	c_rockall_page *page = find->find_page((void *)((unsigned long)address & ~(unsigned long)(page_size - 1)), parent);
	if (page != 0)
	{
		page = page->find_page(address, &search, find, false);
	}
	rockall_find_release_share(find);
	if (page != 0)
	{
		result = page->delete_memory(&search);
	}
	else
	{
		rockall_failure_32ae60("No data page in DeleteDataPage", true);
		// retail gives the low byte of the address here (Failure does not
		// return there)
		result = (byte)(unsigned long)address != 0;
	}
	rockall_cache_release(this);
	return result;
}

// ---------------------------------------------------------------------------
// the page

// @retail 0x32f0d0
c_rockall_page *c_rockall_page::construct(void *new_address, c_rockall_cache *new_cache, long new_size,
	c_rockall_cache *new_parent, long new_version)
{
	links.construct();
	shared_find.construct();
	short words = new_cache->vector_size;
	address = new_address;
	available = new_cache->elements;
	size = new_size;
	cache = new_cache;
	version = new_version;
	allocated = 0;
	first_word = 0;
	parent = new_parent;
	if (words > 0)
	{
		memset(vector, 0, words * 4);
	}
	return this;
}

// @retail 0x32f160
long c_rockall_page::actual_size()
{
	if (parent == k_rockall_no_parent)
	{
		return size;
	}
	return cache->allocation_size;
}

// @retail 0x32f180
bool c_rockall_page::delete_memory(s_rockall_search *search)
{
	if (!(*search->vector & search->mask) || !search->found)
	{
		return false;
	}
	*search->vector &= ~(search->sub_mask | search->mask);
	if (first_word > search->word)
	{
		first_word = (short)search->word;
	}
	if (available <= 0)
	{
		// free space again
		cache->insert_in_bucket_list(this);
	}
	allocated--;
	available++;
	if (allocated <= 0)
	{
		if (cache->parent != k_rockall_no_parent)
		{
			cache->new_pages->delete_page(this);
			return true;
		}
		cache->release_space(cache->heap->max_free_pages);
	}
	return true;
}

// keeps only the elements handed on as smaller pages
// @retail 0x32f210
void c_rockall_page::delete_all()
{
	bool was_full = available <= 0;
	c_rockall_cache *owner = cache;
	allocated = 0;
	available = owner->elements;
	first_word = 0;
	if (owner->children > 0)
	{
		long words = owner->vector_size;
		for (long index = 0; index < words; index++)
		{
			unsigned long word = vector[index];
			unsigned long pairs = (word >> 1) & word & 0x55555555;
			unsigned long kept = pairs * 2 | pairs;
			vector[index] = word & kept;
			if (kept != 0)
			{
				short count = 0;
				for (unsigned long bits = kept; bits != 0; bits >>= 2)
				{
					count = (short)(count + (bits & 1));
				}
				allocated = (short)(allocated + count);
				available = (short)(available - count);
			}
		}
	}
	else if (owner->vector_size > 0)
	{
		memset(vector, 0, owner->vector_size * 4);
	}
	if (was_full && available > 0)
	{
		owner->insert_in_bucket_list(this);
	}
}

// the page (here or, through the elements handed on, in a child cache) an
// address lies in, and where
// @retail 0x32f2c0
c_rockall_page *c_rockall_page::find_page(void *find_address, s_rockall_search *search, c_rockall_find *find,
	bool recursive)
{
	c_rockall_cache *owner = cache;
	if (owner->children <= 0 && search == 0)
	{
		return this;
	}
	bool exact;
	long index = owner->index_of((long)((unsigned long)find_address - (unsigned long)address), &exact);
	long word = index / 16;
	long shift = (word * 16 - index) * 2 + 0x1e;
	unsigned long mask = 2ul << shift;
	unsigned long sub_mask = 1ul << shift;
	if (recursive && (vector[word] & mask) && (vector[word] & sub_mask))
	{
		c_rockall_page *child = owner->find_child_page(find_address, find);
		if (child == 0)
		{
			return 0;
		}
		return child->find_page(find_address, search, find, recursive);
	}
	if (search != 0)
	{
		search->address = find_address;
		search->cache = cache;
		search->found = exact;
		search->mask = mask;
		search->page = this;
		search->sub_mask = sub_mask;
		search->vector = &vector[word];
		search->index = index;
		search->word = word;
		search->shift = shift;
	}
	return this;
}

// the free elements from the end of the array
// @retail 0x32f3b0
bool c_rockall_page::multiple_new(long *actual, void **array, long requested)
{
	if (available > 0 && *actual < requested)
	{
		long words = (unsigned short)cache->vector_size;
		while (first_word < (short)words)
		{
			long word = first_word;
			long base = word * 16;
			long bit = 0;
			unsigned long mask = 0x80000000;
			if ((vector[word] & 0xaaaaaaaa) != 0xaaaaaaaa)
			{
				for (;;)
				{
					unsigned long value = vector[word];
					while (mask & value)
					{
						mask >>= 2;
						bit++;
					}
					if (bit >= 16)
					{
						// retail Failure does not return: the port loops here
						rockall_failure_32ae60("Bit vector is corrupt in MultipleNew", true);
					}
					else
					{
						long index = base + bit;
						if (index >= cache->elements)
						{
							break;
						}
						allocated++;
						available--;
						vector[word] |= mask;
						if (available <= 0)
						{
							cache->delete_from_bucket_list(this);
						}
						array[requested - *actual - 1] = cache->address_of(address, index);
						(*actual)++;
						if (*actual >= requested)
						{
							return true;
						}
					}
					if ((vector[word] & 0xaaaaaaaa) == 0xaaaaaaaa)
					{
						break;
					}
				}
			}
			first_word++;
		}
	}
	return *actual >= requested;
}

// @retail 0x32f500
void *c_rockall_page::new_memory(bool sub_page)
{
	if (available <= 0)
	{
		return 0;
	}
	short words = cache->vector_size;
	while (first_word < words)
	{
		long word = first_word;
		unsigned long value = vector[word];
		if ((value & 0xaaaaaaaa) != 0xaaaaaaaa)
		{
			long bit = 0;
			unsigned long mask = 0x80000000;
			bool corrupt = false;
			if ((long)value < 0)
			{
				do
				{
					mask >>= 2;
					bit++;
				} while (mask & value);
				if (bit >= 16)
				{
					rockall_failure_32ae60("Bit vector is corrupt in New", true);
					corrupt = true;
				}
			}
			if (!corrupt)
			{
				long index = word * 16 + bit;
				if (index < cache->elements)
				{
					allocated++;
					available--;
					vector[word] |= sub_page ? (mask >> 1 | mask) : mask;
					if (available <= 0)
					{
						cache->delete_from_bucket_list(this);
					}
					return cache->address_of(address, index);
				}
			}
		}
		first_word++;
	}
	return 0;
}

// the next element after the search's (retail's 'this' is unused and it is
// called without a page too, so it is static here)
// @retail 0x32f5d0
bool c_rockall_page::walk(s_rockall_search *search, c_rockall_find *find)
{
	bool next_page = false;
	for (;;)
	{
		if (search->address != 0)
		{
			long elements = search->cache->elements;
			c_rockall_page *page = search->page;
			for (long index = search->index + (next_page ? 0 : 1); index < elements; index++)
			{
				void *element = page->cache->address_of(page->address, index);
				search->address = element;
				bool exact;
				long found = page->cache->index_of((long)((unsigned long)element - (unsigned long)page->address), &exact);
				long word = found / 16;
				long shift = (word * 16 - found) * 2 + 0x1e;
				search->address = element;
				search->cache = page->cache;
				search->found = exact;
				search->page = page;
				search->vector = &page->vector[word];
				search->mask = 2ul << shift;
				search->index = found;
				search->word = word;
				search->shift = shift;
				search->sub_mask = 1ul << shift;
				if (!(*search->vector & search->sub_mask))
				{
					return true;
				}
			}
		}
		next_page = true;
		if (!search->cache->new_pages->walk(search, find))
		{
			return false;
		}
	}
}

// @retail 0x32f6e0
void c_rockall_page::destruct()
{
	version++;
	shared_find.destruct();
	links.destruct();
}

// ---------------------------------------------------------------------------
// the bucket and its connections

// @retail 0x32f740
c_rockall_connections *c_rockall_connections::construct_connections()
{
	h2_retail_32ee20(this);
	active = false;
	heap = 0;
	new_pages = 0;
	parent = 0;
	find = 0;
	public_find = 0;
	return this;
}

// @retail 0x32f760
void c_rockall_connections::delete_from_find_list(c_rockall_page *page)
{
	if (public_find != 0)
	{
		public_find->delete_page(page);
	}
	if (find != 0)
	{
		find->delete_page(page);
	}
}

// @retail 0x32f790
void c_rockall_connections::insert_in_find_list(c_rockall_page *page)
{
	if (find != 0)
	{
		find->insert_page(page);
	}
	if (public_find != 0)
	{
		public_find->insert_page(page);
	}
}

// @retail 0x32f7c0
void c_rockall_connections::update_connections(c_rockall_heap *new_heap, c_rockall_new_pages *new_new_pages,
	c_rockall_cache *new_parent, c_rockall_find *new_find, c_rockall_find *new_public_find)
{
	if (!active)
	{
		heap = new_heap;
		new_pages = new_new_pages;
		parent = new_parent;
		find = new_find;
		active = true;
		public_find = new_public_find;
		return;
	}
	if (public_find != new_public_find || new_pages != new_new_pages || parent != new_parent)
	{
		rockall_failure_32ae60("Sharing violation in UpdateConnections", true);
	}
}

// @retail 0x32f820
void c_rockall_connections::destruct_connections()
{
	public_find = 0;
	find = 0;
	parent = 0;
	new_pages = 0;
	heap = 0;
	active = false;
	rockall_environment_destruct_32ee60(this);
}

// the arithmetic: an element's address in its page, and the element at an
// offset (with whether it is the element's start)

// @retail 0x32f840
extern "C" void *rockall_shift_address_32f840(c_rockall_bucket *self, void *page_address, long index)
{
	return (byte *)page_address + (index << (byte)self->allocation_shift);
}

// @retail 0x32f850
extern "C" void *rockall_multiply_address_32f850(c_rockall_bucket *self, void *page_address, long index)
{
	return (byte *)page_address + self->allocation_size * index;
}

// @retail 0x32f860
extern "C" void *rockall_chunk_shift_address_32f860(c_rockall_bucket *self, void *page_address, long index)
{
	long per_chunk = self->chunk_elements;
	long chunk = index / per_chunk;
	long rest = index - per_chunk * chunk;
	return (byte *)page_address + rest * self->allocation_size + (chunk << (byte)self->chunk_shift);
}

// @retail 0x32f890
extern "C" void *rockall_chunk_multiply_address_32f890(c_rockall_bucket *self, void *page_address, long index)
{
	long per_chunk = self->chunk_elements;
	long chunk = index / per_chunk;
	long rest = index - per_chunk * chunk;
	return (byte *)page_address + rest * self->allocation_size + self->chunk_size * chunk;
}

// @retail 0x32f8c0
extern "C" long rockall_shift_offset_32f8c0(c_rockall_bucket *self, long offset, bool *exact)
{
	long index = offset >> (byte)self->allocation_shift;
	*exact = offset == (index << (byte)self->allocation_shift);
	return index;
}

// @retail 0x32f8e0
extern "C" long rockall_multiply_offset_32f8e0(c_rockall_bucket *self, long offset, bool *exact)
{
	long size = self->allocation_size;
	long index = offset / size;
	*exact = offset == size * index;
	return index;
}

// @retail 0x32f900
extern "C" long rockall_chunk_shift_offset_32f900(c_rockall_bucket *self, long offset, bool *exact)
{
	long chunk = offset >> (byte)self->chunk_shift;
	long base = chunk << (byte)self->chunk_shift;
	long size = self->allocation_size;
	long index = (offset - base) / size;
	*exact = offset == size * index + base;
	return self->chunk_elements * chunk + index;
}

// @retail 0x32f950
extern "C" long rockall_chunk_multiply_offset_32f950(c_rockall_bucket *self, long offset, bool *exact)
{
	long chunk = offset / self->chunk_size;
	long base = self->chunk_size * chunk;
	long size = self->allocation_size;
	long index = (offset - base) / size;
	*exact = offset == size * index + base;
	return self->chunk_elements * chunk + index;
}

// @retail 0x32f9a0
bool c_rockall_bucket::delete_bucket_memory(void *address, c_rockall_page *page, long page_version)
{
	s_rockall_search search;
	return page->version == page_version && page->find_page(address, &search, find, false) != 0 &&
		page->delete_memory(&search);
}

// @retail 0x32f9f0
void c_rockall_bucket::delete_from_bucket_list(c_rockall_page *page)
{
	if (--active_pages < 0)
	{
		rockall_failure_32ae60("Active page count in DeleteFromBucketList", true);
		return;
	}
	page->links.bucket.remove(&bucket_list);
	c_rockall_page *first = (c_rockall_page *)bucket_list.next;
	current_end = first != 0 ? (unsigned long)first->address + page_size - 1 : (unsigned long)-1;
}

// in address order
// @retail 0x32fa40
void c_rockall_bucket::insert_in_bucket_list(c_rockall_page *page)
{
	active_pages++;
	c_rockall_page *first = (c_rockall_page *)bucket_list.next;
	if (first != 0)
	{
		c_rockall_page *last = (c_rockall_page *)bucket_list.previous;
		if ((unsigned long)page->address >= (unsigned long)last->address)
		{
			last->links.bucket.insert_after(&bucket_list, &page->links.bucket);
		}
		else
		{
			for (c_rockall_page *node = first; node != 0; node = (c_rockall_page *)node->links.bucket.next)
			{
				if ((unsigned long)page->address < (unsigned long)node->address)
				{
					node->links.bucket.insert_before(&bucket_list, &page->links.bucket);
					break;
				}
			}
		}
	}
	else
	{
		page->links.bucket.insert(&bucket_list);
	}
	current_end = (unsigned long)((c_rockall_page *)bucket_list.next)->address + page_size - 1;
}

// @retail 0x32fab0
bool c_rockall_bucket::multiple_delete_bucket(s_rockall_cache_entry *entries, long *deleted, long count)
{
	*deleted = 0;
	for (long index = 0; index < count; index++)
	{
		s_rockall_cache_entry *entry = &entries[index];
		c_rockall_page *page = entry->page;
		s_rockall_search search;
		if (entry->version == page->version && page->find_page(entry->address, &search, find, false) != 0 &&
			page->delete_memory(&search))
		{
			(*deleted)++;
		}
	}
	return *deleted == count;
}

// @retail 0x32fb30
bool c_rockall_bucket::multiple_new_bucket(long *actual, void **array, long requested)
{
	*actual = 0;
	do
	{
		c_rockall_page *page = (c_rockall_page *)bucket_list.next;
		while (page != 0)
		{
			c_rockall_page *next = (c_rockall_page *)page->links.bucket.next;
			if ((page->size == -1 || page->size == page_size) && page->multiple_new(actual, array, requested))
			{
				return true;
			}
			page = next;
		}
	} while (new_pages->create_page((c_rockall_cache *)this, -1) != 0);
	if (*actual >= requested)
	{
		return true;
	}
	if (*actual <= 0)
	{
		return false;
	}
	// the pages fill from the end: move what there is to the front
	long start = requested - *actual;
	for (long index = 0; index < *actual; index++)
	{
		array[index] = array[start + index];
	}
	return true;
}

// @retail 0x32fbd0
void *c_rockall_bucket::new_bucket_memory(bool sub_page, long new_size)
{
	do
	{
		for (c_rockall_page *page = (c_rockall_page *)bucket_list.next; page != 0;
			page = (c_rockall_page *)page->links.bucket.next)
		{
			if (page->size == -1 || page->size == (new_size == -1 ? page_size : new_size))
			{
				return page->new_memory(sub_page);
			}
		}
	} while (new_pages->create_page((c_rockall_cache *)this, new_size) != 0);
	return 0;
}

// gives back empty pages from the end of the list, down to a number kept
// @retail 0x32fc30
void c_rockall_bucket::release_space(long max_free_pages)
{
	long free_pages = active_pages;
	if (free_pages <= max_free_pages)
	{
		return;
	}
	for (c_rockall_page *page = (c_rockall_page *)bucket_list.previous; page != 0 && free_pages > max_free_pages;)
	{
		c_rockall_page *previous = (c_rockall_page *)page->links.bucket.previous;
		if (page->allocated <= 0)
		{
			free_pages--;
			new_pages->delete_page(page);
		}
		page = previous;
	}
}

// @retail 0x32fc70
void c_rockall_bucket::update_bucket(c_rockall_heap *new_heap, c_rockall_new_pages *new_new_pages,
	c_rockall_cache *new_parent, c_rockall_find *new_find, c_rockall_find *new_public_find)
{
	short key = new_new_pages->find_size_index((short)(unsigned short)elements);
	if (key == -1)
	{
		rockall_failure_32ae60("Bucket can't get a size key in UpdateBucket", true);
		return;
	}
	size_key = key;
	update_connections(new_heap, new_new_pages, new_parent, new_find, new_public_find);
}

// @retail 0x32fcd0
void c_rockall_bucket::destruct_bucket()
{
	bucket_list.destruct();
	destruct_connections();
}

// @retail 0x32fd20
c_rockall_bucket *c_rockall_bucket::construct_bucket(long new_allocation_size, long new_chunk_size, long new_page_size)
{
	construct_connections();
	bucket_list.construct();
	if (new_allocation_size <= 0 || new_chunk_size < new_allocation_size || new_chunk_size > new_page_size ||
		!rockall_power_of_two_32ee00(new_page_size))
	{
		rockall_failure_32ae60("Configuration in constructor for BUCKET", true);
		return this;
	}
	chunk_size = new_chunk_size;
	allocation_size = new_allocation_size;
	page_size = new_page_size;
	active_pages = 0;
	allocation_shift = 0;
	chunk_shift = 0;
	// whole chunks of elements, or elements across chunks
	bool whole = chunk_size == page_size || chunk_size % allocation_size == 0;
	if (rockall_log2_32edd0(new_allocation_size, &allocation_shift) && whole)
	{
		compute_address = rockall_shift_address_32f840;
		compute_offset = rockall_shift_offset_32f8c0;
	}
	else if (whole)
	{
		compute_address = rockall_multiply_address_32f850;
		compute_offset = rockall_multiply_offset_32f8e0;
	}
	else if (rockall_log2_32edd0(chunk_size, &chunk_shift))
	{
		compute_address = rockall_chunk_shift_address_32f860;
		compute_offset = rockall_chunk_shift_offset_32f900;
	}
	else
	{
		compute_address = rockall_chunk_multiply_address_32f890;
		compute_offset = rockall_chunk_multiply_offset_32f950;
	}
	compute_address_adjust = 0;
	compute_offset_adjust = 0;
	size_key = -1;
	short per_chunk = (short)(chunk_size / allocation_size);
	chunk_elements = per_chunk;
	elements = (short)((page_size / chunk_size) * per_chunk);
	vector_size = (short)((elements - 1) / 16 + 1);
	return this;
}

// ---------------------------------------------------------------------------
// the front end

// as src/unknown_081780.cpp declares it
class c_library_32a850
{
public:
	c_library_32a850(void *a1, void *a2, long a3, long a4, long a5, long a6, void *a7, void *heap, long a9, long a10, long a11, long a12, long a13);
	virtual void v0() {}

	byte unknown04[0x30];
};

// a cache's details in the constructor's two tables (ended by a zero size)
struct s_rockall_cache_details
{
	long allocation_size;
	long cache_size;
	long chunk_size;
	long page_size;
};

struct c_rockall_front_end
{
	bool delete_memory(void *address, long size);
	bool details(void *address, long *space);
	bool known_area(void *address);
	bool multiple_delete(long actual, void **array, long size);
	bool multiple_new(long *actual, void **array, long requested, long size, long *space, bool zero);
	void *new_memory(long size, long *space, bool zero);
	void *resize(void *address, long new_size, long move, long *space, bool no_delete, bool zero);
	bool verify(void *address, long *space);
	void delete_all(bool recycle);
	void lock_all();
	bool truncate(long max_free_space);
	void unlock_all();
	bool walk(bool *active, void **address, long *space);
	void destruct();
	void *special_new(long size);

	void const *vtable;
	c_rockall_cache **caches;
	c_rockall_cache *cache_storage;
	c_rockall_heap *heap;
	c_rockall_new_pages *new_pages;
	c_rockall_find *private_find;
	c_rockall_find *public_find;
	c_rockall_back_end *back_end;
	s_rockall_thread_lock *thread_lock;
	bool global_delete;
	long guard;
	long cache_count;
	long total_size;
};

typedef char assert_c_rockall_front_end_size[sizeof(c_rockall_front_end) == 0x34 ? 1 : -1];
typedef char assert_c_library_32a850_size[sizeof(c_library_32a850) == 0x34 ? 1 : -1];

#define k_rockall_guard 0xdeadbeef

// the find every front end with global delete shares, its users, the lock
// over both, and the thread lock it is built with (a function-local static
// in retail, guarded by bit 0 of 0x480fb4)
#define g_rockall_public_find ((c_rockall_find *)0x480ef8)
#define g_rockall_public_find_users (*(long *)0x480f78)
#define g_rockall_public_find_lock ((s_rockall_spinlock *)0x480f7c)
#define g_rockall_public_thread_lock ((s_rockall_thread_lock *)0x480f90)
#define g_rockall_public_thread_lock_built (*(unsigned long *)0x480fb4)

// @retail 0x32a850
c_library_32a850::c_library_32a850(void *a1, void *a2, long a3, long a4, long a5, long a6, void *a7, void *heap, long a9, long a10, long a11, long a12, long a13)
{
	// a1, a2: the two cache tables; a3, a4, a5: the find's lookaside size,
	// lookaside threshold and hash size; a6: the most free space; a7: the
	// page sizes (ended by 0); a9: recycle; a10: global delete; a11, a12: the
	// strides; a13: thread safe
	c_rockall_front_end *self = (c_rockall_front_end *)this;
	s_rockall_cache_details *caches1 = (s_rockall_cache_details *)a1;
	s_rockall_cache_details *caches2 = (s_rockall_cache_details *)a2;
	long *page_sizes = (long *)a7;
	c_rockall_back_end *back_end = (c_rockall_back_end *)heap;

	self->vtable = (void const *)0x454980;
	long align_mask = back_end->natural_size() - 1;
	long size1 = 0;
	while (caches1[size1].allocation_size != 0)
	{
		size1++;
	}
	long size2 = 0;
	while (caches2[size2].allocation_size != 0)
	{
		size2++;
	}
	long size3 = 0;
	while (page_sizes[size3] != 0)
	{
		size3++;
	}
	self->cache_count = size1 + size2;
	self->total_size = self->cache_count * 0x8c + 0x148;
	self->caches = 0;
	self->cache_storage = 0;
	self->heap = 0;
	self->new_pages = 0;
	self->private_find = 0;
	self->public_find = 0;
	self->back_end = back_end;
	self->global_delete = (byte)a10 != 0;
	self->guard = (long)k_rockall_guard;
	if (rockall_power_of_two_32ee00(align_mask + 1) && size1 >= 1 && size2 >= 3 && a11 > 0 &&
		rockall_power_of_two_32ee00(a11) && a12 >= a11 && rockall_power_of_two_32ee00(a12))
	{
		byte *block = (byte *)self->back_end->new_area(align_mask, self->total_size, false);
		if (block != 0)
		{
			self->thread_lock = (s_rockall_thread_lock *)block;
			self->thread_lock->construct((byte)a13 != 0);
			byte *memory = block + sizeof(s_rockall_thread_lock);
			self->cache_storage = (c_rockall_cache *)memory;
			memory += self->cache_count * sizeof(c_rockall_cache);
			for (long index = 0; index < size1; index++)
			{
				s_rockall_cache_details *details = &caches1[index];
				self->cache_storage[index].construct(details->allocation_size, details->cache_size,
					details->chunk_size, details->page_size, (byte)a9 != 0, self->thread_lock);
			}
			for (long index = 0; index < size2; index++)
			{
				s_rockall_cache_details *details = &caches2[index];
				self->cache_storage[index + size1].construct(details->allocation_size, details->cache_size,
					details->chunk_size, details->page_size, (byte)a9 != 0, self->thread_lock);
			}
			self->caches = (c_rockall_cache **)memory;
			memory += self->cache_count * 4;
			for (long index = 0; index < self->cache_count; index++)
			{
				self->caches[index] = &self->cache_storage[index];
			}
			self->private_find = (c_rockall_find *)memory;
			memory += sizeof(c_rockall_find);
			self->private_find->construct(a5, a3, a4, false, true, self->back_end, self->thread_lock);
			if (self->global_delete)
			{
				rockall_claim_lock(g_rockall_public_find_lock);
				if (g_rockall_public_find_users++ == 0)
				{
					if (!(g_rockall_public_thread_lock_built & 1))
					{
						g_rockall_public_thread_lock_built |= 1;
						g_rockall_public_thread_lock->construct(true);
						// retail registers its destructor 0x379c30 with _atexit; the
						// port never runs exit handlers
					}
					self->public_find = g_rockall_public_find;
					g_rockall_public_find->construct(a5, a3, 0, true, true, self->back_end, g_rockall_public_thread_lock);
				}
				else
				{
					self->public_find = g_rockall_public_find;
				}
				rockall_release_lock(g_rockall_public_find_lock);
			}
			self->new_pages = (c_rockall_new_pages *)memory;
			memory += sizeof(c_rockall_new_pages);
			self->new_pages->construct(page_sizes, self->back_end, size3, self->thread_lock);
			self->heap = (c_rockall_heap *)memory;
			self->heap->construct(self->caches, self->caches + size1, a6, self->new_pages, self->private_find,
				self->public_find, self->back_end, size1, size2, a11, a12, self->thread_lock);
		}
		else
		{
			rockall_failure_32ae60("Heap constructor failed in ROCKALL_FRONT_END", true);
		}
	}
	else
	{
		rockall_failure_32ae60("Cache size in constructor for ROCKALL_FRONT_END", true);
	}
}

// the members' deleting destructors (the destructor takes the flags 0)

// @retail 0x32a7b0
extern "C" void *rockall_heap_deleting_32a7b0(c_rockall_heap *self, unsigned long flags)
{
	self->destruct();
	if (flags & 1)
	{
		free(self);
	}
	return self;
}

// @retail 0x32a7d0
extern "C" void *rockall_new_pages_deleting_32a7d0(c_rockall_new_pages *self, unsigned long flags)
{
	self->destruct();
	if (flags & 1)
	{
		free(self);
	}
	return self;
}

// @retail 0x32a7f0
extern "C" void *rockall_find_deleting_32a7f0(c_rockall_find *self, unsigned long flags)
{
	self->destruct();
	if (flags & 1)
	{
		free(self);
	}
	return self;
}

// @retail 0x32a810
extern "C" void *rockall_cache_deleting_32a810(c_rockall_cache *self, unsigned long flags)
{
	self->destruct();
	if (flags & 1)
	{
		free(self);
	}
	return self;
}

// @retail 0x32a830
extern "C" void *rockall_thread_lock_deleting_32a830(s_rockall_thread_lock *self, unsigned long flags)
{
	self->destruct();
	if (flags & 1)
	{
		free(self);
	}
	return self;
}

// @retail 0x32ac90
void c_rockall_front_end::destruct()
{
	vtable = (void const *)0x454980;
	if (guard == (long)k_rockall_guard && cache_count > 0 && total_size > 0)
	{
		rockall_heap_deleting_32a7b0(heap, 0);
		rockall_new_pages_deleting_32a7d0(new_pages, 0);
		if (global_delete)
		{
			rockall_claim_lock(g_rockall_public_find_lock);
			if (--g_rockall_public_find_users == 0)
			{
				rockall_find_deleting_32a7f0(public_find, 0);
			}
			rockall_release_lock(g_rockall_public_find_lock);
		}
		rockall_find_deleting_32a7f0(private_find, 0);
		for (long index = 0; index < cache_count; index++)
		{
			rockall_cache_deleting_32a810(&cache_storage[index], 0);
		}
		rockall_thread_lock_deleting_32a830(thread_lock, 0);
		// retail gives back the block from the caches on (0x24 past its start)
		back_end->delete_area(cache_storage, total_size, false);
		total_size = 0;
		cache_count = 0;
		guard = 0;
		global_delete = false;
		thread_lock = 0;
		back_end = 0;
		public_find = 0;
		private_find = 0;
		new_pages = 0;
		heap = 0;
		cache_storage = 0;
		caches = 0;
	}
}

// @retail 0x32ae00
extern "C" void *rockall_front_end_deleting_32ae00(c_rockall_front_end *self, unsigned long flags)
{
	self->destruct();
	if (flags & 1)
	{
		free(self);
	}
	return self;
}

// slot 0
// @retail 0x329e40
bool c_rockall_front_end::delete_memory(void *address, long size)
{
	if (guard == (long)k_rockall_guard)
	{
		return heap->delete_memory(address, size);
	}
	return false;
}

// slot 1
// @retail 0x329f70
bool c_rockall_front_end::details(void *address, long *space)
{
	if (guard == (long)k_rockall_guard)
	{
		return heap->details(address, 0, space);
	}
	return false;
}

// slot 2
// @retail 0x32a020
bool c_rockall_front_end::known_area(void *address)
{
	if (guard == (long)k_rockall_guard)
	{
		return heap->known_area(address);
	}
	return false;
}

// slot 3
// @retail 0x32a140
bool c_rockall_front_end::multiple_delete(long actual, void **array, long size)
{
	if (actual > 0 && array != 0 && guard == (long)k_rockall_guard)
	{
		return heap->multiple_delete(actual, array, size);
	}
	return false;
}

// slot 4
// @retail 0x32a1f0
bool c_rockall_front_end::multiple_new(long *actual, void **array, long requested, long size, long *space, bool zero)
{
	if (array != 0 && guard == (long)k_rockall_guard && requested > 0 && size >= 0)
	{
		if (size == 0)
		{
			size = 1;
		}
		return heap->multiple_new(actual, array, requested, size, space, zero);
	}
	return false;
}

// slot 5
// @retail 0x32a2c0
void *c_rockall_front_end::new_memory(long size, long *space, bool zero)
{
	if (guard == (long)k_rockall_guard && size >= 0)
	{
		if (size == 0)
		{
			size = 1;
		}
		return heap->new_memory(size, space, zero);
	}
	return 0;
}

// the front end's own slot 5, called through its vtable
class c_rockall_front_end_slots
{
public:
	virtual bool slot0(void *address, long size) = 0;
	virtual bool slot1(void *address, long *space) = 0;
	virtual bool slot2(void *address) = 0;
	virtual bool slot3(long actual, void **array, long size) = 0;
	virtual bool slot4(long *actual, void **array, long requested, long size, long *space, bool zero) = 0;
	virtual void *new_memory(long size, long *space, bool zero) = 0;
};

// slot 6
// @retail 0x32a370
void *c_rockall_front_end::resize(void *address, long new_size, long move, long *space, bool no_delete, bool zero)
{
	if (address == 0)
	{
		return ((c_rockall_front_end_slots *)this)->new_memory(new_size, space, zero);
	}
	if (guard == (long)k_rockall_guard && new_size >= 0)
	{
		if (new_size == 0)
		{
			new_size = 1;
		}
		return heap->resize(address, new_size, move, space, no_delete, zero);
	}
	return 0;
}

// slot 7
// @retail 0x32a630
bool c_rockall_front_end::verify(void *address, long *space)
{
	if (guard == (long)k_rockall_guard)
	{
		if (address != 0 && !heap->verify(address, space))
		{
			return false;
		}
		return true;
	}
	return false;
}

// slot 8
// @retail 0x329ef0
void c_rockall_front_end::delete_all(bool recycle)
{
	if (guard == (long)k_rockall_guard)
	{
		heap->delete_all(recycle);
	}
}

// slot 9
// @retail 0x32a0c0
void c_rockall_front_end::lock_all()
{
	if (guard == (long)k_rockall_guard)
	{
		heap->lock_all();
	}
}

// slot 10
// @retail 0x32a510
bool c_rockall_front_end::truncate(long max_free_space)
{
	if (guard == (long)k_rockall_guard)
	{
		return heap->truncate(max_free_space);
	}
	return false;
}

// slot 11
// @retail 0x32a5b0
void c_rockall_front_end::unlock_all()
{
	if (guard == (long)k_rockall_guard)
	{
		heap->unlock_all(false);
	}
}

// slot 12
// @retail 0x32a6f0
bool c_rockall_front_end::walk(bool *active, void **address, long *space)
{
	if (guard == (long)k_rockall_guard)
	{
		bool walking;
		if (heap->walk(&walking, address, space))
		{
			*active = walking;
			return true;
		}
	}
	return false;
}

// slot 15
// @retail 0x32a460
void *c_rockall_front_end::special_new(long size)
{
	if (guard == (long)k_rockall_guard && size > 0)
	{
		return heap->new_pages->special_new(size);
	}
	return 0;
}
