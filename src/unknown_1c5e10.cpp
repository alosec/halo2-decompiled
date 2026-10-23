// @flags /O2 /Ob1 /Gr
/* UNKNOWN_1C5E10.CPP: packing settings into tagged records (0x1c59a0..0x1c62e0):
   each record starts with a 'b...' tag and ends with the matching 'e...'
   tag, and the records are byte packed */

#include "unknown_11c920.h"
#include "globals.h"
#include <string.h>

struct s_block40
{
	dword unknown[0x10];
};

struct s_vr_source
{
	long unknown00;
	s_block40 unknown04;
	long unknown44;
	byte unknown48[4];
	long unknown4c;
	long unknown50;
	long unknown54;
	byte unknown58[0x74 - 0x58];
	long unknown74;
	byte unknown78[0x80 - 0x78];
	long unknown80;
	byte unknown84[0xd4 - 0x84];
	byte unknownd4;
	byte unknownd5;
	byte unknownd6;
	byte unknownd7;
};

struct s_pap_source
{
	long unknown0;
	byte unknown4;
	byte unknown5;
	byte unknown6;
	byte unknown7;
};

struct s_con_source
{
	long unknown0;
	byte unknown4;
	byte unknown5;
	byte unknown6;
};

struct s_block10
{
	dword unknown[4];
};

struct s_block0c
{
	dword unknown[3];
};

struct s_block20
{
	dword unknown[8];
};

/* six shorts, at an odd place in s_pgd_source */
struct s_shorts6
{
	short unknown[6];
};

struct s_pcn_source
{
	s_block40 unknown00;
	s_pap_source unknown40;
	byte unknown48[8];
	s_block20 unknown50;
	s_block0c unknown70;
	byte unknown7c;
	byte unknown7d;
	byte unknown7e;
	byte unknown7f;
	byte unknown80;
	byte unknown81;
	byte unknown82[2];
	long unknown84;
	short unknown88;
	short unknown8a;
	long unknown8c;
};

struct s_pgd_source
{
	byte unknown00;
	byte unknown01;
	s_shorts6 unknown02;
	byte unknown0e[2];
	s_pcn_source unknown10;
	byte unknowna0;
	byte unknowna1;
	short unknowna2;
};

struct s_ppr_source
{
	byte unknown00[8];
	s_block40 unknown08;
	s_block10 unknown48;
	byte unknown58[0xec - 0x58];
	s_block10 unknownec;
	s_con_source unknownfc;
	byte unknown104[0x118 - 0x104];
	s_pap_source unknown118;
	byte unknown120[0x1e0 - 0x120];
};

struct s_block104
{
	dword unknown[0x41];
};

/* nine entries of 0x20 bytes, which the 'grs' writer copies whole, nine times */
struct s_block20x9
{
	s_block20 entries[9];
};

struct s_block8ca0
{
	dword unknown[0x2328];
};

#pragma pack(push, 1)

struct s_packed_vr
{
	dword begin;
	s_block40 unknown04;
	long unknown44;
	dword inner_begin;
	long unknown4c;
	long unknown50;
	long unknown54;
	long unknown58;
	long unknown5c;
	byte unknown60;
	byte unknown61;
	byte unknown62;
	dword inner_end;
	dword end;
};

struct s_packed_pap
{
	dword begin;
	long unknown4;
	byte unknown8;
	byte unknown9;
	byte unknowna;
	byte unknownb;
	dword end;
};

struct s_packed_con
{
	dword begin;
	long unknown4;
	byte unknown8;
	byte unknown9;
	byte unknowna;
	dword end;
};

struct s_packed_pcn
{
	dword begin;
	s_block40 unknown04;
	s_packed_pap unknown44;
	s_block20 unknown54;
	s_block0c unknown74;
	byte unknown80;
	byte unknown81;
	byte unknown82;
	byte unknown83;
	byte unknown84;
	byte unknown85;
	long unknown86;
	short unknown8a;
	short unknown8c;
	long unknown8e;
	dword end;
};

struct s_packed_pgd
{
	dword begin;
	byte unknown04;
	byte unknown05;
	s_shorts6 unknown06;
	s_packed_pcn unknown12;
	byte unknowna8;
	byte unknowna9;
	short unknownaa;
	dword end;
};

