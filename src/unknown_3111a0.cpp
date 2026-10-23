// @flags /O2 /GL-
// Havok (third-party physics library): a free function that copies an
// object's state out into a flat record (0x30c200).

struct link_30c200;
struct record_30c200;

class object_30c200
{
public:
	link_30c200 *link;
	void function_30c200(record_30c200 *out) const;
};

// @retail 0x3111a0
void __cdecl function_3111a0(object_30c200 const *object, record_30c200 *out)
{
	object->function_30c200(out);
}

class class_30c330
{
public:
	void function_30c330(void const *record);
};

// @retail 0x3111b0
void __cdecl function_3111b0(class_30c330 *object, void const *record)
{
	object->function_30c330(record);
}
