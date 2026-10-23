#ifndef BLOCK_2F9B40_H
#define BLOCK_2F9B40_H
// Havok (third-party physics library): a block of vectors, words and two
// flags.
#include <xmmintrin.h>

struct block_2f9b40
{
	__m128 field_0;
	__m128 field_10;
	long field_20;
	long field_24;
	__m128 field_30;
	__m128 field_40;
	__m128 field_50;
	__m128 field_60;
	long field_70;
	long field_74;
	long field_78;
	bool field_7c;
	bool field_7d;

	block_2f9b40(block_2f9b40 const &other);
};

#endif