struct s_packed_ppr
{
	dword begin;
	s_block40 unknown04;
	s_block10 unknown44;
	s_block10 unknown54;
	s_packed_con unknown64;
	s_packed_pap unknown73;
	dword end;
};

struct s_packed_clc
{
	dword begin;
	long version;
	long size;
	long unknown0c;
	long unknown10;
	s_block104 unknown14;
	short unknown118;
	byte unknown11a;
	s_packed_ppr profiles[4];
	dword end;
};

#pragma pack(pop)

/* the empty records (the writers below with no source), which the record
   writers inline */
static inline void packed_con_clear(s_packed_con *packed)
{
	memset(packed, 0, sizeof(s_packed_con));
	packed->begin = 'bcon';
	packed->end = 'econ';
}

static inline void packed_pap_clear(s_packed_pap *packed)
{
	memset(packed, 0, sizeof(s_packed_pap));
	packed->begin = 'bpap';
	packed->end = 'epap';
}

// @retail 0x1c5e10
void packed_vr_write(s_vr_source const *source, s_packed_vr *packed)
{
	packed->begin = 'bgvr';
	packed->unknown04 = source->unknown04;
	packed->unknown44 = source->unknown44;
	packed->inner_begin = 'buvr';
	packed->unknown4c = source->unknown4c;
	packed->unknown50 = source->unknown50;
	packed->unknown54 = source->unknown54;
	packed->unknown58 = source->unknown74;
	packed->unknown5c = source->unknown80;
	packed->unknown60 = source->unknownd4;
	packed->unknown61 = source->unknownd6;
	packed->unknown62 = source->unknownd7;
	packed->inner_end = 'euvr';
	packed->end = 'egvr';
}

// @retail 0x1c5e80
void packed_pap_write(s_pap_source const *source, s_packed_pap *packed)
{
	if (!source)
	{
		memset(packed, 0, sizeof(s_packed_pap));
	}
	else
	{
		packed->unknown4 = source->unknown0;
		packed->unknown8 = source->unknown4;
		packed->unknown9 = source->unknown5;
		packed->unknowna = source->unknown6;
		packed->unknownb = source->unknown7;
	}
	packed->begin = 'bpap';
	packed->end = 'epap';
}

// @retail 0x1c61c0
void packed_con_write(s_con_source const *source, s_packed_con *packed)
{
	if (!source)
	{
		memset(packed, 0, sizeof(s_packed_con));
	}
	else
	{
		packed->unknown4 = source->unknown0;
		packed->unknown8 = source->unknown4;
		packed->unknown9 = source->unknown5;
		packed->unknowna = source->unknown6;
	}
	packed->begin = 'bcon';
	packed->end = 'econ';
}
// @retail 0x1c5ed0
void packed_pcn_write(s_pcn_source const *source, s_packed_pcn *packed)
{
	packed->begin = 'bpcn';
	packed->unknown04 = source->unknown00;
	packed_pap_write(&source->unknown40, &packed->unknown44);
	packed->unknown54 = source->unknown50;
	packed->unknown74 = source->unknown70;
	packed->unknown80 = source->unknown7c;
	packed->unknown81 = source->unknown7d;
	packed->unknown82 = source->unknown7e;
	packed->unknown83 = source->unknown7f;
	packed->unknown84 = source->unknown80;
	packed->unknown85 = source->unknown81;
	packed->unknown86 = source->unknown84;
	packed->unknown8a = source->unknown88;
	packed->unknown8c = source->unknown8a;
	packed->unknown8e = source->unknown8c;
	packed->end = 'epcn';
}

// @retail 0x1c5fa0
void packed_pgd_write(s_pgd_source const *source, s_packed_pgd *packed)
{
	packed->begin = 'bgpd';
	packed->unknown04 = source->unknown00;
	packed->unknown05 = source->unknown01;
	packed->unknown06 = source->unknown02;
	packed_pcn_write(&source->unknown10, &packed->unknown12);
	packed->unknowna8 = source->unknowna0;
	packed->unknowna9 = source->unknowna1;
	packed->unknownaa = source->unknowna2;
	packed->end = 'egpd';
}

