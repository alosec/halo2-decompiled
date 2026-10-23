// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include <xtl.h>
#include "unknown_223b60.h"
#include "physical_memory.h"
#include "unknown_080e10.h"

void function_12d520(long a);

struct s_key
{
	long a;
	long b;
};

class c_resource_lock
{
public:
	virtual long method0(long handle);
	virtual void method1(long block);

	s_key key;
};

// score 4 (was 4): retail saves edi before it stores the call's arguments,
// ours stores the handle first and saves edi after it, so the two instructions
// and the offsets that follow them differ.
// @retail 0x18f260
long c_resource_lock::method0(long handle)
{
	return function_80e14(handle, 0, this->key.a, this->key.b, 30, 0.1f);
}

// @retail 0x18f350
void c_resource_lock::method1(long block)
{
	function_12d520(block);
}