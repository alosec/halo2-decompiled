#include "unknown_4cf790.h"
// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_07F190.CPP: the trimmed mean of a set of samples (lane D: called
   by the session's member latency summary and the observer) */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_075870.h"
#include "unknown_0662e0.h"
#include "unknown_0259d0.h"
#include <xtl.h>
#include <string.h>

/* the fraction of the samples dropped at each end */


struct s_network_observer;
void network_observer_reset_bandwidth(s_network_observer *observer);












/* sorts the samples, drops the given fraction of them at each end and returns
   the mean of the rest */
// @retail 0x7f190
long samples_trimmed_mean(const long *samples, long count)
{
	long sorted[128];
	long trim = real_truncate(count * *g_4cf8e8);
	long used;
	long total;
	long i;

	if (trim > 0)
	{
		for (used = 0; used < count; used++)
		{
			for (long j = used; j >= 0; j--)
			{
				if (j <= 0 || sorted[j - 1] <= samples[used])
				{
					sorted[j] = samples[used];
					break;
				}
				sorted[j] = sorted[j - 1];
			}
		}
		used -= trim * 2;
		memmove(sorted, &sorted[trim], used * sizeof(long));
	}
	else
	{
		memcpy(sorted, samples, count * sizeof(long));
		used = count;
	}
	total = 0;
	for (i = 0; i < used; i++)
		total += sorted[i];
	return total / used;
}

// @retail 0x7f020
bool function_7f020(s_network_observer *observer, real *trim)
{
	g_4cf8e4 = observer;
	g_4cf95c = false;
	g_4cf964 = false;
	g_4cf8ec = false;
	g_4cf8e8 = trim;
	g_4cf968 = NONE;
	g_4cf96c = 16;
	memset(g_4cf8f0, 0, sizeof(g_4cf8f0));
	g_4cf8e0 = true;
	return true;
}

// @retail 0x7f260
void function_7f260(void)
{
	if (g_4cf8e0)
	{
		network_observer_reset_bandwidth(g_4cf8e4);
		g_4cf95d = true;
		if (g_510548)
			g_4cf960 = g_51054c;
		else
			g_4cf960 = GetTickCount();
	}
}

long g_51099c[27];
void function_7a740(s_network_observer *observer);

struct s_estimate_configuration
{
	real trim;
	long minimum_samples;
	long reserve;
	long minimum;
	long unknown10;
	long unknown14;
	long threshold;
	long unknown1c;
	long step;
};

extern bool g_510819;
void function_7f410(void);

struct s_bandwidth_sample
{
	byte unknown00[0x10];
	long value;
};

// @retail 0x7f0d0
void __stdcall function_7f0d0(const byte *data)
{
	const s_bandwidth_sample *sample = (const s_bandwidth_sample *)data;
	if (!g_4cf8ec)
	{
		memcpy(g_4cf8f0, g_51099c, sizeof(g_4cf8f0));
		g_4cf8ec = true;
		function_7f410();
	}
	if (g_4cf8f0[0] >= 8)
	{
		g_4cf8f0[0] = 7;
		memmove(g_4cf8f0 + 1, g_4cf8f0 + 2, 7 * sizeof(long));
	}
	g_4cf8f0[1 + g_4cf8f0[0]] = sample->value;
	g_4cf8f0[0]++;
	function_7f410();
	if (g_4cf964 && sample->value > g_4cf968 + ((s_estimate_configuration *)g_4cf8e8)->unknown1c)
	{
		g_4cf8f0[26]++;
		function_7f410();
	}
	memcpy(g_51099c, g_4cf8f0, sizeof(g_4cf8f0));
	g_510819 = true;
}

bool network_observer_get_bandwidth(s_network_observer *observer, long *sent, real *ratio, long *received);