// @retail 0x1c6210
void packed_ppr_write(s_ppr_source const *source, s_packed_ppr *packed)
{
	if (!source)
	{
		memset(packed, 0, sizeof(s_packed_ppr));
		packed_con_clear(&packed->unknown64);
		packed_pap_clear(&packed->unknown73);
	}
	else
	{
		packed->unknown04 = source->unknown08;
		packed->unknown44 = source->unknown48;
		packed->unknown54 = source->unknownec;
		packed_con_write(&source->unknownfc, &packed->unknown64);
		packed_pap_write(&source->unknown118, &packed->unknown73);
	}
	packed->begin = 'bppr';
	packed->end = 'eppr';
}
/* the local player profiles in the player slots (globals.h) */
struct s_player_slot_profile
{
	dword flags;
	byte unknown004[0x14];
	s_ppr_source profile;
	long unknown1f8;
};

struct s_clc_source
{
	byte unknown000[0x14];
	long unknown014;
	long unknown018;
	s_block104 unknown01c;
	byte unknown120[0x12a - 0x120];
	short unknown12a;
	byte unknown12c;
};

#define MAXIMUM_LOCAL_PROFILES 4

inline long local_profile_next(long index)
{
	long next = NONE;

	if (index >= 0 && index < MAXIMUM_LOCAL_PROFILES - 1)
	{
		next = index + 1;
	}
	return next;
}

static inline void packed_ppr_clear(s_packed_ppr *packed)
{
	memset(packed, 0, sizeof(s_packed_ppr));
	packed_con_clear(&packed->unknown64);
	packed_pap_clear(&packed->unknown73);
	packed->begin = 'bppr';
	packed->end = 'eppr';
}

inline s_player_slot_profile *local_profile_slot_get(long index)
{
	s_player_slot_profile *slot = NULL;

	if (index != NONE)
	{
		slot = (s_player_slot_profile *)&g_54e8e0[index];
	}
	return slot;
}

PRIVATE __forceinline long function_1c5ca1(s_player_slot_profile const *arg_0, s_ppr_source *arg_1)
{
    long local_0 = NONE;
    if (arg_0 && (arg_0->flags & 0x10))
    {
        *arg_1 = arg_0->profile;
        local_0 = arg_0->unknown1f8;
    }
    return local_0;
}

// @retail 0x1c5ca0
void packed_clc_write(s_clc_source const *source, s_packed_clc *packed)
{
	long index;

	memset(packed, 0xcd, sizeof(s_packed_clc));
	packed->begin = 'bclc';
	packed->version = 1;
	packed->size = 0x2651;
	packed->unknown0c = source->unknown014;
	packed->unknown10 = source->unknown018;
	packed->unknown14 = source->unknown01c;
	packed->unknown0c = source->unknown014;
	packed->unknown118 = source->unknown12a;
	packed->unknown11a = source->unknown12c;
	for (index = 0; index != NONE; index = local_profile_next(index))
	{
		s_player_slot_profile *slot = local_profile_slot_get(index);

		s_ppr_source profile;
		long local_0 = function_1c5ca1(slot, &profile);
		if (local_0 == NONE)
			packed_ppr_clear(&packed->profiles[index]);
		else
			packed_ppr_write(&profile, &packed->profiles[index]);
	}
	packed->end = 'eclc';
}

/* the 'sta' record: 16 'pst' records of text, then a table of pairs and 16
   names. Each text is copied with a fixed count of 45 characters, whatever
   room its source and destination have (as retail does). */
struct s_sta_text_source
{
	byte flag;
	byte unknown1;
	short text[7];
};

struct s_pst_source
{
	short unknown000[45];
	short unknown05a[32];
	s_sta_text_source texts[45];
};

struct s_sta_source
{
	s_pst_source entries[16];
	short pairs[16][16][2];
	short names[16][45];
};

#pragma pack(push, 1)

struct s_packed_sta_text
{
	byte flag;
	long count;
	short text[16];
};

struct s_packed_pst
{
	dword begin;
	long unknown04_count;
	short unknown04[64];
	long unknown88_count;
	short unknown88[48];
	long text_count;
	s_packed_sta_text texts[64];
	dword end;
};

