// @flags /O1 /Oi /Gr
/* Native split of D3D-independent text drawing state from
   unknown_13e8a0.cpp. The Xbox build keeps the original bodies there. */

#include "unknown_11c920.h"
#include "globals.h"
#include <string.h>

#ifdef H2_NATIVE_PORT

// @retail 0x13eb20
void function_13eb20(short count, short const *tab_stops)
{
	g_4e73a0.tab_stop_count = count > 16 ? 16 : count;
	if (g_4e73a0.tab_stop_count > 0)
		memcpy(g_4e73a0.tab_stops, tab_stops, g_4e73a0.tab_stop_count * sizeof(short));
}

// @retail 0x13eb60
void function_13eb60(color4f const *color)
{
	bool valid =
		color->alpha >= 0.0f && 1.0f >= color->alpha &&
		color->red >= 0.0f && 1.0f >= color->red &&
		color->green >= 0.0f && 1.0f >= color->green &&
		color->blue >= 0.0f && 1.0f >= color->blue;

	g_4e73a0.color = *color;
	if (!valid)
	{
		if (0.0f > g_4e73a0.color.alpha)
			g_4e73a0.color.alpha = 0.0f;
		else if (g_4e73a0.color.alpha > 1.0f)
			g_4e73a0.color.alpha = 1.0f;
		if (0.0f > g_4e73a0.color.red)
			g_4e73a0.color.red = 0.0f;
		else if (g_4e73a0.color.red > 1.0f)
			g_4e73a0.color.red = 1.0f;
		if (0.0f > g_4e73a0.color.green)
			g_4e73a0.color.green = 0.0f;
		else if (g_4e73a0.color.green > 1.0f)
			g_4e73a0.color.green = 1.0f;
		if (0.0f > g_4e73a0.color.blue)
			g_4e73a0.color.blue = 0.0f;
		else if (g_4e73a0.color.blue > 1.0f)
			g_4e73a0.color.blue = 1.0f;
	}
}

// @retail 0x13ec70
void function_13ec70(color4f const *color)
{
	color4f c = *color;
	bool bright = true;
	bright &= c.red > 0.68f;
	bright &= c.green > 0.68f;
	bright &= c.blue > 0.68f;
	if (bright)
	{
		real minimum = c.green > c.blue ? c.blue : c.green;
		minimum = c.red > minimum ? (c.green > c.blue ? c.blue : c.green) : c.red;
		if (0.68f > minimum)
			minimum = 0.68f;
		else if (minimum > 1.0f)
			minimum = 1.0f;
		real scale = 0.68f / minimum;
		c.red *= scale;
		c.green *= scale;
		c.blue *= scale;
	}
	function_13eb60(&c);
}

// @retail 0x13ed50
void function_13ed50(color4f const *shadow)
{
	if (shadow)
	{
		g_4e73a0.shadow = true;
		g_4e73a0.field_24 = *shadow;
	}
	else
	{
		g_4e73a0.shadow = false;
	}
}

#endif
