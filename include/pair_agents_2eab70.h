#ifndef PAIR_AGENTS_2EAB70_H
#define PAIR_AGENTS_2EAB70_H
// Havok (third-party physics library): objects that run a query between two
// sides of a pair (each side a shape, a key, a placement and a parent side)
// through a method, and their methods that run the same query with the two
// sides given the other way round, or with one side replaced by the child
// of the shape that wraps it.
#include <xmmintrin.h>

class collector_2ecc80;

class pair_agent_2ea410;

// a part's key, and the agent that runs queries against that part
struct agent_entry_2ea410
{
	long key;
	long field_4;
	pair_agent_2ea410 *agent;
};

// the agents of the parts of a pair's container side
class agent_list_2ea410
{
public:
	char unknown0[0xc];
	agent_entry_2ea410 *entries;
	long entry_count;
};

// runs a query between two sides through the dispatcher's function for
// their kinds
typedef void (__cdecl *pair_query_2e90f0)(void const *a, void const *b, void const *input, void *collector);
void __cdecl function_2e90f0(void const *a, void const *b, void const *input, void *collector);

// bring the agent list up to date with the parts the sides overlap
class class_2e96a0
{
public:
	void function_2e96a0(void const *a, void const *b, void const *input, pair_query_2e90f0 query);
};

class class_2e9d90
{
public:
	void function_2e9d90(void const *a, void const *b, void const *input, pair_query_2e90f0 query);
};

class class_2eab70
{
public:
	void function_2eab70(void const *a, void const *b, void const *input, void *collector);
	void function_2eb170(void const *a, void const *b, void const *input, void *collector);
	void function_2eb8d0(void const *a, void const *b, void const *input, void *collector);
	void function_2f9d40(void const *a, void const *b, void const *input, void *collector);
	void function_2fab30(void const *a, void const *b, void const *input, void *collector);
	void function_2faff0(void const *a, void const *b, void const *input, void *collector);
};

class class_2ea530 : public agent_list_2ea410
{
public:
	void function_2ea530(void const *a, void const *b, void const *input, void *collector, void *second_collector);
	void function_2eb3f0(void const *a, void const *b, void const *input, void *collector, void *second_collector);
	void function_2eb9f0(void const *a, void const *b, void const *input, void *collector, void *second_collector);
	void function_2f9740(void const *a, void const *b, void const *input, void *collector, void *second_collector);
	void function_2f9f20(void const *a, void const *b, void const *input, void *collector, void *second_collector);
	void function_2fb790(void const *a, void const *b, void const *input, void *collector, void *second_collector);
};

class class_2ea830 : public agent_list_2ea410
{
public:
	void function_2ea830(void const *a, void const *b, void const *input, void *collector);
	void function_2eb230(void const *a, void const *b, void const *input, void *collector);
	void function_2eb930(void const *a, void const *b, void const *input, void *collector);
	void function_2f9dc0(void const *a, void const *b, void const *input, void *collector);
	void function_2fabb0(void const *a, void const *b, void const *input, void *collector);
	void function_2fb070(void const *a, void const *b, void const *input, void *collector);
};

class class_2ecf20
{
public:
	char field_0[0xc];
	collector_2ecc80 *collector;

	void function_2ecf20(void const *a, void const *b, void const *input, void *collector);
	void function_2ed3a0(void const *a, void const *b, void const *input, void *collector);
};

// holds the terms that solve for a point's weights in a triangle's plane
class class_2f1560
{
public:
	char unknown0[0x10];
	float terms[4];

	void function_2f1560(void const *a, void const *b, void const *input, void *collector);
	void function_2f17d0(void const *a, void const *b, void const *input, void *collector);
};

class class_2f63d0
{
public:
	void function_2f63d0(void const *a, void const *b, void const *input, void *collector);
	void function_2f7790(void const *a, void const *b, void const *input, void *collector);
};

// holds the terms that solve for a point's weights in a triangle's plane
class class_2f8330
{
public:
	char unknown0[0xc];
	float terms[4];

	void function_2f8330(void const *a, void const *b, void const *input, void *collector);
	void function_2f8620(void const *a, void const *b, void const *input, void *collector);
};

class class_2f4290
{
public:
	void function_2f4290(void const *a, void const *b, void const *input, void *collector);
};

class class_2ed030
{
public:
	char field_0[0xc];
	collector_2ecc80 *collector;

	void function_2ed030(void const *a, void const *b, void *collector);
	void function_2ed4c0(void const *a, void const *b, void *collector);
};

// a collector of hits, each a point and a normal, in an array it points to
struct hit_2f88a0
{
	__m128 point;
	__m128 normal;
	char unknown20[0x30 - 0x20];
};

struct hits_2f88a0
{
	hit_2f88a0 *data;
	int size;
};

struct hit_collector_2f88a0
{
	hits_2f88a0 *hits;
};

class contact_owner_2f7c40;

// keeps the ids its owner hands out for each sphere of a shape of several, and
// the terms that solve for a point's weights in a triangle's plane
class class_2f7c40
{
public:
	char unknown0[8];
	contact_owner_2f7c40 *owner;
	float terms[4];
	unsigned short ids[8];