struct s_packed_sta_pair
{
	long count;
	short values[2];
};

struct s_packed_sta_name
{
	long count;
	short text[64];
};

struct s_packed_sta
{
	dword begin;
	s_packed_pst entries[16];
	s_packed_sta_pair pairs[16][16];
	s_packed_sta_name names[16];
	dword end;
};

#pragma pack(pop)

/* a count, then that many shorts */
static inline void packed_shorts_write(long *packed_count, short *packed, short const *source, long count)
{
	long i;

	*packed_count = count;
	for (i = 0; i < count; i++)
	{
		packed[i] = source[i];
	}
}

// @retail 0x1c6010
void packed_sta_write(s_sta_source const *source, s_packed_sta *packed)
{
	long i;
	long j;

	packed->begin = 'bsta';
	for (i = 0; i < 16; i++)
	{
		packed->entries[i].begin = 'bpst';
		packed_shorts_write(&packed->entries[i].unknown04_count, packed->entries[i].unknown04, source->entries[i].unknown000, 45);
		packed_shorts_write(&packed->entries[i].unknown88_count, packed->entries[i].unknown88, source->entries[i].unknown05a, 32);
		packed->entries[i].text_count = 45;
		for (j = 0; j < 45; j++)
		{
			packed->entries[i].texts[j].flag = source->entries[i].texts[j].flag;
			packed_shorts_write(&packed->entries[i].texts[j].count, packed->entries[i].texts[j].text, source->entries[i].texts[j].text, 45);
		}
		packed->entries[i].end = 'epst';
	}
	for (i = 0; i < 16; i++)
	{
		for (j = 0; j < 16; j++)
		{
			packed_shorts_write(&packed->pairs[i][j].count, packed->pairs[i][j].values, source->pairs[i][j], 2);
		}
	}
	for (i = 0; i < 16; i++)
	{
		packed_shorts_write(&packed->names[i].count, packed->names[i].text, source->names[i], 45);
	}
	packed->end = 'esta';
}

/* the top record, 'grs' (0x150bc bytes, filled with 0xcd first) */

struct s_grs_entry_source
{
	byte unknown00;
	byte unknown01;
	short unknown02;
	short unknown04;
	byte unknown06[2];
	long unknown08;
	s_block0c unknown0c;
};

struct s_mac_address
{
	byte address[6];
};

struct s_grs_mac_source
{
	s_mac_address mac;
	byte unknown6;
	byte unknown7;
	byte unknown8;
	byte unknown9;
	byte unknowna;
};

struct s_grs_source
{
	byte unknown0000[4];
	long unknown0004;
	s_block20x9 unknown0008;
	byte unknown0128;
	byte unknown0129;
	byte unknown012a;
	byte unknown012b[5];
	long unknown0130;
	long unknown0134;
	s_vr_source unknown0138;
	byte unknown0210[0x268 - 0x210];
	long unknown0268;
	s_block104 unknown026c;
	byte unknown0370;
	byte unknown0371[3];
	long unknown0374;
	byte unknown0378;
	byte unknown0379[3];
	long unknown037c;
	byte unknown0380;
	byte unknown0381;
	byte unknown0382;
	byte unknown0383;
	s_pgd_source unknown0384[16];
	s_grs_entry_source unknown0dc4[16];
	s_sta_source unknown0f44;
	s_block8ca0 unknown4f84;
	s_grs_mac_source unknowndc24[16];
};

#pragma pack(push, 1)

struct s_packed_grs_entry
{
	byte unknown00;
	byte unknown01;
	short unknown02;
	short unknown04;
	long unknown06;
	s_block0c unknown0a;
};

struct s_packed_mac
{
	dword begin;
	s_mac_address mac;
	byte unknowna;
	byte unknownb;
	byte unknownc;
	byte unknownd;
	byte unknowne;
	dword end;
};

