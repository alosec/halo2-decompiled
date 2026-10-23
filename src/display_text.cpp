// @flags /O2 /arch:SSE /Gr
/* DISPLAY_TEXT.CPP: the fade-in of the text 0x13cf00 shows */

#include "unknown_11c920.h"
#include "display_text.h"
#include <math.h>

/* the text's opacity: rising as (age / 1.5) ^ 1.9 over the first 1.5, then 1 */
// @retail 0x13cbb0
real display_text_get_alpha(s_display_text const *text)
{
	real alpha = 1.0f;
	if (1.5f >= text->age)
	{
		alpha = (real)pow(text->age * 0.6666667f, 1.9f);
	}
	return alpha;
}
