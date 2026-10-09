#include "unknown_11c920.h"
#include "globals.h"
#include <string.h>

// @flags /O2 /Ob1 /arch:SSE /Gr

void function_210e20(void);
void function_210f00(void);

// @retail 0x210df0
void function_210df0(void)
{
    memset(g_4f93a4, 0xff, 0x40);
    function_210f00();
    function_210e20();
}