	void function_2f7c40(void const *a, void const *b, void const *input, void *collector);
	void function_2f88a0(void const *a, void const *b, void const *input, hit_collector_2f88a0 *collector);
};

class contact_manager_2f90a0;

struct flag_2f90a0
{
	char value;

	flag_2f90a0() {}
	flag_2f90a0(bool state) : value((char)state) {}
	__forceinline void operator=(bool state)
	{
		value = (char)state;
	}
	__forceinline flag_2f90a0 operator==(bool state) const
	{
		return flag_2f90a0(value == (char)state);
	}
	__forceinline operator bool() const
	{
		return value != 0;
	}
};

// whether a ray of the pair has a contact point, and its id
struct contact_entry_2f90a0
{
	flag_2f90a0 valid;
	char unknown1;
	unsigned short id;
};

struct contacts_2f90a0
{
	contact_entry_2f90a0 *data;
	long size;
	long capacity;

	__forceinline contact_entry_2f90a0 &operator[](int index)
	{
		return data[index];
	}
};

typedef char assert_flag_2f90a0[(sizeof(flag_2f90a0) == 1) ? 1 : -1];
typedef char assert_contact_entry_2f90a0[(sizeof(contact_entry_2f90a0) == 4) ? 1 : -1];
typedef char assert_contacts_2f90a0[(sizeof(contacts_2f90a0) == 0xc) ? 1 : -1];

class class_2f90a0
{
public:
	char unknown0[8];
	contact_manager_2f90a0 *manager;
	contacts_2f90a0 entries;
	contact_entry_2f90a0 inline_entries[4];

	void function_2f90a0(void const *a, void const *b, void const *input, void *collector);
	void function_2f94a0(void const *a, void const *b, void const *input, hit_collector_2f88a0 *collector);
};

class class_2fb150
{
public:
	void function_2fb150(void const *a, void const *b, void const *input, void *collector);
	void function_2fb910(void const *a, void const *b, void const *input, hit_collector_2f88a0 *collector);
};

class class_2ece10
{
public:
	char field_0[0xc];
	collector_2ecc80 *collector;

	void function_2ece10(void const *a, void const *b, void const *input, void *collector);
	void function_2ed400(void const *a, void const *b, void const *input, void *collector);
};

class class_2f0450
{
public:
	void function_2f0450(void const *a, void const *b, void const *input, void *collector);
	void function_2f0be0(void const *a, void const *b, void const *input, void *collector);
};

// holds the terms that solve for a point's weights in a triangle's plane
class class_2f1270
{
public:
	char unknown0[0x10];
	float terms[4];

	void function_2f1270(void const *a, void const *b, void const *input, void *collector);
	void function_2f1890(void const *a, void const *b, void const *input, void *collector);
};

class class_2f1ea0
{
public:
	void function_2f1ea0(void const *a, void const *b, void const *input, void *collector);
	void function_2f25e0(void const *a, void const *b, void const *input, void *collector);
};

class class_2f4f00
{
public:
	void function_2f4f00(void const *a, void const *b, void const *input, void *collector);
	void function_2f7850(void const *a, void const *b, void const *input, void *collector);
};

class class_2f7f10
{
public:
	void function_2f7f10(void const *a, void const *b, void const *input, void *collector);
	void function_2f86e0(void const *a, void const *b, void const *input, void *collector);
};

class class_2ea410 : public agent_list_2ea410
{
public:
	void function_2ea410(void const *a, void const *b, void const *input, void *collector);
	void function_2eb990(void const *a, void const *b, void const *input, hit_collector_2f88a0 *collector);
};

class class_2f0100
{
public:
	void function_2f0100(void const *a, void const *b, void const *input, void *collector);
	void function_2f0da0(void const *a, void const *b, void const *input, hit_collector_2f88a0 *collector);
};

class contact_owner_2f1080;

// keeps a contact id its owner hands out, and the terms that solve for a
// point's weights in a triangle's plane
class class_2f1080
{
public:
	char unknown0[8];
	contact_owner_2f1080 *owner;
	unsigned short id;
	float terms[4];

	void function_2f1080(void const *a, void const *b, void const *input, void *collector);
	void function_2f1a50(void const *a, void const *b, void const *input, hit_collector_2f88a0 *collector);
};

class contact_owner_2f1b80;

class class_2f1b80
{
public:
	char unknown0[8];
	contact_owner_2f1b80 *owner;
	unsigned short id;

	void function_2f1b80(void const *a, void const *b, void const *input, void *collector);
	void function_2f27a0(void const *a, void const *b, void const *input, hit_collector_2f88a0 *collector);
};

class class_2f4440
{
public:
	void function_2f4440(void const *a, void const *b, void const *input, void *collector);
	void function_2f7a10(void const *a, void const *b, void const *input, hit_collector_2f88a0 *collector);
};

class class_2fa2f0
{
public:
	void function_2fa2f0(void const *a, void const *b, void const *input, void *collector);
	void function_2faa60(void const *a, void const *b, void const *input, hit_collector_2f88a0 *collector);
};

class class_2fec10
{
public:
	void function_2fec10(void const *a, void const *b, void const *input, void *collector, void *second_collector);
	void function_2f9530(void const *a, void const *b, void const *input, void *collector, void *second_collector);
};

#endif
