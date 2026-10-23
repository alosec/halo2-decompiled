// @flags /O2 /GL-
// Havok (third-party physics library): copying an object's table of 32
// masks out and back in, as free functions.

struct s_2df840
{
	long field_0;
	unsigned long masks[32];
};

class class_2df840
{
public:
	char unknown0[0x18];
	long field_18;
	unsigned long masks[32];

	void get(s_2df840 *out) const;
	void set(const s_2df840 *in);
};

// @retail 0x2fbaa0
void __cdecl function_2fbaa0(class_2df840 const *object, s_2df840 *out)
{
	object->get(out);
}

// @retail 0x2fbab0
void __cdecl function_2fbab0(class_2df840 *object, const s_2df840 *in)
{
	object->set(in);
}
