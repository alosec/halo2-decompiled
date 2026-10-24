// @flags /O2 /Ob1 /Gr
/* NETWORK_QOS.CPP: QoS (quality of service) probes of other machines and of
   the service, and their results. Each probe is a handle into the
   "transport qos attempts" data array. */

#include "unknown_11c920.h"
#include "globals.h"
#include "data_array.h"
#include "network_qos.h"
#include <xtl.h>
#include <string.h>

/* QoS probe handles: a data array of 8-byte elements {salt, state, XNQOS *}
   kept in g_4cf8d8 (the transport's "transport qos attempts"); g_4cf8d4 says
   whether the transport made it valid */
struct s_qos_handle
{
	short salt;
	short state;
	XNQOS *qos;
};

/* one machine to probe */
struct s_qos_target
{
	XNKID kid;
	XNKEY key;
	XNADDR xna;
};

/* by lookup kind: the probes to send to each target, and the bandwidth to
   use when the caller passes NONE */
static const long g_qos_lookup_probe_counts[2] = { 8, 0 };
static const long g_qos_lookup_bits_per_second[2] = { 16384, 4096 };

/* stand-in: retail reloads g_4cf8d8 after record_pool_allocate because some function not yet decompiled
   (probably the QoS initializer) takes its address; remove this once that function exists */
PRIVATE s_record_pool **qos_handle_array(void)
{
	return &g_4cf8d8;
}

/* retail inlines this into all four callers; with __inline alone, LTCG stopped
   inlining it once lane A round 3's unknown_272b70 code joined the program */
static __forceinline s_qos_handle *qos_handle_get(long handle)
{
	s_record_pool *data = g_4cf8d8;
	long index = handle & 0xffff;

	if (index < data->high_water_index)
	{
		s_qos_handle *element = (s_qos_handle *)(data->data + data->size * index);

		if (element->salt && element->salt == (handle >> 16))
		{
			return element;
		}
	}

	return 0;
}

/* starts probing up to 64 machines; returns the probe's handle or NONE */
// @retail 0x7b4c0
long qos_lookup(long kind, long count, long bits_per_second, s_qos_target *targets)
{
	long handle = NONE;

	if (g_4cf8d4)
	{
		long probes = g_qos_lookup_probe_counts[kind];
		long bandwidth = bits_per_second;
		XNQOS *qos;
		const XNKEY *keys[64];
		const XNKID *kids[64];
		const XNADDR *addresses[64];

		if (bits_per_second == NONE)
		{
			bandwidth = g_qos_lookup_bits_per_second[kind];
		}

		if (count > 64)
		{
			count = 64;
		}

		for (long i = 0; i < count; i++)
		{
			kids[i] = &targets[i].kid;
			keys[i] = &targets[i].key;
			addresses[i] = &targets[i].xna;
		}

		if (XNetQosLookup(count, addresses, kids, keys, 0, 0, 0, probes, bandwidth, 0, 0, &qos) == 0)
		{
			handle = record_pool_allocate(g_4cf8d8);
			if (handle != NONE)
			{
				s_qos_handle *element = &((s_qos_handle *)g_4cf8d8->data)[handle & 0xffff];

				element->state = 1;
				element->qos = qos;
			}
			else
			{
				XNetQosRelease(qos);
			}
		}
	}

	return handle;
}

/* starts probing the service; returns the probe's handle or NONE */
// @retail 0x7b5e0
long qos_service_lookup(void)
{
	long handle = NONE;

	if (g_4cf8d4)
	{
		XNQOS *qos;

		if (XNetQosServiceLookup(0, 0, &qos) == 0)
		{
			handle = record_pool_allocate(g_4cf8d8);
			if (handle != NONE)
			{
				s_qos_handle *element = &((s_qos_handle *)g_4cf8d8->data)[handle & 0xffff];

				element->state = 1;
				element->qos = qos;
			}
			else
			{
				XNetQosRelease(qos);
			}
		}
	}

	return handle;
}

/* releases a probe and its handle */
// @retail 0x7b650
void qos_release(long handle)
{
	if (g_4cf8d4 && handle != NONE)
	{
		s_qos_handle *element = qos_handle_get(handle);

		if (element)
		{
			if (XNetQosRelease(element->qos) == 0)
			{
				element->qos = 0;
			}

			record_pool_release(g_4cf8d8, handle);
		}
	}
}

/* whether no target of a probe is still pending (inline: 0xb3200 inlines
   it, its other callers call it) */
// @retail 0x7b6c0
inline bool qos_is_complete(long handle)
{
	bool result = true;

	if (g_4cf8d4 && handle != NONE)
	{
		s_qos_handle *element = qos_handle_get(handle);

		if (element)
		{
			result = element->qos->cxnqosPending == 0;
		}
	}

	return result;
}

/* one target's progress: 0 nothing sent, 1 probing, 2 partial, 3 complete
   or unknown, 4 disabled, 5 contacted */
// @retail 0x7b720
long qos_target_status(long handle, long index)
{
	long result = 3;

	if (g_4cf8d4 && handle != NONE)
	{
		s_qos_handle *element = qos_handle_get(handle);

		if (element)
		{
			XNQOSINFO *info = &element->qos->axnqosinfo[index];

			if (info->bFlags & XNET_XNQOSINFO_COMPLETE)
			{
				if (info->bFlags & XNET_XNQOSINFO_TARGET_DISABLED)
				{
					result = 4;
				}
				else
				{
					result = (info->bFlags & XNET_XNQOSINFO_TARGET_CONTACTED) ? 5 : 3;
				}
			}
			else if (info->bFlags & XNET_XNQOSINFO_PARTIAL_COMPLETE)
			{
				result = 2;
			}
			else
			{
				result = info->cProbesXmit > 0;
			}
		}
	}

	return result;
}

/* copies a contacted target's measurements and the data it returned */
// @retail 0x7b7b0
bool qos_target_result(long handle, s_qos_result *result, long index)
{
	bool success = false;

	if (g_4cf8d4 && handle != NONE)
	{
		s_qos_handle *element = qos_handle_get(handle);

		if (element)
		{
			XNQOSINFO *info = &element->qos->axnqosinfo[index];

			if ((info->bFlags & XNET_XNQOSINFO_TARGET_CONTACTED) && (info->bFlags & (XNET_XNQOSINFO_COMPLETE | XNET_XNQOSINFO_PARTIAL_COMPLETE)))
			{
				memset(result, 0, sizeof(*result));

				result->probes_sent = info->cProbesXmit;
				result->probes_received = info->cProbesRecv;
				result->rtt_minimum = info->wRttMinInMsecs;
				result->rtt_median = info->wRttMedInMsecs;
				result->upstream_bits_per_second = info->dwUpBitsPerSec;
				result->downstream_bits_per_second = info->dwDnBitsPerSec;
				if (info->bFlags & XNET_XNQOSINFO_DATA_RECEIVED)
				{
					result->data_size = info->cbData;
					result->data = info->pbData;
				}
				success = true;
			}
		}
	}

	return success;
}
