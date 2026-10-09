// @flags /O1 /Oi /Gr
/* UNKNOWN_18F576.CPP: the local player slots (g_54e8e0, four slots of 0xc70
   bytes): the profile each slot holds, and queries over the slots */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_18f576.h"
#include "screen_widgets.h"
#include "unknown_24b5bc.h"
#include "loop_allocator.h"
#include <xtl.h>
#include <xonline.h>
#include <string.h>
#include <new>

/* the profile a slot holds (0x1e0 bytes at +0x18) */
struct s_player_profile
{
	dword data[0x78];
};

/* the identity at +0x4e0 (0x6a2 bytes; set when the 64 bit id is not zero) */
#pragma pack(push, 2)
struct s_player_identity
{
	unsigned __int64 id;
	byte data[0x6a2 - 8];
};
#pragma pack(pop)

/* the head of a slot (0x470 bytes) */
struct s_player_slot_data
{
	union
	{
		dword flags;
		struct
		{
			dword flag0 : 1;
			dword flag1 : 1;
			dword flag2 : 1;
			dword flag3 : 1;
			dword flag4 : 1;
			dword flag5 : 1;
			dword flag6 : 1;
		};
	};
	long controller_id;
	XUID xuid;
	byte unknown14[4];
	s_player_profile profile;
	union
	{
		long profile_index;
		struct
		{
			dword : 21;
			dword profile_flag21 : 1;
		};
	};
	long value1fc;
	byte value200;
	byte unknown201[3];
	byte value204;
	byte unknown205[3];
	s_player_slot_profile sign_in_profile;
	byte unknown46c[4];
};

/* a view of g_54e8e0's slots: the head, the online user at +0x470 (whose
   flags 1900a5 reads), then the identity */
struct s_player_slot_view : s_player_slot_data, XONLINE_USER
{
	s_player_slot_view();
	s_player_identity identity;
	s_player_slot_blockb82 blockb82;
	long valuec14;
	long valuec18;
	word name[32];
	byte unknownc5c[0xc70 - 0xc5c];

};
#if defined(H2_NATIVE_PORT)
static_assert(sizeof(s_player_slot_view) == 0xc70, "player slot layout");
#endif
#define k_player_slot_count 4

extern dword g_4e61cc[k_player_slot_count];

long function_190262(long value);
bool function_1900a5(long player);

static inline s_player_slot_view *player_slots(void)
{
	return (s_player_slot_view *)g_54e8e0;
}

static inline long player_slot_next(long index)
{
	long next = NONE;

	if (index >= 0 && index < k_player_slot_count - 1)
	{
		next = index + 1;
	}
	return next;
}

/* player_slot_get.cpp: retail built it without LTCG, so its callers see
   the standard convention (it may clobber edx) */
s_player_slot_view *player_slot_get(long index);

// @retail 0x18f564
s_player_slot_view::s_player_slot_view()
{
}

// @retail 0x18f548
s_player_slot_view *__stdcall function_18f548(s_player_slot_view *slots)
{
	__assume(slots != 0);
	return new (slots) s_player_slot_view[k_player_slot_count];
}

// @retail 0x18f8b8
bool controller_is_connected(short index)
{
	// Retail keeps the controller argument on the stack.
	short const *index_reference = &index;
	return g_4e61cc[*index_reference] != 0;
}

// @retail 0x18f8ce
long player_slot_find_controller(long controller_id)
{
	long result = NONE;

	if (controller_id != NONE)
	{
		long index = 0;
		do
		{
			if (TEST_FIELD_BIT(player_slots()[index].flag4) && player_slots()[index].controller_id == controller_id)
			{
				result = index;
				break;
			}
			index = player_slot_next(index);
		} while (index != NONE);
	}
	return result;
}

// @retail 0x18f911
short player_slot_count_active(void)
{
	short count = 0;
	long index = 0;

	do
	{
		if (TEST_FIELD_BIT(player_slots()[index].flag4))
		{
			count++;
		}
		index = function_190262(index);
	} while (index != NONE);
	return count;
}

