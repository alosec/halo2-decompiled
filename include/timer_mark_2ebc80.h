#ifndef TIMER_MARK_2EBC80_H
#define TIMER_MARK_2EBC80_H
// Havok (third-party physics library): marks added to a timing stream that
// shares the scratch buffer's cursor and limit (include/scratch_2d88a0.h).

extern char *g_480154; // cursor
extern char *g_480158; // limit

// one mark: a label and the low half of the time stamp counter
struct timer_mark_2ebc80
{
	char const *label;
	unsigned long time;
	long field_8;
};

__forceinline void timer_add_2ebc80(char const *label)
{
	if (g_480154 < g_480158)
	{
		timer_mark_2ebc80 *mark = (timer_mark_2ebc80 *)g_480154;
		mark->label = label;
		unsigned long time;
#if H2_NATIVE_PORT
		/* profiling marks only need increasing values */
		static unsigned long h2_timer_mark_counter;
		time = ++h2_timer_mark_counter;
#else
		__asm
		{
			rdtsc
			mov time, eax
		}
#endif
		mark->time = time;
		g_480154 = (char *)(mark + 1);
	}
}

#endif
