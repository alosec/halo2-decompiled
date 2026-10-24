// @flags /O2
// The CRT's static-object tail at the end of .text (0x3762e0-0x379c40): the
// initialisers that only register a destructor with _atexit (0x320fb4), and
// the destructors they register. These are compiler-generated around the
// game's and the libraries' (mostly Havok's) static objects:
//   - an _atexit registration: push the destructor, call _atexit
//   - a destructor resetting a static object's vtable to its base's
//   - a destructor calling the object's own destructor with the object's
//     retail address in ecx
//   - three that give a table back through the allocator its 0x34 holds
// The port never runs exit handlers (port/recovered/
// retail_initializers_generated.cpp skips the registrations), so none of
// this runs; it is here so every retail address has its code. The objects
// are at their retail addresses (the binder places them there).
//
// Not recovered from this stretch: the SEH unwind funclets (0x373bd0-
// 0x376110, which run on their parent's frame and are reached only from the
// exception tables), the CRT's x87 math pieces 0x373b60/0x373b74 (arguments
// on the x87 stack), and 0x379c4a (two bytes of padding).
// the guest CRT's (port/runtime/guest_crt.c)
extern "C" int atexit(void (*function)(void));

extern "C" void h2_retail_315510(void *self);
extern "C" void h2_retail_3156c0(void *self);
extern "C" void h2_retail_318970(void *self);
extern "C" void h2_retail_3192b0(void *self);
extern "C" void h2_retail_3202df(void *self);
extern "C" void h2_retail_32b070(void *self);
extern "C" void h2_retail_32ed60(void *self);
extern "C" void h2_retail_816f0(void *self);
extern "C" void h2_retail_3796c0(void);
extern "C" void h2_retail_3796d0(void);
extern "C" void h2_retail_3796f0(void);
extern "C" void h2_retail_379700(void);
extern "C" void h2_retail_379750(void);
extern "C" void h2_retail_379760(void);
extern "C" void h2_retail_379770(void);
extern "C" void h2_retail_379780(void);
extern "C" void h2_retail_379790(void);
extern "C" void h2_retail_3797a0(void);
extern "C" void h2_retail_3797b0(void);
extern "C" void h2_retail_3797c0(void);
extern "C" void h2_retail_3797d0(void);
extern "C" void h2_retail_3797e0(void);
extern "C" void h2_retail_3797f0(void);
extern "C" void h2_retail_379800(void);
extern "C" void h2_retail_379810(void);
extern "C" void h2_retail_379820(void);
extern "C" void h2_retail_379830(void);
extern "C" void h2_retail_379840(void);
extern "C" void h2_retail_379850(void);
extern "C" void h2_retail_379860(void);
extern "C" void h2_retail_379870(void);
extern "C" void h2_retail_379880(void);
extern "C" void h2_retail_3798c0(void);
extern "C" void h2_retail_379c10(void);

// a table that keeps the allocator it came from at 0x34
class allocator_3796d0
{
public:
	virtual void slot_0() = 0;
	virtual void release(void *block) = 0;
};

struct table_allocator_3796d0
{
	char unknown0[0x34];
	allocator_3796d0 *allocator;
};

// registers 0x3796c0 with _atexit
// @retail 0x3762e0
extern "C" void crt_tail_3762e0(void)
{
	atexit(h2_retail_3796c0);
}

// registers 0x3796d0 with _atexit
// @retail 0x376300
extern "C" void crt_tail_376300(void)
{
	atexit(h2_retail_3796d0);
}

// registers 0x3796f0 with _atexit
// @retail 0x376410
extern "C" void crt_tail_376410(void)
{
	atexit(h2_retail_3796f0);
}

// registers 0x379700 with _atexit
// @retail 0x376420
extern "C" void crt_tail_376420(void)
{
	atexit(h2_retail_379700);
}

// registers 0x379750 with _atexit
// @retail 0x376ab0
extern "C" void crt_tail_376ab0(void)
{
	atexit(h2_retail_379750);
}

// registers 0x379760 with _atexit
// @retail 0x376ac0
extern "C" void crt_tail_376ac0(void)
{
	atexit(h2_retail_379760);
}

