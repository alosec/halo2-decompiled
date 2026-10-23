// @flags /O2 /GL-
// Havok (third-party physics library): reading the processor time stamp
// counter, and measuring how fast it counts.

// @retail 0x2da0d0
unsigned __int64 __cdecl function_2da0d0()
{
	unsigned __int64 ticks;
	__asm
	{
		push ebx
		pop ebx
		rdtsc
		mov dword ptr ticks, eax
		mov dword ptr ticks + 4, edx
	}
	return ticks;
}

extern "C" int __stdcall QueryPerformanceCounter(__int64 *count);
extern "C" int __stdcall QueryPerformanceFrequency(__int64 *frequency);

// the time stamp counter's ticks per second, measured once (0 until then)
unsigned __int64 g_4801d8;

// a clock read through its table
class clock_2da0f0
{
public:
	virtual void slot_0() {}
	// the time stamp counter
	virtual unsigned __int64 slot_1() { return 0; }

	unsigned __int64 function_2da0f0();
};

// the clock's ticks per second, measured the first time: over ten tries of a short busy
// loop, the fewest clock ticks per performance counter tick, times the
// performance counter's frequency
// @retail 0x2da0f0
unsigned __int64 clock_2da0f0::function_2da0f0()
{
	if (g_4801d8 == 0)
	{
		double ratio = 1000000.0;
		for (int tries = 10; tries != 0; tries--)
		{
			unsigned __int64 const start_ticks = slot_1();
			__int64 start_count;
			QueryPerformanceCounter(&start_count);
			// the work timed (kept by volatile)
			volatile int busy = 1;
			for (int round = 5000; round != 0; round--)
			{
				busy = busy * busy + busy;
			}
			unsigned __int64 const end_ticks = slot_1();
			__int64 end_count;
			QueryPerformanceCounter(&end_count);
			QueryPerformanceFrequency((__int64 *)&g_4801d8);
			double const measured = (double)(end_ticks - start_ticks) / (double)(unsigned __int64)(end_count - start_count);
			if (measured < ratio)
			{
				ratio = measured;
			}
		}
		g_4801d8 = (unsigned __int64)((double)g_4801d8 * ratio);
	}
	return g_4801d8;
}
