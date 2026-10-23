// @flags /O2 /GL-
// Havok (third-party physics library): copies an object's pairs out into two
// lists through its own method.

struct lists_2fc200;

class class_2fc200
{
public:
	void function_2fc200(lists_2fc200 *lists) const;
};

// @retail 0x2fde10
void __cdecl function_2fde10(class_2fc200 const *object, lists_2fc200 *lists)
{
	object->function_2fc200(lists);
}
