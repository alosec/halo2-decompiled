// @flags /O2 /GL-
// Havok (third-party physics library): an array of 16-byte entries giving
// its storage back, as a free function.

class class_2fc610
{
public:
	long field_0;
	void *data;
	long size;
	unsigned long capacity;

	void release();
};

// @retail 0x2fc640
void __cdecl function_2fc640(class_2fc610 *array)
{
	array->release();
}
