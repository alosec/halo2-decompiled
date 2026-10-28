// @flags /O2 /GL-
// Runtime class-descriptor getters. The three newly defined objects have
// 40-byte extents, as used by the startup calls at 0x3793f0..0x37950e.
struct s_class_descriptor_315b40
{
	unsigned long dispatch;
	unsigned short memory_size;
	unsigned short references;
	char const *name;
	void *parent;
	long interface_count;
	long object_size;
	void *enums;
	long enum_count;
	void *members;
	long member_count;
};
typedef char assert_descriptor_315b40[sizeof(s_class_descriptor_315b40) == 40 ? 1 : -1];
s_class_descriptor_315b40 g_480ac8;
s_class_descriptor_315b40 g_480b18;
s_class_descriptor_315b40 g_480b40;
// These singleton types and definitions already belong to unknown_3109b0.cpp.
struct object_480af0;
struct object_480b90;
extern object_480af0 g_480af0;
extern object_480b90 g_480b90;

// @retail 0x315b40
s_class_descriptor_315b40 *__cdecl function_315b40()
{
	return &g_480ac8;
}
// @retail 0x315b70
object_480af0 *__cdecl function_315b70()
{
	return &g_480af0;
}
// @retail 0x315b90
s_class_descriptor_315b40 *__cdecl function_315b90()
{
	return &g_480b18;
}
// @retail 0x315bd0
s_class_descriptor_315b40 *__cdecl function_315bd0()
{
	return &g_480b40;
}
// @retail 0x319290
object_480b90 *__cdecl function_319290()
{
	return &g_480b90;
}
