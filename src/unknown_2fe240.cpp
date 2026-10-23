// @flags /O2 /GL-
// Havok (third-party physics library): copying an object's description out,
// as a free function.

struct s_2fe3f0;

class class_2fe3f0
{
public:
	void function_2fe3f0(s_2fe3f0 *out) const;
};

// @retail 0x2fe240
void __cdecl function_2fe240(class_2fe3f0 const *object, s_2fe3f0 *out)
{
	object->function_2fe3f0(out);
}
