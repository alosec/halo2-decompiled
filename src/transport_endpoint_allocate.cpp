// @flags /O2 /arch:SSE /Gr
/* TRANSPORT_ENDPOINT_ALLOCATE.CPP: allocates a transport endpoint
   (include/unknown_0b4da0.h), with no socket yet, while the transport is up */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0b4da0.h"
#include <xtl.h>

typedef char assert_transport_endpoint_size[(sizeof(s_transport_endpoint) == 8) ? 1 : -1];

PRIVATE s_transport_endpoint *transport_endpoint_initialize(s_transport_endpoint *endpoint, word type)
{
	if (endpoint)
	{
		endpoint->socket = NONE;
		endpoint->flags = 0;
		endpoint->type = type;
	}

	return endpoint;
}

PRIVATE s_transport_endpoint *transport_endpoint_allocate()
{
	s_transport_endpoint *endpoint = (s_transport_endpoint *)VirtualAlloc(0, sizeof(s_transport_endpoint), MEM_COMMIT | MEM_TOP_DOWN, PAGE_READWRITE);

	if (!endpoint)
	{
		GetLastError();
	}

	return endpoint;
}

/* a new endpoint of a type, or none when the transport is not started or
   the allocation fails */
// @retail 0xb4d50
s_transport_endpoint *transport_endpoint_allocate(word type)
{
	s_transport_endpoint *endpoint = 0;

	if (g_transport_globals.initialized && g_transport_globals.started)
	{
		endpoint = transport_endpoint_allocate();
		endpoint = transport_endpoint_initialize(endpoint, type);
	}

	return endpoint;
}
