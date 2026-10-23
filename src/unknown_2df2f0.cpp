// @flags /O2 /GL-
// Havok (third-party physics library): copying an object's description out,
// as a free function.

struct s_2ddcd0;

class class_2ddcd0
{
public:
	void function_2ddcd0(s_2ddcd0 *out) const;
};

// @retail 0x2df2f0
void __cdecl function_2df2f0(class_2ddcd0 const *object, s_2ddcd0 *out)
{
	object->function_2ddcd0(out);
}