// registers 0x379770 with _atexit
// @retail 0x376ad0
extern "C" void crt_tail_376ad0(void)
{
	atexit(h2_retail_379770);
}

// registers 0x379780 with _atexit
// @retail 0x376ae0
extern "C" void crt_tail_376ae0(void)
{
	atexit(h2_retail_379780);
}

// registers 0x379790 with _atexit
// @retail 0x376af0
extern "C" void crt_tail_376af0(void)
{
	atexit(h2_retail_379790);
}

// registers 0x3797a0 with _atexit
// @retail 0x376b00
extern "C" void crt_tail_376b00(void)
{
	atexit(h2_retail_3797a0);
}

// registers 0x3797b0 with _atexit
// @retail 0x376b10
extern "C" void crt_tail_376b10(void)
{
	atexit(h2_retail_3797b0);
}

// registers 0x3797c0 with _atexit
// @retail 0x376b20
extern "C" void crt_tail_376b20(void)
{
	atexit(h2_retail_3797c0);
}

// registers 0x3797d0 with _atexit
// @retail 0x376b30
extern "C" void crt_tail_376b30(void)
{
	atexit(h2_retail_3797d0);
}

// registers 0x3797e0 with _atexit
// @retail 0x376b40
extern "C" void crt_tail_376b40(void)
{
	atexit(h2_retail_3797e0);
}

// registers 0x3797f0 with _atexit
// @retail 0x376b50
extern "C" void crt_tail_376b50(void)
{
	atexit(h2_retail_3797f0);
}

// registers 0x379800 with _atexit
// @retail 0x376b60
extern "C" void crt_tail_376b60(void)
{
	atexit(h2_retail_379800);
}

// registers 0x379810 with _atexit
// @retail 0x376b70
extern "C" void crt_tail_376b70(void)
{
	atexit(h2_retail_379810);
}

// registers 0x379820 with _atexit
// @retail 0x376b80
extern "C" void crt_tail_376b80(void)
{
	atexit(h2_retail_379820);
}

// registers 0x379830 with _atexit
// @retail 0x376b90
extern "C" void crt_tail_376b90(void)
{
	atexit(h2_retail_379830);
}

// registers 0x379840 with _atexit
// @retail 0x376ba0
extern "C" void crt_tail_376ba0(void)
{
	atexit(h2_retail_379840);
}

// registers 0x379850 with _atexit
// @retail 0x376bb0
extern "C" void crt_tail_376bb0(void)
{
	atexit(h2_retail_379850);
}

// registers 0x379860 with _atexit
// @retail 0x377130
extern "C" void crt_tail_377130(void)
{
	atexit(h2_retail_379860);
}

// registers 0x379870 with _atexit
// @retail 0x377140
extern "C" void crt_tail_377140(void)
{
	atexit(h2_retail_379870);
}

// registers 0x379880 with _atexit
// @retail 0x377260
extern "C" void crt_tail_377260(void)
{
	atexit(h2_retail_379880);
}

// registers 0x3798c0 with _atexit
// @retail 0x377ec0
extern "C" void crt_tail_377ec0(void)
{
	atexit(h2_retail_3798c0);
}

// registers 0x379c10 with _atexit
// @retail 0x379600
extern "C" void crt_tail_379600(void)
{
	atexit(h2_retail_379c10);
}

// the table at 0x51f400 given back through its allocator (slot 1 of the object at its 0x34)
// @retail 0x3796d0
extern "C" void crt_tail_3796d0(void)
{
	table_allocator_3796d0 *const table = *(table_allocator_3796d0 **)0x51f400;
	if (table)
	{
		table->allocator->release(table);
	}
}

// c_lifecycle_network's destructor (src/unknown_0815e0.cpp) on the static object at 0x528000 (retail passes it in esi)
// @retail 0x379720
extern "C" void crt_tail_379720(void)
{
	h2_retail_816f0((void *)0x528000);
}

// the static object at 0x4797b0 back to its base's table
// @retail 0x379750
extern "C" void crt_tail_379750(void)
{
	*(unsigned long *)0x4797b0 = 0x450fa0;
}

// the static object at 0x4797b4 back to its base's table
// @retail 0x379760
extern "C" void crt_tail_379760(void)
{
	*(unsigned long *)0x4797b4 = 0x451058;
}

