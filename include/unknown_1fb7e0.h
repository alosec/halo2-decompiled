#ifndef __UNKNOWN_1FB7E0_H__
#define __UNKNOWN_1FB7E0_H__

/* what an actor's unit says or does when function_1fb7e0 has no type
   (0x14 bytes, copied into the event function_1fbac0 takes) */
struct s_1fb7e0_data
{
	long unknown00;
	long unknown04;
	long unknown08;
	long unknown0c;
	long unknown10;
};

struct s_1fbac0_event
{
	short unknown00;
	short unknown02;
	s_1fb7e0_data data;
};

/* has the unit of an actor start an event of the given type (or, with no
   type, the event in data); false when the actor has no unit */
bool __stdcall function_20ba60(short type, long unit_index, long target_index, long unknown, short unknown2, s_1fb7e0_data const *data);
bool function_1fb7e0(short type, long actor_index, s_1fb7e0_data const *data, long target_index, long unknown);

#endif
