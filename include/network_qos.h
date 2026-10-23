/* NETWORK_QOS.H: the results of a QoS (quality of service) probe
   (src/network_qos.cpp) */

#ifndef NETWORK_QOS_H
#define NETWORK_QOS_H

#include "unknown_11c920.h"

struct s_qos_result
{
	long probes_sent;
	long probes_received;
	long rtt_minimum;
	long rtt_median;
	long upstream_bits_per_second;
	long downstream_bits_per_second;
	long data_size;
	byte *data;
};

bool qos_target_result(long handle, s_qos_result *result, long index);

#endif