// @retail 0x18f93a
short function_18f93a(void)
{
	short count = 0;
	long index = 0;

	do
	{
		if (TEST_FIELD_BIT(player_slots()[index].flag4) && !function_1900a5(index))
		{
			count++;
		}
		index = function_190262(index);
	} while (index != NONE);
	return count;
}

// @retail 0x18fa4d
long function_18fa4d(long mode)
{
	long result = NONE;
	long index = 0;

	do
	{
		bool excluded = false;
		if (mode == 1)
		{
			excluded = function_1900a5(index);
		}
		if (TEST_FIELD_BIT(player_slots()[index].flag4) && !excluded)
		{
			result = index;
			break;
		}
		index = function_190262(index);
	} while (index != NONE);
	return result;
}

// @retail 0x18fa94
long function_18fa94(XONLINE_USER *users)
{
	long result = NONE;
	long index = 0;

	do
	{
		if (users[index].xuid.qwUserID != 0 && users[index].szGamertag[0] && SUCCEEDED(users[index].hr) && !function_1900a5(index))
		{
			result = index;
			break;
		}
		index = function_190262(index);
	} while (index != NONE);
	return result;
}

// @retail 0x18fada
long player_slot_get_single_profile(void)
{
	long count;
	long result;
	long index;

	result = NONE;
	count = 0;
	index = 0;

	do
	{
		if (player_slots()[index].flags & 0x10)
		{
			count++;
			result = index;
		}
		index = function_190262(index);
	} while (index != NONE);

	return count == 1 ? result : NONE;
}

// @retail 0x18fb0d
long player_slot_get_single_profile_index(void)
{
	long result = NONE;
	long index = player_slot_get_single_profile();

	if (index != NONE)
	{
		s_player_slot_view *slot = &player_slots()[index];
		if (slot->flags & 0x10)
		{
			result = slot->profile_index;
		}
	}
	return result;
}

// @retail 0x18fc44
void player_slot_get_profile(long index, s_player_profile *profile, long *profile_index)
{
	s_player_slot_view *slot = player_slot_get(index);

	if (slot && (slot->flags & 0x10))
	{
		*profile = slot->profile;
		*profile_index = slot->profile_index;
	}
	else
	{
		memset(profile, 0, sizeof(*profile));
		*profile_index = NONE;
	}
}

// @retail 0x18fc75
bool player_slot_profile_in_use(long profile_index)
{
	bool result = false;

	for (long index = 0; !result && index != NONE; index = player_slot_next(index))
	{
		s_player_slot_view *slot = &player_slots()[index];
		if (slot->flags & 0x10)
		{
			result = !TEST_FIELD_BIT(slot->profile_flag21) && slot->profile_index == profile_index;
		}
	}
	return result;
}

// @retail 0x18fd7c
long player_slot_get_value1fc(long index)
{
	long result = NONE;
	s_player_slot_view *slot = &player_slots()[index];

	if (slot)
	{
		result = slot->value1fc;
	}
	return result;
}

// @retail 0x18ff47
void function_18ff47(long player, dword *out)
{
	s_player_slot_view *slot = &player_slots()[player];
	__assume(slot != NULL);
	*(XONLINE_USER *)out = *slot;
}

// @retail 0x18ff64
bool function_18ff64(long index)
{
	s_player_slot_view *slot = &player_slots()[index];

	return slot->valuec14 != NONE || slot->valuec18 != NONE;
}

// @retail 0x18ff88
bool player_slot_get_identity(long index, s_player_identity *identity)
{
	s_player_slot_view *slot = &player_slots()[index];
	bool valid = slot->identity.id != 0;

	if (valid)
	{
		*identity = slot->identity;
	}
	else
	{
		memset(identity, 0, sizeof(*identity));
	}
	return valid;
}

// @retail 0x18ffc3
bool function_18ffc3(long index, s_player_slot_blockb82 *block)
{
	s_player_slot_view *slot = &player_slots()[index];
	bool valid = slot->identity.id != 0;

	if (valid)
	{
		*block = slot->blockb82;
	}
	else
	{
		memset(block, 0, sizeof(*block));
	}
	return valid;
}

struct s_type_fb9815;
void machine_identifier_build(s_type_fb9815 *identifier, long index);
long __stdcall function_64610(dword *xuid);
void function_2172a0(long handle);
void function_190c34(long index);
void function_190eb3(long index);
void function_53810(long voice_mask, long controller_index);
void function_54fc0(long controller_index, long voice_through_tv);

