// @flags /O2 /GL-
// Havok (third-party physics library): string copies and a sized block
// handed back to the allocator.
#include <string.h>
#include "physics_allocator.h"

// @retail 0x2d8e10
char *__cdecl function_2d8e10(const char *string)
{
	char *copy = (char *)havok_allocator()->allocate(strlen(string) + 1, 0x11);
	strcpy(copy, string);
	return copy;
}

class class_2d8e50
{
public:
	long field_0;
	long length;

	void release();
};

// @retail 0x2d8e50
void class_2d8e50::release()
{
	havok_allocator()->release(this, length + 0xd, 0x11);
}
