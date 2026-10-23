// @flags /O2 /GL-
// Havok (third-party physics library): the constructor of an object built
// on two interfaces, holding switches, a value passed in and seven empty
// arrays of four-byte values (two of them with room for one value inline).

// a one-byte boolean with a constructor
struct flag_3171a0
{
	char value;

	flag_3171a0(bool value_) : value((char)value_) {}
};

struct values_3171a0
{
	long *data;
	long size;
	unsigned long capacity_and_flags;

	values_3171a0() : data(0), size(0), capacity_and_flags(0x80000000) {}
};

// an array whose storage starts as one value inside the object
struct inline_values_3171a0
{
	long *data;
	long size;
	unsigned long capacity_and_flags;
	long storage[1];

	inline_values_3171a0()
	{
		data = storage;
		size = 0;
		capacity_and_flags = 1 | 0x80000000;
	}
};

class first_3171a0
{
public:
	first_3171a0() : field_8(0), field_c(0), field_10(0), field_14(0), field_18(0) {}
	virtual void slot_0() {}

	long field_4;
	long field_8;
	long field_c;
	long field_10;
	long field_14;
	long field_18;
};

class second_3171a0
{
public:
	virtual void slot_0() {}
};

class object_3171a0 : public first_3171a0, public second_3171a0
{
public:
	long field_20;
	long field_24;
	flag_3171a0 flag_28;
	flag_3171a0 flag_29;
	char unknown2a[2];
	flag_3171a0 flag_2c;
	char unknown2d[3];
	flag_3171a0 flag_30;
	char unknown31[3];
	long field_34;
	long field_38;
	inline_values_3171a0 values_3c;
	values_3171a0 values_4c;
	inline_values_3171a0 values_58;
	values_3171a0 values_68;
	values_3171a0 values_74;
	values_3171a0 values_80;
	values_3171a0 values_8c;
	char unknown98[0x10];
	long field_a8;
	short field_ac;
	char flag_ae;

	object_3171a0(long value);
};

// @retail 0x3171a0
object_3171a0::object_3171a0(long value)
	: field_20(value), flag_28(false), flag_29(true), flag_2c(false), flag_30(true), field_34(0), field_38(0)
{
	field_a8 = -1;
	field_ac = 0;
	flag_ae = 0;
}
