// @flags /O2 /GL-
// Havok (third-party physics library): copying an object's description out,
// as a free function.

struct s_2df5b0;

class class_2df5b0
{
public:
	void function_2df5b0(s_2df5b0 *out) const;
};

// @retail 0x2fdfd0
void __cdecl function_2fdfd0(class_2df5b0 const *object, s_2df5b0 *out)
{
	object->function_2df5b0(out);
}