// the static object at 0x4797b8 back to its base's table
// @retail 0x379770
extern "C" void crt_tail_379770(void)
{
	*(unsigned long *)0x4797b8 = 0x451188;
}

// the static object at 0x4797bc back to its base's table
// @retail 0x379780
extern "C" void crt_tail_379780(void)
{
	*(unsigned long *)0x4797bc = 0x451240;
}

// the static object at 0x4797c0 back to its base's table
// @retail 0x379790
extern "C" void crt_tail_379790(void)
{
	*(unsigned long *)0x4797c0 = 0x451318;
}

// the static object at 0x4797c4 back to its base's table
// @retail 0x3797a0
extern "C" void crt_tail_3797a0(void)
{
	*(unsigned long *)0x4797c4 = 0x451410;
}

// the static object at 0x4797c8 back to its base's table
// @retail 0x3797b0
extern "C" void crt_tail_3797b0(void)
{
	*(unsigned long *)0x4797c8 = 0x4514e8;
}

// the static object at 0x4797cc back to its base's table
// @retail 0x3797c0
extern "C" void crt_tail_3797c0(void)
{
	*(unsigned long *)0x4797cc = 0x4516a8;
}

// the static object at 0x4797dc back to its base's table
// @retail 0x3797d0
extern "C" void crt_tail_3797d0(void)
{
	*(unsigned long *)0x4797dc = 0x4517a8;
}

// the static object at 0x4797e0 back to its base's table
// @retail 0x3797e0
extern "C" void crt_tail_3797e0(void)
{
	*(unsigned long *)0x4797e0 = 0x451b18;
}

// the static object at 0x47980c back to its base's table
// @retail 0x3797f0
extern "C" void crt_tail_3797f0(void)
{
	*(unsigned long *)0x47980c = 0x451f68;
}

// the static object at 0x479810 back to its base's table
// @retail 0x379800
extern "C" void crt_tail_379800(void)
{
	*(unsigned long *)0x479810 = 0x452030;
}

// the static object at 0x479814 back to its base's table
// @retail 0x379810
extern "C" void crt_tail_379810(void)
{
	*(unsigned long *)0x479814 = 0x4520f0;
}

// the static object at 0x479818 back to its base's table
// @retail 0x379820
extern "C" void crt_tail_379820(void)
{
	*(unsigned long *)0x479818 = 0x452210;
}

// the static object at 0x47982c back to its base's table
// @retail 0x379830
extern "C" void crt_tail_379830(void)
{
	*(unsigned long *)0x47982c = 0x451f68;
}

// the static object at 0x479848 back to its base's table
// @retail 0x379840
extern "C" void crt_tail_379840(void)
{
	*(unsigned long *)0x479848 = 0x452788;
}

// the static object at 0x47984c back to its base's table
// @retail 0x379850
extern "C" void crt_tail_379850(void)
{
	*(unsigned long *)0x47984c = 0x452848;
}

// the table at 0x54d570 given back through its allocator (slot 1 of the object at its 0x34)
// @retail 0x379880
extern "C" void crt_tail_379880(void)
{
	table_allocator_3796d0 *const table = *(table_allocator_3796d0 **)0x54d570;
	if (table)
	{
		table->allocator->release(table);
	}
}

// the static object at 0x4798b0 back to its base's table
// @retail 0x3798a0
extern "C" void crt_tail_3798a0(void)
{
	*(unsigned long *)0x4798b0 = 0x453760;
}

// the table at 0x557c6c given back through its allocator (slot 1 of the object at its 0x34)
// @retail 0x3798c0
extern "C" void crt_tail_3798c0(void)
{
	table_allocator_3796d0 *const table = *(table_allocator_3796d0 **)0x557c6c;
	if (table)
	{
		table->allocator->release(table);
	}
}

// the Havok singletons' destructor (src/unknown_3192b0.cpp) on the static object at 0x480120
// @retail 0x3798e0
extern "C" void crt_tail_3798e0(void)
{
	h2_retail_3192b0((void *)0x480120);
}

// the Havok singletons' destructor (src/unknown_3192b0.cpp) on the static object at 0x4801ac
// @retail 0x3798f0
extern "C" void crt_tail_3798f0(void)
{
	h2_retail_3192b0((void *)0x4801ac);
}

