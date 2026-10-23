/* INPUT_RECORD.H: the record of the input state of one tick and its packed
   update (unknown_1967d0.cpp), and the input globals that share its views */

#ifndef INPUT_RECORD_H
#define INPUT_RECORD_H

#include "unknown_11c920.h"
#include "globals.h"

/* the input counters' definitions (0x46ddc0, unknown_1967d0.cpp): a name, the
   range of the counter's value and the bits the codecs send it in. The
   groups' first counters and the counters use entries 0..44, the groups'
   entries 45..51, the pairs 52..53 and the groups' second counters 54..85 */
struct s_counter_bits
{
	char const *name;
	dword unknown04;
	word minimum;
	word maximum;
	long bits;
};

extern s_counter_bits g_46ddc0[86];

struct s_input_entry
{
	byte active;
	byte unknown01;
	short value;
	byte unknown04[0x14];
};

/* eleven bytes: a six byte address, a used flag and four more */
struct s_input_address
{
	byte data[11];
};

/* the record of the input state of one tick (0x4cc0 bytes) */
struct s_input_record
{
	byte flag0;
	byte flag1;
	byte unknown02[2];
	dword value4;
	byte flag8;
	byte unknown09[3];
	dword valuec;
	s_input_device_view devices[4][4];
	s_input_entry entries[4][4];
	s_input_counter groups[16][0x1b5];
	s_input_counter pairs[16][16][2];
	s_input_counter counters[16][45];
	s_input_address addresses[4][4];
};

/* the packed form the record is merged from */
struct s_device_update
{
	byte changed;
	byte full;
	byte unknown02[2];
	s_input_device_view view;
};

struct s_entry_update
{
	byte changed;
	byte full;
	byte unknown02[2];
	s_input_entry entry;
};

struct s_counters_update
{
	byte flag;
	byte unknown01;
	s_input_counter counters[45];
};

struct s_counter_group_update
{
	byte flag0;
	byte unknown01;
	s_input_counter first[45];
	byte flag1;
	byte unknown5d;
	s_input_counter second[32];
	struct
	{
		byte flag;
		byte unknown01;
		s_input_counter counters[7];
	} entries[45];
};

struct s_pair_update
{
	byte flag;
	byte unknown01;
	s_input_counter counters[2];
};

struct s_address_update
{
	byte flag;
	s_input_address address;
};

struct s_input_update
{
	byte flag0;
	byte unknown01[3];
	dword value4;
	byte flag8;
	byte unknown09[3];
	dword valuec;
	s_device_update devices[4][4];
	s_entry_update entries[4][4];
	s_counter_group_update groups[16];
	s_pair_update pairs[16][16];
	s_counters_update counters[16];
	s_address_update addresses[4][4];
};

/* a flag and the value it guards (g_511020 and g_511028, defined in
   unknown_1967d0.cpp) */
struct s_flagged_value
{
	byte flag;
	byte unknown01[3];
	dword value;
};

/* 0x511020 and 0x511028 lie inside g_511000, before its entries */
#define g_511020 (*(s_flagged_value *)(g_511000.unknown00 + 0x20))
#define g_511028 (*(s_flagged_value *)(g_511000.unknown00 + 0x28))

extern byte g_510ca1;
extern s_input_entry g_511a74[16];

#endif
