#include "unknown_4e8c38.h"
// @flags /O2 /Gr
/* UNKNOWN_1552E0.CPP: the lifecycle callbacks of entry 35 */

#include "unknown_11c920.h"
#include "unknown_123b30.h"
#include "globals.h"
#include <string.h>

struct s_unknown_78
{
	byte unknown00[0x78];
};

s_storage_4e8c38 g_storage_4e8c38;
s_unknown_78 *g_510c6c;

// @retail 0x1552e0
void function_1552e0(void)
{
	memset(g_4e8c38, 0, sizeof(g_4e8c38));
	byte *flag = (byte *)function_123d40("unknown", "unknown", 4);
	*flag = 0;
	g_4e8c34 = flag;
	g_510c6c = (s_unknown_78 *)function_123d40("unknown", "unknown", sizeof(s_unknown_78));
}

// @retail 0x155490
void function_155490(void)
{
	memset(g_4e8c38, 0, sizeof(g_4e8c38));
	*g_4e8c34 = 0;
}
