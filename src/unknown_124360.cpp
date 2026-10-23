// @flags /O2 /Ob1 /Gr
#include "unknown_11c920.h"
#include "unknown_123b30.h"

struct s_saved_game_header;
struct s_saved_game_read;
bool __stdcall function_2174b0(long arg_5, void *arg_0, long arg_1, void *arg_2, long arg_3, s_saved_game_read *arg_4);

// @retail 0x124360
bool function_124360(long arg_2, s_saved_game_header *arg_0, s_saved_game_read *arg_1)
{
	return function_2174b0(arg_2, arg_0, 0x1288, NULL, 0x3fe000 - 0x1288, arg_1);
}

struct s_124790
{
	bool volatile field_0;
	byte field_1;
	bool field_2;
	byte field_3[0x10c - 3];
};

void function_120d50(bool volatile *arg_0, bool arg_1);
bool __stdcall function_217520(long arg_0, void const *arg_1, long arg_2, void const *arg_3, long arg_4, s_saved_game_read *arg_5);

// @retail 0x124790
bool function_124790(long arg_0, void *arg_1, long arg_2, long arg_3)
{
	bool local_0 = false;
	s_124790 local_1;
	void *const *local_2 = &arg_1;
	if (function_2174b0(arg_0, *local_2, arg_2, NULL, arg_3 - arg_2, (s_saved_game_read *)&local_1))
	{
		function_120d50(&local_1.field_0, true);
		local_0 = local_1.field_2;
	}
	return local_0;
}

// @retail 0x124800
void function_124800(long arg_0, void *arg_1, long arg_2)
{
	s_124790 local_0;
	if (function_2174b0(arg_0, NULL, 0, arg_1, arg_2, (s_saved_game_read *)&local_0))
		function_120d50(&local_0.field_0, true);
}

// @retail 0x124840
bool function_124840(long arg_0, void const *arg_1, long arg_2, long arg_3)
{
	bool local_0 = false;
	s_124790 local_1;
	void const *local_2 = buffer_after_header(arg_1, arg_2);
	long const *local_3 = &arg_0;
	if (function_217520(*local_3, arg_1, arg_2, local_2, arg_3 - arg_2, (s_saved_game_read *)&local_1))
	{
		function_120d50(&local_1.field_0, true);
		local_0 = local_1.field_2;
	}
	return local_0;
}

// @retail 0x124770
bool function_124770(long arg_0)
{
	return function_124840(arg_0, NULL, 0x1288, 0x3fe000);
}