// @retail 0x7f2a0
void function_7f2a0(void)
{
	if (g_4cf95d)
	{
		long sent;
		long received;
		real ratio;
		if (network_observer_get_bandwidth(g_4cf8e4, &sent, &ratio, &received))
		{
			long last = g_4cf960;
			long now = g_510548 ? g_51054c : GetTickCount();
			bool elapsed = now - last >= ((s_estimate_configuration *)g_4cf8e8)->unknown10;
			bool acceptable = ratio < *(real *)&((s_estimate_configuration *)g_4cf8e8)->unknown14;
			if (!g_4cf8ec)
			{
				memcpy(g_4cf8f0, g_51099c, sizeof(g_4cf8f0));
				g_4cf8ec = true;
				function_7f410();
			}
			if (elapsed && (acceptable || (sent >= g_4cf968 && received >= g_4cf968)))
			{
				if (g_4cf8f0[9] >= 8)
				{
					g_4cf8f0[9] = 7;
					memmove(g_4cf8f0 + 10, g_4cf8f0 + 11, 7 * sizeof(long));
					memmove(g_4cf8f0 + 18, g_4cf8f0 + 19, g_4cf8f0[9] * sizeof(long));
				}
				g_4cf8f0[10 + g_4cf8f0[9]] = sent;
				g_4cf8f0[18 + g_4cf8f0[9]] = received;
				g_4cf8f0[9]++;
				g_4cf8f0[26] = 0;
				memcpy(g_51099c, g_4cf8f0, sizeof(g_4cf8f0));
				g_510819 = true;
				function_7f410();
			}
		}
		network_observer_reset_bandwidth(g_4cf8e4);
		g_4cf95d = false;
	}
}

// @retail 0x7f070
void function_7f070(void)
{
	if (g_4cf95c && g_4e6948 && g_4e6948->flag1120)
	{
		long mode = g_4e6948->mode;
		if (mode >= 4 && mode <= 5)
		{
			switch (mode)
			{
			case 2:
			case 4:
				break;
			default:
				if (!g_4cf95d)
					function_7f260();
				return;
			}
		}
	}
	if (g_4cf95d)
		function_7f2a0();
}

/* stores a bandwidth estimate in the observer, pinned between the observer's
   minimum and 0x100000 (0x100000 and reliable when there is no estimate),
   and restarts its bandwidth measurement */
static inline void observer_pin_rate(s_network_observer *observer, long bandwidth, bool reliable)
{
	if (bandwidth == NONE)
	{
		observer->value4e04 = 0x100000;
		observer->unknown4e01[0] = true;
	}
	else
	{
		long minimum = *(long *)((byte *)observer->configuration + 0x114);
		observer->value4e04 = bandwidth < minimum ? minimum : (bandwidth > 0x100000 ? 0x100000 : bandwidth);
		observer->unknown4e01[0] = reliable;
	}
	network_observer_reset_bandwidth(observer);
	function_7a740(observer);
}

// @retail 0x7f410
void function_7f410(void)
{
	long estimate = NONE;
	long level = 16;
	bool measured = false;
	if (!g_4cf8ec)
	{
		memcpy(g_4cf8f0, g_51099c, sizeof(g_4cf8f0));
		g_4cf8ec = true;
		function_7f410();
	}
	if (g_4cf95c)
	{
		long sent = 0;
		long received = 0;
		long single = 0;
		if (g_4cf8f0[9] >= ((s_estimate_configuration *)g_4cf8e8)->minimum_samples)
		{
			sent = samples_trimmed_mean(g_4cf8f0 + 10, g_4cf8f0[9]);
			received = samples_trimmed_mean(g_4cf8f0 + 18, g_4cf8f0[9]);
		}
		if (g_4cf8f0[0] > 0)
			single = samples_trimmed_mean(g_4cf8f0 + 1, g_4cf8f0[0]);
		if (sent > 0)
		{
			estimate = sent <= received - ((s_estimate_configuration *)g_4cf8e8)->reserve ?
				sent : received - ((s_estimate_configuration *)g_4cf8e8)->reserve;
			long minimum = ((s_estimate_configuration *)g_4cf8e8)->minimum;
			if (estimate <= minimum)
				estimate = minimum;
			measured = true;
			if (single > estimate && g_4cf8f0[26] > ((s_estimate_configuration *)g_4cf8e8)->threshold)
			{
				long grown = (g_4cf8f0[26] - ((s_estimate_configuration *)g_4cf8e8)->threshold) * ((s_estimate_configuration *)g_4cf8e8)->step + estimate;
				if (grown > single) grown = single;
				estimate = grown;
			}
		}
		else if (single > 0)
		{
			long minimum = ((s_estimate_configuration *)g_4cf8e8)->minimum;
			estimate = single > minimum ? single : minimum;
		}
		if (estimate != NONE)
		{
			level = 0;
			for (long i = 16; i > 0; i--)
			{
				if (estimate >= g_network_configuration.value40[i])
				{
					level = i;
					break;
				}
			}
		}
	}
	g_4cf968 = estimate;
	g_4cf964 = measured;
	g_4cf96c = level;
	observer_pin_rate(g_4cf8e4, estimate, measured);
}