// the Havok singletons' destructor (src/unknown_3192b0.cpp) on the static object at 0x480184
// @retail 0x379900
extern "C" void crt_tail_379900(void)
{
	h2_retail_3192b0((void *)0x480184);
}

// the Havok singletons' destructor (src/unknown_3192b0.cpp) on the static object at 0x48027c
// @retail 0x379910
extern "C" void crt_tail_379910(void)
{
	h2_retail_3192b0((void *)0x48027c);
}

// the Havok singletons' destructor (src/unknown_3192b0.cpp) on the static object at 0x4802a8
// @retail 0x379920
extern "C" void crt_tail_379920(void)
{
	h2_retail_3192b0((void *)0x4802a8);
}

// the Havok singletons' destructor (src/unknown_3192b0.cpp) on the static object at 0x4802d0
// @retail 0x379930
extern "C" void crt_tail_379930(void)
{
	h2_retail_3192b0((void *)0x4802d0);
}

// the Havok singletons' destructor (src/unknown_3192b0.cpp) on the static object at 0x4802f8
// @retail 0x379940
extern "C" void crt_tail_379940(void)
{
	h2_retail_3192b0((void *)0x4802f8);
}

// the Havok singletons' destructor (src/unknown_3192b0.cpp) on the static object at 0x480320
// @retail 0x379950
extern "C" void crt_tail_379950(void)
{
	h2_retail_3192b0((void *)0x480320);
}

// the Havok singletons' destructor (src/unknown_3192b0.cpp) on the static object at 0x480348
// @retail 0x379960
extern "C" void crt_tail_379960(void)
{
	h2_retail_3192b0((void *)0x480348);
}

// the Havok singletons' destructor (src/unknown_3192b0.cpp) on the static object at 0x480370
// @retail 0x379970
extern "C" void crt_tail_379970(void)
{
	h2_retail_3192b0((void *)0x480370);
}

// the Havok singletons' destructor (src/unknown_3192b0.cpp) on the static object at 0x48039c
// @retail 0x379980
extern "C" void crt_tail_379980(void)
{
	h2_retail_3192b0((void *)0x48039c);
}

// the Havok singletons' destructor (src/unknown_3192b0.cpp) on the static object at 0x4803c4
// @retail 0x379990
extern "C" void crt_tail_379990(void)
{
	h2_retail_3192b0((void *)0x4803c4);
}

// the Havok singletons' destructor (src/unknown_3192b0.cpp) on the static object at 0x4803ec
// @retail 0x3799a0
extern "C" void crt_tail_3799a0(void)
{
	h2_retail_3192b0((void *)0x4803ec);
}

// the Havok singletons' destructor (src/unknown_3192b0.cpp) on the static object at 0x480414
// @retail 0x3799b0
extern "C" void crt_tail_3799b0(void)
{
	h2_retail_3192b0((void *)0x480414);
}

// the Havok singletons' destructor (src/unknown_3192b0.cpp) on the static object at 0x48043c
// @retail 0x3799c0
extern "C" void crt_tail_3799c0(void)
{
	h2_retail_3192b0((void *)0x48043c);
}

// the Havok singletons' destructor (src/unknown_3192b0.cpp) on the static object at 0x480464
// @retail 0x3799d0
extern "C" void crt_tail_3799d0(void)
{
	h2_retail_3192b0((void *)0x480464);
}

// the Havok singletons' destructor (src/unknown_3192b0.cpp) on the static object at 0x48048c
// @retail 0x3799e0
extern "C" void crt_tail_3799e0(void)
{
	h2_retail_3192b0((void *)0x48048c);
}

// the Havok singletons' destructor (src/unknown_3192b0.cpp) on the static object at 0x4804b4
// @retail 0x3799f0
extern "C" void crt_tail_3799f0(void)
{
	h2_retail_3192b0((void *)0x4804b4);
}

// the Havok singletons' destructor (src/unknown_3192b0.cpp) on the static object at 0x4804dc
// @retail 0x379a00
extern "C" void crt_tail_379a00(void)
{
	h2_retail_3192b0((void *)0x4804dc);
}

