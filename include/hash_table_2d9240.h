#ifndef HASH_TABLE_2D9240_H
#define HASH_TABLE_2D9240_H
// Havok (third-party physics library): an open-addressing table of keys
// followed by their values; an empty key slot holds 0.

struct bool_2d9240
{
	bool value;

	bool_2d9240(bool value_)
	{
		value = value_;
	}
};

class class_2d9240
{
public:
	unsigned long *slots;
	long field_4;
	long mask;

	~class_2d9240();
	unsigned long find(unsigned long key) const;
	void function_2d93b0(unsigned long hole);
	int first() const;
	unsigned long word_at(int index) const;
	void store_word_at(int index, unsigned long value);
	int next(int index) const;
	bool_2d9240 is_valid(int index) const;
	void function_2d92f0(unsigned long key, unsigned long value);
	void function_2d94d0(int capacity);
};

#endif
