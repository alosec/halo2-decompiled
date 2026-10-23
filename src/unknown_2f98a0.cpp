// @flags /O2 /GL-
// Havok (third-party physics library): whether a dispatcher's table holds
// one particular creation function for a pair of kinds.

struct result_2f98a0
{
	char value;

	result_2f98a0(bool value_) : value((char)value_) {}
};

typedef void *(__cdecl *create_function_2f98a0)(long arg_0, long arg_1, long arg_2, long value);

void *__cdecl function_2ee390(long arg_0, long arg_1, long arg_2, long value);

struct dispatcher_2f98a0
{
	char unknown0[0x18c];
	create_function_2f98a0 functions[32][32];
};

// @retail 0x2f98a0
result_2f98a0 __cdecl function_2f98a0(dispatcher_2f98a0 const *dispatcher, long first, long second)
{
	if (dispatcher->functions[first][second] == function_2ee390)
	{
		return true;
	}
	return false;
}
