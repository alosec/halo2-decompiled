/* SLOT_OWNER.H: the elements (0x888 bytes) of the actor slot-owner data array
   g_4f55f0, as seen by actor_behavior.cpp (the slots and their entries),
   unknown_1f8640.cpp (the owner state) and unknown_26e370.cpp (the joint
   invitations). Only the fields they touch are
   named. */

#ifndef SLOT_OWNER_H
#define SLOT_OWNER_H

#include "unknown_11c920.h"

struct s_slot
{
	short type;
	short state;
	short unknown4;
	byte unknown6[2];
	long time;
	byte unknownc[0x34];
};

struct s_slot_entry
{
	short type;
	short field2;
	short field4;
	short field6;
	short field8;
	short priority;
};

/* an invitation to join a joint behavior (unknown_26e370.cpp) */
struct s_joint_invitation
{
	short type;
	short participant_index;
	long joint_index;
	long expiration_time;
};

struct s_slot_owner_entry
{
	byte unknown0[0x7c];
	long joint_index;
	byte unknown80[4];
	short unknown84;
	byte unknown86[0xa];
	s_slot slots[4];
	short current;
	byte unknown192[2];
	s_joint_invitation joint_invitations[4];
	s_slot_entry entries[3];
	byte unknown1e8[0x4ac - 0x1e8];
	short unknown4ac;
	byte unknown4ae[0x56];
	short unknown504;
	byte unknown506[2];
	short unknown508;
	byte unknown50a[2];
	byte unknown50c;
	byte unknown50d[0x9f];
	long unknown5ac;
	short unknown5b0;
	byte unknown5b2[2];
	short unknown5b4;
	short unknown5b6;
	byte unknown5b8[0x2d0];
};

#endif
