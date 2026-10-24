// @flags /O2
// Retail C++ dynamic initialisers (the CRT's __xc table, 0x461194-0x46180c)
// that port/tools/gen_initializer_recoveries.py does not interpret: a loop, a
// call whose result is stored, and vector_constructor_iterator handed a
// constructor's code address. The runner, h2_run_retail_initializers
// (retail_initializers_generated.cpp), calls these in table order.
//
// Also here: three constructors the table calls that nothing else recovers
// (0x315700, 0x3155d0, 0x32ae90 with its helper 0x32ee20). The generated
// initialisers reach them through h2_retail_<address>.
#include <xtl.h>

extern "C" void h2_retail_31c000(void *self);
extern "C" void h2_retail_232d54(void *self);
extern "C" void h2_retail_232d67(void *self);

/* clears the first byte of 16 records of 0x170 bytes from 0x525a04 (the
   voice channels, g_525a00), then the byte at 0x525a00 */
// @retail 0x3769c0
extern "C" void retail_initializer_3769c0(void)
{
	unsigned char *record = (unsigned char *)0x525a04;
	for (int i = 0; i < 0x10; i++, record += 0x170)
	{
		*record = 0;
	}
	*(unsigned char *)0x525a00 = 0;
}

/* XNet's htons (0x3ce184, the bytes of a 16-bit value swapped) on a constant */
static inline unsigned short swap_bytes_3ce184(unsigned short value)
{
	return (unsigned short)((value >> 8) | (value << 8));
}

// @retail 0x376bc0
extern "C" void retail_initializer_376bc0(void)
{
	*(unsigned short *)0x546a80 = swap_bytes_3ce184(1000); // push 0x3e8
}

// @retail 0x376be0
extern "C" void retail_initializer_376be0(void)
{
	*(unsigned short *)0x546a84 = swap_bytes_3ce184(80); // push 0x50
}

/* the id of the thread that runs the initialisers, the main thread (retail
   calls the kernel thunk 0x2d2234) */
// @retail 0x377ef0
extern "C" void retail_initializer_377ef0(void)
{
	*(unsigned long *)0x557c88 = GetCurrentThreadId();
}

/* vector_constructor_iterator(0x55caf0, 0x114, 0x10, 0x232d54): 16
   s_postgame_player constructed in place */
// @retail 0x3781d1
extern "C" void retail_initializer_3781d1(void)
{
	unsigned char *element = (unsigned char *)0x55caf0;
	for (int i = 0; i < 0x10; i++, element += 0x114)
	{
		h2_retail_232d54(element);
	}
}

/* vector_constructor_iterator(0x55dc30, 0x84, 0x10, 0x232d67): 16
   s_postgame_team constructed in place */
// @retail 0x3781e8
extern "C" void retail_initializer_3781e8(void)
{
	unsigned char *element = (unsigned char *)0x55dc30;
	for (int i = 0; i < 0x10; i++, element += 0x84)
	{
		h2_retail_232d67(element);
	}
}

/* Havok: a contact constraint (0x31c000) with its own two tables, 0x414d0c
   and 0x414cd0 at +8 (built at 0x480770) */
// @retail 0x315700
extern "C" void *retail_constructor_315700(void *self)
{
	h2_retail_31c000(self);
	*(unsigned int *)self = 0x414d0c;
	*(unsigned int *)((unsigned char *)self + 8) = 0x414cd0;
	return self;
}

/* Havok: the same with the table 0x414ce0 (built at 0x480880) */
// @retail 0x3155d0
extern "C" void *retail_constructor_3155d0(void *self)
{
	h2_retail_31c000(self);
	*(unsigned int *)self = 0x414ce0;
	*(unsigned int *)((unsigned char *)self + 8) = 0x414cd0;
	return self;
}

/* the first construction of any object of 0x32ae90's kind sets up shared
   state: a count at 0x480fbc and four words after it */
// @retail 0x32ee20
extern "C" void retail_constructor_32ee20(void *self)
{
	(void)self;
	if (__atomic_add_fetch((long *)0x480fbc, 1, __ATOMIC_SEQ_CST) == 1)
	{
		*(unsigned int *)0x480fc0 = 0x10000;
		*(unsigned short *)0x480fc4 = 1;
		*(unsigned int *)0x480fc8 = 0x4000000;
		*(unsigned int *)0x480fcc = 0x1000;
	}
}

/* two words from the arguments and three cleared (built at 0x480f7c with
   0x1000 and 0x100) */
// @retail 0x32ae90
extern "C" void *retail_constructor_32ae90(void *self, unsigned int a, unsigned int b)
{
	unsigned int *words = (unsigned int *)self;
	retail_constructor_32ee20(self);
	words[0] = a;
	words[3] = 0;
	words[2] = 0;
	words[4] = 0;
	words[1] = b;
	return self;
}