// the Havok singletons' destructor (src/unknown_3192b0.cpp) on the static object at 0x480504
// @retail 0x379a10
extern "C" void crt_tail_379a10(void)
{
	h2_retail_3192b0((void *)0x480504);
}

// the Havok singletons' destructor (src/unknown_3192b0.cpp) on the static object at 0x48052c
// @retail 0x379a20
extern "C" void crt_tail_379a20(void)
{
	h2_retail_3192b0((void *)0x48052c);
}

// the Havok singletons' destructor (src/unknown_3192b0.cpp) on the static object at 0x480554
// @retail 0x379a30
extern "C" void crt_tail_379a30(void)
{
	h2_retail_3192b0((void *)0x480554);
}

// the Havok singletons' destructor (src/unknown_3192b0.cpp) on the static object at 0x48057c
// @retail 0x379a40
extern "C" void crt_tail_379a40(void)
{
	h2_retail_3192b0((void *)0x48057c);
}

// the reference base's destructor (src/unknown_318970.cpp) on the static object at 0x4805a4
// @retail 0x379a50
extern "C" void crt_tail_379a50(void)
{
	h2_retail_318970((void *)0x4805a4);
}

// the Havok singletons' destructor (src/unknown_3192b0.cpp) on the static object at 0x4805b8
// @retail 0x379a60
extern "C" void crt_tail_379a60(void)
{
	h2_retail_3192b0((void *)0x4805b8);
}

// the Havok singletons' destructor (src/unknown_3192b0.cpp) on the static object at 0x4805e0
// @retail 0x379a70
extern "C" void crt_tail_379a70(void)
{
	h2_retail_3192b0((void *)0x4805e0);
}

// the Havok singletons' destructor (src/unknown_3192b0.cpp) on the static object at 0x480608
// @retail 0x379a80
extern "C" void crt_tail_379a80(void)
{
	h2_retail_3192b0((void *)0x480608);
}

// the Havok singletons' destructor (src/unknown_3192b0.cpp) on the static object at 0x480630
// @retail 0x379a90
extern "C" void crt_tail_379a90(void)
{
	h2_retail_3192b0((void *)0x480630);
}

// the Havok singletons' destructor (src/unknown_3192b0.cpp) on the static object at 0x480658
// @retail 0x379aa0
extern "C" void crt_tail_379aa0(void)
{
	h2_retail_3192b0((void *)0x480658);
}

// the Havok singletons' destructor (src/unknown_3192b0.cpp) on the static object at 0x480680
// @retail 0x379ab0
extern "C" void crt_tail_379ab0(void)
{
	h2_retail_3192b0((void *)0x480680);
}

// the Havok singletons' destructor (src/unknown_3192b0.cpp) on the static object at 0x4806a8
// @retail 0x379ac0
extern "C" void crt_tail_379ac0(void)
{
	h2_retail_3192b0((void *)0x4806a8);
}

// the Havok singletons' destructor (src/unknown_3192b0.cpp) on the static object at 0x4806d0
// @retail 0x379ad0
extern "C" void crt_tail_379ad0(void)
{
	h2_retail_3192b0((void *)0x4806d0);
}

// the Havok singletons' destructor (src/unknown_3192b0.cpp) on the static object at 0x4806f8
// @retail 0x379ae0
extern "C" void crt_tail_379ae0(void)
{
	h2_retail_3192b0((void *)0x4806f8);
}

// the Havok singletons' destructor (src/unknown_3192b0.cpp) on the static object at 0x480720
// @retail 0x379af0
extern "C" void crt_tail_379af0(void)
{
	h2_retail_3192b0((void *)0x480720);
}

// the Havok singletons' destructor (src/unknown_3192b0.cpp) on the static object at 0x480748
// @retail 0x379b00
extern "C" void crt_tail_379b00(void)
{
	h2_retail_3192b0((void *)0x480748);
}

// the contact constraint's destructor on the static object at 0x480990
// @retail 0x379b10
extern "C" void crt_tail_379b10(void)
{
	h2_retail_315510((void *)0x480990);
}

// a constraint's destructor on the static object at 0x480880
// @retail 0x379b20
extern "C" void crt_tail_379b20(void)
{
	h2_retail_3156c0((void *)0x480880);
}

