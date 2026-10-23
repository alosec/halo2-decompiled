// @flags /O2 /GL-
// Havok (third-party physics library): copies an object's word and vectors
// out through its own method.

struct out_2fc820;

class class_2fc820
{
public:
	void function_2fc820(out_2fc820 *out) const;
};

// @retail 0x2fc530
void __cdecl function_2fc530(class_2fc820 const *object, out_2fc820 *out)
{
	object->function_2fc820(out);
}
