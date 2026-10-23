#include "unknown_4cf790.h"
// @flags /O2 /Gr
/* TRANSPORT.CPP: the transport layer's startup and shutdown: XNet, Winsock and
   Xbox Live start when the Ethernet link comes up, and the network modules'
   registered transition functions follow them (lane D) */

#include "unknown_11c920.h"
#include <xtl.h>
#include <xonline.h>
#include <string.h>
#include "data_array.h"
#include "globals.h"

s_transport_globals g_transport_globals;

/* the last Ethernet link status seen */
DWORD g_55e704;

/* the XNet key registry (unknown_07a9a0.cpp) */
struct s_xnet_registry_entry
{
	bool valid;
	byte unknown1[7];
	XNKID kid;
	XNKEY key;
};



/* the QoS pool (globals.cpp) */



/* the transport address cache and the security keys (unknown_07a9a0.cpp;
   0x7a840 is not decompiled yet: src/stubs/lane_d.cpp) */
void function_07a840(void);
void function_07b3e0(void);
void function_07b430(void);
bool function_07a9b0(void);
void function_07ae70(void);

void function_8d840(void);
void transport_shutdown(void);

/* function_16b790, which retail inlines here (unknown_16b570.cpp is /Ob1) */
static inline void transport_data_make_valid(s_record_pool *data)
{
	data->valid = true;
	record_pool_release_all(data);
}

static inline bool transport_link_up(void)
{
	DWORD status = XNetGetEthernetLinkStatus();
	if (g_55e704 != status)
		g_55e704 = status;
	return (status & XNET_ETHERNET_LINK_ACTIVE) != 0;
}

// @retail 0x8d690
void function_8d690(void)
{
	memset(&g_transport_globals, 0, sizeof(g_transport_globals));
	function_07a840();
	function_07b3e0();
	g_transport_globals.initialized = true;
	if (transport_link_up())
		function_8d840();
}

// @retail 0x8d6d0
void transport_dispose(void)
{
	if (g_transport_globals.initialized)
	{
		transport_shutdown();
		function_07b430();
		if (g_4cf8d8)
		{
			data_dispose(g_4cf8d8);
			g_4cf8d8 = 0;
		}
		g_transport_globals.transition_function_count = 0;
		g_transport_globals.initialized = false;
	}
}

// @retail 0x8d730
void transport_reset(void)
{
	for (long i = 0; i < g_transport_globals.transition_function_count; i++)
	{
		if (g_transport_globals.reset_functions[i])
			g_transport_globals.reset_functions[i](g_transport_globals.contexts[i]);
	}
}

// @retail 0x8d770
void transport_register_transition_functions(transport_transition_function startup, transport_transition_function shutdown, transport_transition_function reset, void *context)
{
	g_transport_globals.field_8_6[g_transport_globals.transition_function_count] = startup;
	g_transport_globals.shutdown_functions[g_transport_globals.transition_function_count] = shutdown;
	g_transport_globals.reset_functions[g_transport_globals.transition_function_count] = reset;
	g_transport_globals.contexts[g_transport_globals.transition_function_count] = context;
	g_transport_globals.transition_function_count++;
}

// @retail 0x8d7e0
void transport_update(void)
{
	bool link_up = g_transport_globals.initialized;
	if (link_up)
	{
		if (!transport_link_up())
			link_up = false;
		if (g_transport_globals.link_up != link_up)
		{
			g_transport_globals.link_up = link_up;
			if (link_up)
				function_8d840();
			else
				transport_shutdown();
		}
		if (g_transport_globals.link_up && !g_4cf791)
			function_07a9b0();
	}
}

// @retail 0x8d840
void function_8d840(void)
{
	if (!g_transport_globals.started)
	{
		WSADATA wsa_data = { 0 };
		XNetStartupParams parameters = { 0 };
		parameters.cfgSizeOfStruct = sizeof(parameters);
		parameters.cfgFlags = 0;
		parameters.cfgPrivatePoolSizeInPages = 12;
		parameters.cfgEnetReceiveQueueLength = 8;
		parameters.cfgIpFragMaxSimultaneous = 4;
		parameters.cfgIpFragMaxPacketDiv256 = 8;
		parameters.cfgSockMaxSockets = 24;
		parameters.cfgSockDefaultRecvBufsizeInK = 8;
		parameters.cfgSockDefaultSendBufsizeInK = 8;
		parameters.cfgKeyRegMax = 8;
		parameters.cfgSecRegMax = 40;
		parameters.cfgQosDataLimitDiv4 = 0xff;
		XNetGetEthernetLinkStatus();
		if (XNetStartup(&parameters) == 0)
		{
			if (WSAStartup(MAKEWORD(2, 0), &wsa_data) == 0)
			{
				/* XONLINE_STARTUP_PARAMS is empty; retail passes a zeroed dword */
				DWORD online_parameters = 0;
				if (XOnlineStartup((XONLINE_STARTUP_PARAMS *)&online_parameters) == 0)
				{
					g_transport_globals.started = true;
					function_07ae70();
					function_07a9b0();
					if (!g_4cf8d4)
					{
						transport_data_make_valid(g_4cf8d8);
						g_4cf8d4 = true;
					}
					for (long i = 0; i < g_transport_globals.transition_function_count; i++)
					{
						if (g_transport_globals.field_8_6[i])
							g_transport_globals.field_8_6[i](g_transport_globals.contexts[i]);
					}
				}
				else
				{
					WSACleanup();
					XNetCleanup();
				}
			}
			else
			{
				XNetCleanup();
			}
		}
	}
}

// @retail 0x8d970
void transport_shutdown(void)
{
	if (g_transport_globals.started)
	{
		for (long i = 0; i < g_transport_globals.transition_function_count; i++)
		{
			if (g_transport_globals.shutdown_functions[i])
				g_transport_globals.shutdown_functions[i](g_transport_globals.contexts[i]);
		}
		function_07b430();
		for (long key = 0; key < 8; key++)
		{
			s_xnet_registry_entry *entry = &g_4cf7d4[key];
			if (entry->valid)
			{
				XNetUnregisterKey(&entry->kid);
				entry->valid = false;
			}
		}
		g_transport_globals.started = false;
		XOnlineCleanup();
		WSACleanup();
		XNetCleanup();
	}
}

void __stdcall function_07a9a0(long unused);

// @retail 0x7a840
void function_07a840(void)
{
    memset(&g_4cf790, 0, 0x144);
    transport_register_transition_functions(0,
        (transport_transition_function)function_07a9a0,
        (transport_transition_function)function_07a9a0, 0);
    g_4cf790 = 1;
}
