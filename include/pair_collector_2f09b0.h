#ifndef PAIR_COLLECTOR_2F09B0_H
#define PAIR_COLLECTOR_2F09B0_H
// Havok (third-party physics library): the collector a pair test reports each
// overlapping pair of bodies to, and the sphere-capsule test that uses it.

struct body_2f09b0;

class collector_2f09b0
{
public:
	virtual ~collector_2f09b0() {}
	virtual void add_pair(body_2f09b0 const *first, body_2f09b0 const *second) {}

	bool field_4;

	collector_2f09b0()
	{
		field_4 = false;
	}
};

// 0x2f09b0
void __cdecl function_2f09b0(body_2f09b0 const *sphere_body, body_2f09b0 const *capsule_body, void const *input, collector_2f09b0 *collector);

#endif