// a constraint's destructor on the static object at 0x480770
// @retail 0x379b30
extern "C" void crt_tail_379b30(void)
{
	h2_retail_3156c0((void *)0x480770);
}

// the Havok singletons' destructor (src/unknown_3192b0.cpp) on the static object at 0x480ac8
// @retail 0x379b40
extern "C" void crt_tail_379b40(void)
{
	h2_retail_3192b0((void *)0x480ac8);
}

// the Havok singletons' destructor (src/unknown_3192b0.cpp) on the static object at 0x480aa0
// @retail 0x379b50
extern "C" void crt_tail_379b50(void)
{
	h2_retail_3192b0((void *)0x480aa0);
}

// the Havok singletons' destructor (src/unknown_3192b0.cpp) on the static object at 0x480af0
// @retail 0x379b60
extern "C" void crt_tail_379b60(void)
{
	h2_retail_3192b0((void *)0x480af0);
}

// the Havok singletons' destructor (src/unknown_3192b0.cpp) on the static object at 0x480b18
// @retail 0x379b70
extern "C" void crt_tail_379b70(void)
{
	h2_retail_3192b0((void *)0x480b18);
}

// the Havok singletons' destructor (src/unknown_3192b0.cpp) on the static object at 0x480b40
// @retail 0x379b80
extern "C" void crt_tail_379b80(void)
{
	h2_retail_3192b0((void *)0x480b40);
}

// the Havok singletons' destructor (src/unknown_3192b0.cpp) on the static object at 0x480b68
// @retail 0x379b90
extern "C" void crt_tail_379b90(void)
{
	h2_retail_3192b0((void *)0x480b68);
}

// the Havok singletons' destructor (src/unknown_3192b0.cpp) on the static object at 0x480b90
// @retail 0x379ba0
extern "C" void crt_tail_379ba0(void)
{
	h2_retail_3192b0((void *)0x480b90);
}

// the Havok singletons' destructor (src/unknown_3192b0.cpp) on the static object at 0x480bb8
// @retail 0x379bb0
extern "C" void crt_tail_379bb0(void)
{
	h2_retail_3192b0((void *)0x480bb8);
}

// the Havok singletons' destructor (src/unknown_3192b0.cpp) on the static object at 0x480bc0
// @retail 0x379bc0
extern "C" void crt_tail_379bc0(void)
{
	h2_retail_3192b0((void *)0x480bc0);
}

// the Havok singletons' destructor (src/unknown_3192b0.cpp) on the static object at 0x480be8
// @retail 0x379bd0
extern "C" void crt_tail_379bd0(void)
{
	h2_retail_3192b0((void *)0x480be8);
}

// the Havok singletons' destructor (src/unknown_3192b0.cpp) on the static object at 0x480c10
// @retail 0x379be0
extern "C" void crt_tail_379be0(void)
{
	h2_retail_3192b0((void *)0x480c10);
}

// the Havok singletons' destructor (src/unknown_3192b0.cpp) on the static object at 0x480c38
// @retail 0x379bf0
extern "C" void crt_tail_379bf0(void)
{
	h2_retail_3192b0((void *)0x480c38);
}

// the exception class's destructor on the static object at 0x480c80
// @retail 0x379bfa
extern "C" void crt_tail_379bfa(void)
{
	h2_retail_3202df((void *)0x480c80);
}

// the static object at 0x462cbc back to its base's table
// @retail 0x379c10
extern "C" void crt_tail_379c10(void)
{
	*(unsigned long *)0x462cbc = 0x41661c;
}

// a handle owner's destructor (CloseHandle) on the static object at 0x480f7c
// @retail 0x379c20
extern "C" void crt_tail_379c20(void)
{
	h2_retail_32b070((void *)0x480f7c);
}

// a section owner's destructor on the static object at 0x480f90
// @retail 0x379c30
extern "C" void crt_tail_379c30(void)
{
	h2_retail_32ed60((void *)0x480f90);
}

// the Havok singletons' destructor (src/unknown_3192b0.cpp) on the static object at 0x484b4c
// @retail 0x379c40
extern "C" void crt_tail_379c40(void)
{
	h2_retail_3192b0((void *)0x484b4c);
}