struct s_55c164
{
	void *field0;
	byte unknown04[0x40];
};
extern long g_55c160;
extern s_55c164 g_55c164[16];

// @retail 0x18fb34
void __stdcall function_18fb34(long index, s_player_profile_settings *settings, long profile_index)
{
	s_player_slot_view *slot = &player_slots()[index];
#if defined(H2_NATIVE_PORT)
	/* Native offline activation owns no machine identity, voice graph, or
	   save-task queue. Preserve the local profile and the display-name effect
	   that retail function_190eb3 applies after installation. */
	slot->profile = *(s_player_profile *)settings;
	slot->flags |= 0x10;
	slot->profile_index = profile_index;
	memcpy(slot->name, settings->name, sizeof(slot->name));
#else
	bool changed = !(slot->flags & 0x10) || slot->profile_index != profile_index;

	if (!(slot->flags & 0x10))
	{
		machine_identifier_build((s_type_fb9815 *)&slot->s_player_slot_data::xuid, index);
		slot->controller_id = function_64610((dword *)&slot->s_player_slot_data::xuid);
		slot->profile_index = NONE;
	}
	if (changed && slot->profile_index != NONE)
	{
		function_2172a0(slot->profile_index);
	}
	slot->profile = *(s_player_profile *)settings;
	slot->flags |= 0x10;
	slot->profile_index = profile_index;
	function_190c34(index);
	function_190eb3(index);
	if (changed && slot->profile_index != NONE)
	{
		g_55c160++;
		g_55c164[g_55c160 - 1].field0 = (void *)slot->profile_index;
	}
	function_53810(settings->voice_mask, index);
	function_54fc0(index, settings->voice_through_tv);
#endif
}

void function_215790();
long function_2170e0(long file_index);
void online_mutelist_reset(long controller_index);
void function_190d4b(long index);
extern byte g_55c14f;
extern bool g_551ae4;

// @retail 0x18f5e3
void function_18f5e3()
{
	function_215790();
	long index = 0;
	do
	{
		s_player_slot_view *slot = &player_slots()[index];
		if (slot->flags & 0x10)
		{
			long *profile_index = &slot->profile_index;
			if (*profile_index != NONE)
#ifdef H2_NATIVE_PORT
				/* Native saved-profile enumeration is not available yet. */
				*profile_index = NONE;
#else
				*profile_index = function_2170e0(*profile_index);
#endif
		}
		index = function_190262(index);
	} while (index != NONE);
	g_55c14f = 0;
}

// @retail 0x18f58c
void function_18f58c()
{
	memset(g_54e8e0, 0, 0x3208);
	long index = 0;
	do
	{
		s_player_slot_view *slot = &player_slots()[index];
		slot->controller_id = NONE;
		slot->valuec14 = NONE;
		slot->valuec18 = NONE;
		online_mutelist_reset(index);
		function_190d4b(index);
		index = function_190262(index);
	} while (index != NONE);
	g_551ae4 = false;
}

// @retail 0x18f3ba
void *c_unknown_249fa3::get_item_data()
{
	return entries;
}

// @retail 0x18f3c1
void *c_unknown_249fa3::get_items(long *count)
{
	*count = 4;
	return profiles;
}

c_class_1473c9 *__stdcall function_18f42d(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_18f474(s_screen_parameters *parameters);

class c_profile_mode_screen : public c_screen_with_menu
{
public:
	virtual void v17();
	virtual screen_load_proc get_load_proc();
	c_unknown_249fa3 menu;
	bool mode;
};

// @retail 0x18f4f6
void c_profile_mode_screen::v17()
{
	menu.field88 = mode;
}

// @retail 0x18f503
screen_load_proc c_profile_mode_screen::get_load_proc()
{
	return mode ? function_18f474 : function_18f42d;
}

// @retail 0x18f533 destructor c_profile_mode_screen

// @retail 0x18f3f0
c_unknown_249fa3_base::~c_unknown_249fa3_base()
{
}

// @retail 0x18f517 deleting c_profile_mode_screen
