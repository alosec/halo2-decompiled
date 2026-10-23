#pragma once

/* the network globals block at 0x4d8ea8: set by the network initialize,
   cleared by the network dispose; the system link globals (0x4d8eb4) sit
   at +0xc. The including file defines s_system_link_globals first. */
struct s_network_link_globals
{
	bool initialized;
	byte unknown01[3];
	void *link;
	void *gateway;
	s_system_link_globals system_link;
	long unknown1c;
	void *unknown20;
};
extern s_network_link_globals g_4d8ea8;
#define g_4d8eb4 g_4d8ea8.system_link
#define g_4d8ec4 g_4d8ea8.unknown1c
#define g_4d8ec8 g_4d8ea8.unknown20
