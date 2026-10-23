#ifndef EIGHT_VALUES_2D96C0_H
#define EIGHT_VALUES_2D96C0_H
// Havok (third-party physics library): a reference-counted object holding
// eight values, built from a structure or from eight arguments.
#include "reference_counted_2d88a0.h"

struct values_2d96c0
{
	long value_0;
	long value_4;
	long value_8;
	long value_c;
	long value_10;
	long value_14;
	long value_18;
	long value_1c;
};

class eight_values_2d96c0 : public referenced_2d88a0
{
public:
	REFERENCED_NEW_2D88A0(0x19)

	long field_8;
	long field_c;
	long field_10;
	long field_14;
	long field_18;
	long field_1c;
	long field_20;
	long field_24;

	eight_values_2d96c0(values_2d96c0 const &values);
	eight_values_2d96c0(long a, long b, long c, long d, long e, long f, long g, long h);
};

#endif