struct s_packed_grs
{
	dword begin;
	long version;
	long size;
	long unknown000c;
	s_block20x9 unknown0010;
	byte unknown0130;
	byte unknown0131;
	byte unknown0132;
	byte unknown0133;
	long unknown0134;
	long unknown0138;
	s_packed_vr unknown013c;
	long unknown01a7;
	s_block104 unknown01ab;
	byte unknown02af;
	long unknown02b0;
	byte unknown02b4;
	long unknown02b5;
	byte unknown02b9;
	byte unknown02ba;
	byte unknown02bb;
	s_packed_pgd unknown02bc[16];
	s_packed_grs_entry unknown0dbc[16];
	s_packed_sta unknown0f1c;
	dword eve_begin;
	s_block8ca0 unknownc2a8;
	dword eve_end;
	s_packed_con unknown14f4c[4];
	s_packed_mac unknown14f88[16];
	dword end;
};

#pragma pack(pop)

// @retail 0x1c59a0
void packed_grs_write(s_grs_source const *source, s_packed_grs *packed)
{
	long i;
	long index;

	memset(packed, 0xcd, sizeof(s_packed_grs));
	packed->begin = 'bgrs';
	packed->version = 6;
	packed->size = 0x2651;
	packed->unknown02b5 = source->unknown037c;
	packed->unknown000c = source->unknown0004;
	i = 0;
	do
	{
		packed->unknown0010 = source->unknown0008;
		i++;
	}
	while (i < 9);
	packed->unknown0130 = source->unknown0128;
	packed->unknown0131 = source->unknown0129;
	packed->unknown0132 = source->unknown012a;
	packed->unknown0134 = source->unknown0130;
	packed->unknown0138 = source->unknown0134;
	packed_vr_write(&source->unknown0138, &packed->unknown013c);
	packed->unknown01a7 = source->unknown0268;
	packed->unknown01ab = source->unknown026c;
	packed->unknown02af = source->unknown0370;
	packed->unknown02b0 = source->unknown0374;
	packed->unknown02b4 = source->unknown0378;
	packed->unknown02b5 = source->unknown037c;
	packed->unknown02b9 = source->unknown0380;
	packed->unknown02ba = source->unknown0381;
	packed->unknown02bb = source->unknown0382;
	for (i = 0; i < 16; i++)
	{
		packed_pgd_write(&source->unknown0384[i], &packed->unknown02bc[i]);
	}
	for (i = 0; i < 16; i++)
	{
		s_packed_grs_entry *packed_entry = &packed->unknown0dbc[i];
		s_grs_entry_source const *entry = &source->unknown0dc4[i];

		packed_entry->unknown00 = entry->unknown00;
		packed_entry->unknown01 = entry->unknown01;
		packed_entry->unknown02 = entry->unknown02;
		packed_entry->unknown04 = entry->unknown04;
		packed_entry->unknown06 = entry->unknown08;
		packed_entry->unknown0a = entry->unknown0c;
	}
	packed_sta_write(&source->unknown0f44, &packed->unknown0f1c);
	packed->eve_begin = 'beve';
	packed->unknownc2a8 = source->unknown4f84;
	packed->eve_end = 'eeve';
	for (index = 0; index != NONE; index = local_profile_next(index))
	{
		s_player_slot_profile *slot = local_profile_slot_get(index);
		s_ppr_source profile;
		s_packed_con *packed_con;

		if (slot && (slot->flags & 0x10))
		{
			profile = slot->profile;
		}
		else
		{
			memset(&profile, 0, sizeof(profile));
		}
		packed_con = &packed->unknown14f4c[index];
		packed_con->unknown4 = profile.unknownfc.unknown0;
		packed_con->unknown8 = profile.unknownfc.unknown4;
		packed_con->unknown9 = profile.unknownfc.unknown5;
		packed_con->unknowna = profile.unknownfc.unknown6;
		packed_con->begin = 'bcon';
		packed_con->end = 'econ';
	}
	for (i = 0; i < 16; i++)
	{
		s_grs_mac_source const *mac_source;
		s_packed_mac *mac;

		packed->unknown14f88[i].begin = 'bmac';
		mac = &packed->unknown14f88[i];
		mac_source = &source->unknowndc24[i];
		mac->mac = mac_source->mac;
		mac->unknowna = mac_source->unknown6;
		mac->unknownb = mac_source->unknown7;
		mac->unknownc = mac_source->unknown8;
		mac->unknownd = mac_source->unknown9;
		mac->unknowne = mac_source->unknowna;
		mac->end = 'emac';
	}
	packed->end = 'egrs';
}
