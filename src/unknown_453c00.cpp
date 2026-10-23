// @flags /O2 /Gr
#include "unknown_11c920.h"

// the callback table at 0x453c00: pairs of dispose and initialize procedures
void function_81f80(void);
void __stdcall function_81780(long stage);
void function_1a479a(void);
void __stdcall function_1a474c(long stage);
void function_215810(void);
void __stdcall function_2157e0(long stage);
void function_533e0(void);
void __stdcall function_53310(long stage);
void function_b3db0(void);
void __stdcall function_b3d30(long stage);
void function_21ebe0(void);
void __stdcall function_21ebc0(long stage);
void animation_cache_memory_dispose(void);
void __stdcall animation_cache_memory_initialize(long stage);
void function_12dad0(void);
void __stdcall function_12d9f0(long stage);

struct s_callback_pair
{
	void (*dispose)(void);
	void (__stdcall *initialize)(long stage);
};

s_callback_pair g_453c00[8] =
{
	{ function_81f80, function_81780 },
	{ function_1a479a, function_1a474c },
	{ function_215810, function_2157e0 },
	{ function_533e0, function_53310 },
	{ function_b3db0, function_b3d30 },
	{ function_21ebe0, function_21ebc0 },
	{ animation_cache_memory_dispose, animation_cache_memory_initialize },
	{ function_12dad0, function_12d9f0 },
};