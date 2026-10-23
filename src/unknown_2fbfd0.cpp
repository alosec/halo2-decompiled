// @flags /O2 /GL-
// Havok (third-party physics library): copying an object's description out,
// as a free function.
struct s_2fc4e0
{
	long field_0;
	long field_4;
};

class class_2fc4e0
{
public:
	char field_0[0xc];
	long field_c;

	void function_2fc4e0(s_2fc4e0 *out) const;
};

// @retail 0x2fbfd0
void __cdecl function_2fbfd0(class_2fc4e0 const *object, s_2fc4e0 *out)
{
	object->function_2fc4e0(out);
}
