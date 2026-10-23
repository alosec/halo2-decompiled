// @flags /O2 /arch:SSE /Gr
/* GAME_ENGINE_ENTITY_DEFINITIONS.CPP: the simulation entity definitions of the
   game engine globals (slayer, ctf, oddball, king, territories and juggernaut,
   vtables 0x450fa0, 0x451058, 0x451188, 0x451240, 0x451318 and 0x451410) and
   of the game engine statborg (vtable 0x4516a8).

   The entity definitions have 27 slots. Slots shared with other vtables
   (identical code the linker folded) are owned by the class listed first;
   slots that no decompiled function owns keep placeholder bodies. */

#include "unknown_11c920.h"
#include "globals.h"
#include "engine_peer.h"
#include "object_types_21_1.h"
#include "bitstream.h"
#include "flags_writer.h"
#include "game_engine_globals_update.h"
#include "entity_relevance.h"
#include <string.h>

bool game_engine_globals_read_update(c_game_engine_entity_definition const *definition, s_game_engine_globals_update *update, s_bitstream *stream, dword *read);

void game_engine_globals_describe_update(c_game_engine_entity_definition const *definition, dword const *flags,
	unsigned long size, char *buffer);

#define MIN(a, b) ((a) < (b) ? (a) : (b))

class c_game_engine_entity_definition;
long function_a4950(c_game_engine_entity_definition const *definition, dword *flags_pointer);
void __stdcall function_1e98e0(void *statborg, long b, void *data);
bool __stdcall function_1e9ad0(void *statborg, long b, void *data);

/* the multiplayer globals, as the statborg sees them */
struct s_statborg_globals_view
{
	byte unknown000[0x304];
	byte statborg[1];
};

class c_game_engine_entity_definition
{
public:
	virtual long v0() { return 0; }
	virtual const char *v1() { return 0; }
	virtual long v2() { return 0; }
	virtual long v3() { return 0; }
	virtual long v4() { return 0; }
	virtual long v5() { return 0; }
	virtual bool v6(long a) { return false; }
	virtual bool v7(long a) { return false; }
	virtual bool v8(long a, long b) { return false; }
	virtual void v9(long a, long b, long *size) {}
	virtual void v10(s_creation_request *request, long parameter, long size, char *buffer);
	virtual void v11(long a, dword *flags, long *size) {}
	virtual void v12(long a, void const *data, long c, s_bitstream *stream) {}
	virtual bool v13(long a, void *data, s_bitstream *stream) { return false; }
	virtual bool v14(long a1, long a2, long a3, long a4, long a5, long a6, long a7, long a8) { return false; }
	virtual bool v15(long a, dword *flags, long c, void *data, s_bitstream *stream) { return false; }
	virtual void v16(long a, long b, long c) {}
	virtual bool v17(long a, long b, long c) { return false; }
	virtual void v18(s_entity_slot *entity, long b, void *state) {}
	virtual bool v19(long a, long b, long c, void *data) { return false; }
	virtual bool v20(s_entity_slot *entity, long b, long c, void *data) { return false; }
	virtual void v21(s_entity_slot *entity) {}
	virtual bool v22(s_entity_slot *entity, long b, long c, long d, long e, long f);
	virtual bool v23(s_entity_slot *entity, long b, long c, void *data) { return false; }
	virtual bool v24(s_entity_slot *entity);
	virtual bool v25(s_entity_slot *entity) { return false; }
	virtual void v26(long a, dword *flags, long size, char *buffer) {}
};

/* slayer: also the oddball's slots 5, 11, 14 and 15 */
class c_slayer_globals_entity_definition : public c_game_engine_entity_definition
{
public:
	virtual long v5();
	virtual void v11(long a, dword *flags, long *size);
	virtual bool v14(long a1, long a2, long a3, long a4, long a5, long a6, long a7, long a8);
	virtual bool v15(long a, dword *flags, long c, void *data, s_bitstream *stream);
	virtual void v26(long a, dword *flags, long size, char *buffer);
};

class c_ctf_globals_entity_definition : public c_game_engine_entity_definition
{
public:
	virtual const char *v1();
	virtual long v2();
	virtual void v11(long a, dword *flags, long *size);
	virtual bool v14(long a1, long a2, long a3, long a4, long a5, long a6, long a7, long a8);
	virtual bool v15(long a, dword *flags, long c, void *data, s_bitstream *stream);
	virtual void v26(long a, dword *flags, long size, char *buffer);
};

class c_oddball_globals_entity_definition : public c_game_engine_entity_definition
{
public:
	virtual const char *v1();
	virtual void v26(long a, dword *flags, long size, char *buffer);
};

class c_king_globals_entity_definition : public c_game_engine_entity_definition
{
public:
	virtual const char *v1();
	virtual long v2();
	virtual long v5();
	virtual void v11(long a, dword *flags, long *size);
	virtual bool v14(long a1, long a2, long a3, long a4, long a5, long a6, long a7, long a8);
	virtual bool v15(long a, dword *flags, long c, void *data, s_bitstream *stream);
	virtual void v26(long a, dword *flags, long size, char *buffer);
};

class c_territories_globals_entity_definition : public c_game_engine_entity_definition
{
public:
	virtual const char *v1();
	virtual long v2();
	virtual long v5();
	virtual void v11(long a, dword *flags, long *size);
	virtual bool v14(long a1, long a2, long a3, long a4, long a5, long a6, long a7, long a8);
	virtual bool v15(long a, dword *flags, long c, void *data, s_bitstream *stream);
	virtual void v26(long a, dword *flags, long size, char *buffer);
};

class c_juggernaut_globals_entity_definition : public c_game_engine_entity_definition
{
public:
	virtual const char *v1();
	virtual long v2();
	virtual void v11(long a, dword *flags, long *size);
	virtual bool v14(long a1, long a2, long a3, long a4, long a5, long a6, long a7, long a8);
	virtual bool v15(long a, dword *flags, long c, void *data, s_bitstream *stream);
	virtual void v26(long a, dword *flags, long size, char *buffer);
};

class c_game_engine_statborg_entity_definition : public c_game_engine_entity_definition
{
public:
	virtual const char *v1();
	virtual long v2();
	virtual void v10(s_creation_request *request, long parameter, long size, char *buffer);
	virtual void v11(long a, dword *flags, long *size);
	virtual bool v14(long a1, long a2, long a3, long a4, long a5, long a6, long a7, long a8);
	virtual bool v15(long a, dword *flags, long c, void *data, s_bitstream *stream);
	virtual bool v19(long a, long b, long c, void *data);
	virtual bool v20(s_entity_slot *entity, long b, long c, void *data);
	virtual void v21(s_entity_slot *entity);
	virtual bool v22(s_entity_slot *entity, long b, long c, long d, long e, long f);
	virtual bool v23(s_entity_slot *entity, long b, long c, void *data);
	virtual bool v24(s_entity_slot *entity);
	virtual void v26(long a, dword *flags, long size, char *buffer);
};

c_game_engine_statborg_entity_definition g_4797cc;

/* the globals of each game engine: the fields they all have, then their own */
struct s_ctf_globals_update
{
	s_game_engine_globals_update globals;
	word unknown22;
	long defensive_team;
	dword flag_swap_timer;
	short flag_reset_timers[9];
	short flag_arming_timers[9];
	byte flag_weapon_flags[9];
	byte unknown59[3];
	long bomb_placers[9];
};

struct s_king_globals_update
{
	s_game_engine_globals_update globals;
	word unknown22;
	short hill_id;
	word players_in_hill;
};

struct s_territories_globals_update
{
	s_game_engine_globals_update globals;
	word unknown22;
	long controller_indices[8];
	char player_teams[16];
	byte player_counts[16];
};

struct s_juggernaut_globals_update
{
	s_game_engine_globals_update globals;
	word unknown22;
	word juggernaut_bitvector;
};

// ---- slayer ----

// @retail 0x9a140
long c_slayer_globals_entity_definition::v5()
{
	return 0x1f;
}

// @retail 0x9a150
void c_slayer_globals_entity_definition::v11(long a, dword *flags, long *size)
{
	*size = function_a4950(this, flags);
}

// @retail 0x9a200
bool c_slayer_globals_entity_definition::v14(long a1, long a2, long a3, long a4, long a5, long a6, long a7, long a8)
{
	return game_engine_globals_write_update(this, a8, a2, (dword *)a3, (s_game_engine_globals_update const *)a5, (s_bitstream *)a7);
}

// @retail 0x9a230
bool c_slayer_globals_entity_definition::v15(long a, dword *flags, long c, void *data, s_bitstream *stream)
{
	dword read = 0;
	bool result;
	if (game_engine_globals_read_update(this, (s_game_engine_globals_update *)data, stream, &read) && read)
		result = true;
	else
		result = false;
	*flags = read;
	return result;
}

// @retail 0x99830
void c_slayer_globals_entity_definition::v26(long a, dword *flags, long size, char *buffer)
{
	real relevance = 0.0f;
	long period = 0;
	char flags_string[1024];
	game_engine_globals_describe_update(this, flags, sizeof(flags_string), flags_string);
	function_abac0(&relevance, (s_creation_request const *)a, (s_update_state const *)flags, &period);
	function_11c9c0(buffer, size, "slayer update: %s relevance=%5.3f: period=%d", flags_string, relevance, period);
}

// ---- ctf ----

// @retail 0x998c0
const char *c_ctf_globals_entity_definition::v1()
{
	return "ctf-engine-globals";
}

// @retail 0x998d0
long c_ctf_globals_entity_definition::v2()
{
	return 0x84;
}

// @retail 0x998e0
void c_ctf_globals_entity_definition::v11(long a, dword *flags, long *size)
{
	dword update_flags = *flags;
	long result = function_a4950(this, flags) + 6;
	if (update_flags & 0x20)
		result = MIN(result, 0xf);
	if (update_flags & 0x40)
		result = MIN(result, 0x1b);
	if (update_flags & 0x80)
		result = MIN(result, 0x53);
	if (update_flags & 0x100)
		result = MIN(result, 0x5c);
	if (update_flags & 0x200)
		result = MIN(result, 0x2f);
	if (update_flags & 0x400)
		result = MIN(result, 0x38);
	*size = result;
}

// @retail 0x99960
void c_ctf_globals_entity_definition::v26(long a, dword *flags, long size, char *buffer)
{
	real relevance = 0.0f;
	long period = 0;
	char flags_string[1024];
	game_engine_globals_describe_update(this, flags, sizeof(flags_string), flags_string);
	function_abac0(&relevance, (s_creation_request const *)a, (s_update_state const *)flags, &period);
	function_11c9c0(buffer, size, "ctf update: %s relevance=%5.3f: period=%d", flags_string, relevance, period);
}

// @retail 0x999f0
bool c_ctf_globals_entity_definition::v14(long a1, long a2, long a3, long a4, long a5, long a6, long a7, long a8)
{
	s_ctf_globals_update const *update = (s_ctf_globals_update const *)a5;
	s_bitstream *stream = (s_bitstream *)a7;
	bool result = false;
	if (game_engine_globals_write_update(this, a8 + 6, a2 & 0x1f, (dword *)a3, &update->globals, stream))
	{
		s_flags_writer writer;
		flags_writer_initialize(&writer, stream, 5, 6, a2 & 0x7e0, a8);
		if (writer.space)
		{
			long k;
			if (flags_writer_begin(&writer, 5, "defensive-team-exists"))
				stream_write_checked(stream, update->defensive_team + 1, 4);
			flags_writer_end(&writer);
			if (flags_writer_begin(&writer, 6, "flag-swap-timer"))
				stream_write_checked(stream, update->flag_swap_timer, 16);
			flags_writer_end(&writer);
			if (flags_writer_begin(&writer, 7, "flag-reset-timers"))
			{
				for (k = 0; k < 9; k++)
				{
					stream_write_bit(stream, update->flag_reset_timers[k] != NONE);
					if (update->flag_reset_timers[k] != NONE)
						stream_write_checked(stream, update->flag_reset_timers[k], 8);
				}
			}
			flags_writer_end(&writer);
			if (flags_writer_begin(&writer, 8, "flag-arming-timers"))
			{
				for (k = 0; k < 9; k++)
				{
					stream_write_bit(stream, update->flag_arming_timers[k] != NONE);
					if (update->flag_arming_timers[k] != NONE)
						stream_write_checked(stream, update->flag_arming_timers[k], 9);
				}
			}
			flags_writer_end(&writer);
			if (flags_writer_begin(&writer, 9, "flag-weapon-flags"))
			{
				for (k = 0; k < 9; k++)
				{
					stream_write_bit(stream, update->flag_weapon_flags[k] != 0);
					if (update->flag_weapon_flags[k])
						stream_write_checked(stream, update->flag_weapon_flags[k], 4);
				}
			}
			flags_writer_end(&writer);
			if (flags_writer_begin(&writer, 10, "player-that-placed-bombs"))
			{
				for (k = 0; k < 9; k++)
					stream_write_checked(stream, update->bomb_placers[k] + 1, 5);
			}
			flags_writer_end(&writer);
			*(dword *)a3 |= writer.written;
			result = true;
		}
	}
	return result;
}

// @retail 0x99e10
bool c_ctf_globals_entity_definition::v15(long a, dword *flags, long c, void *data, s_bitstream *stream)
{
	s_ctf_globals_update *update = (s_ctf_globals_update *)data;
	dword read = 0;
	bool valid = game_engine_globals_read_update(this, &update->globals, stream, &read) != 0;
	long k;
	if (function_1957d0(stream))
	{
		update->defensive_team = function_1959c0(stream, 4) - 1;
		if (update->defensive_team != NONE)
		{
			if (valid && update->defensive_team >= 0 && update->defensive_team < 8)
				valid = true;
			else
				valid = false;
		}
		read |= 0x20;
	}
	if (function_1957d0(stream))
	{
		update->flag_swap_timer = function_1959c0(stream, 16);
		if (valid && (long)update->flag_swap_timer >= 0)
			valid = true;
		else
			valid = false;
		read |= 0x40;
	}
	if (function_1957d0(stream))
	{
		for (k = 0; k < 9; k++)
		{
			if (stream_read_bit(stream))
				update->flag_reset_timers[k] = (short)function_1959c0(stream, 8);
			else
				update->flag_reset_timers[k] = NONE;
		}
		read |= 0x80;
	}
	if (stream_read_bit(stream))
	{
		for (k = 0; k < 9; k++)
		{
			if (stream_read_bit(stream))
				update->flag_arming_timers[k] = (short)function_1959c0(stream, 9);
			else
				update->flag_arming_timers[k] = NONE;
		}
		read |= 0x100;
	}
	if (stream_read_bit(stream))
	{
		for (k = 0; k < 9; k++)
		{
			if (stream_read_bit(stream))
				update->flag_weapon_flags[k] = (byte)function_1959c0(stream, 4);
			else
				update->flag_weapon_flags[k] = 0;
		}
		read |= 0x200;
	}
	if (stream_read_bit(stream))
	{
		for (k = 0; k < 9; k++)
			update->bomb_placers[k] = function_1959c0(stream, 5) - 1;
		read |= 0x400;
	}
	bool result;
	if (valid && read)
		result = true;
	else
		result = false;
	*flags = read;
	return result;
}

// ---- oddball ----

// @retail 0x9a130
const char *c_oddball_globals_entity_definition::v1()
{
	return "oddball-engine-globals";
}

// @retail 0x9a170
void c_oddball_globals_entity_definition::v26(long a, dword *flags, long size, char *buffer)
{
	real relevance = 0.0f;
	long period = 0;
	char flags_string[1024];
	game_engine_globals_describe_update(this, flags, sizeof(flags_string), flags_string);
	function_abac0(&relevance, (s_creation_request const *)a, (s_update_state const *)flags, &period);
	function_11c9c0(buffer, size, "oddball update: %s relevance=%5.3f: period=%d", flags_string, relevance, period);
}

// ---- king ----

// @retail 0x9a290
const char *c_king_globals_entity_definition::v1()
{
	return "king-engine-globals";
}

// @retail 0x9a2a0
long c_king_globals_entity_definition::v2()
{
	return 0x28;
}

// @retail 0x9a2b0
long c_king_globals_entity_definition::v5()
{
	return 0x7f;
}

// @retail 0x9a2c0
void c_king_globals_entity_definition::v11(long a, dword *flags, long *size)
{
	dword update_flags = *flags;
	long result = function_a4950(this, flags) + 2;
	if (update_flags & 0x20)
		result = MIN(result, 0xb);
	if (update_flags & 0x40)
		result = MIN(result, 0x17);
	*size = result;
}

// @retail 0x9a3a0
bool c_king_globals_entity_definition::v14(long a1, long a2, long a3, long a4, long a5, long a6, long a7, long a8)
{
	s_king_globals_update const *update = (s_king_globals_update const *)a5;
	s_bitstream *stream = (s_bitstream *)a7;
	bool result = false;
	if (game_engine_globals_write_update(this, a8 + 2, a2 & 0x1f, (dword *)a3, &update->globals, stream))
	{
		s_flags_writer writer;
		flags_writer_initialize(&writer, stream, 5, 2, a2 & 0x60, a8);
		if (writer.space)
		{
			if (flags_writer_begin(&writer, 5, "hill-id-exists"))
				stream_write_checked(stream, update->hill_id + 1, 4);
			flags_writer_end(&writer);
			if (flags_writer_begin(&writer, 6, "players-in-hill-exists"))
				stream_write_checked(stream, update->players_in_hill, 16);
			flags_writer_end(&writer);
			*(dword *)a3 |= writer.written;
			result = true;
		}
	}
	return result;
}

// @retail 0x9a530
bool c_king_globals_entity_definition::v15(long a, dword *flags, long c, void *data, s_bitstream *stream)
{
	s_king_globals_update *update = (s_king_globals_update *)data;
	dword read = 0;
	bool valid = game_engine_globals_read_update(this, &update->globals, stream, &read) ? true : false;
	if (function_1957d0(stream))
	{
		update->hill_id = (short)(function_1959c0(stream, 4) - 1);
		read |= 0x20;
	}
	if (function_1957d0(stream))
	{
		update->players_in_hill = (word)function_1959c0(stream, 16);
		read |= 0x40;
	}
	bool result;
	if (valid && read)
		result = true;
	else
		result = false;
	*flags = read;
	return result;
}

// @retail 0x9a310
void c_king_globals_entity_definition::v26(long a, dword *flags, long size, char *buffer)
{
	real relevance = 0.0f;
	long period = 0;
	char flags_string[1024];
	game_engine_globals_describe_update(this, flags, sizeof(flags_string), flags_string);
	function_abac0(&relevance, (s_creation_request const *)a, (s_update_state const *)flags, &period);
	function_11c9c0(buffer, size, "king update: %s relevance=%5.3f: period=%d", flags_string, relevance, period);
}

// ---- territories ----

// @retail 0x9a5c0
const char *c_territories_globals_entity_definition::v1()
{
	return "territories-engine-globals";
}

// @retail 0x9a5d0
long c_territories_globals_entity_definition::v2()
{
	return 0x64;
}

// @retail 0x9a5f0
long c_territories_globals_entity_definition::v5()
{
	return 0x3fffff;
}

// @retail 0x9a600
void c_territories_globals_entity_definition::v11(long a, dword *flags, long *size)
{
	dword update_flags = *flags;
	long result = function_a4950(this, flags) + 0x11;
	if (update_flags & 0x20)
		result = MIN(result, 0x3e);
	for (long i = 0; i < 16; i++)
	{
		if (update_flags & (1 << (i + 6)))
			result = MIN(result, 0x20);
	}
	*size = result;
}

// @retail 0x9a6e0
bool c_territories_globals_entity_definition::v14(long a1, long a2, long a3, long a4, long a5, long a6, long a7, long a8)
{
	s_territories_globals_update const *update = (s_territories_globals_update const *)a5;
	s_bitstream *stream = (s_bitstream *)a7;
	bool result = false;
	if (game_engine_globals_write_update(this, a8 + 0x11, a2 & 0x1f, (dword *)a3, &update->globals, stream))
	{
		s_flags_writer writer;
		flags_writer_initialize(&writer, stream, 5, 0x11, a2 & 0x3fffe0, a8);
		if (writer.space)
		{
			if (flags_writer_begin(&writer, 5, "territory-controller-indices-exist"))
			{
				for (long k = 0; k < 8; k++)
					stream_write_checked(stream, update->controller_indices[k] + 1, 5);
			}
			flags_writer_end(&writer);
			for (long i = 0; i < 16; i++)
			{
				if (flags_writer_begin(&writer, i + 6, "territory-player-exist"))
				{
					stream_write_checked(stream, update->player_teams[i] + 1, 4);
					stream_write_checked(stream, update->player_counts[i], 6);
				}
				flags_writer_end(&writer);
			}
			*(dword *)a3 |= writer.written;
			result = true;
		}
	}
	return result;
}

// @retail 0x9a8d0
bool c_territories_globals_entity_definition::v15(long a, dword *flags, long c, void *data, s_bitstream *stream)
{
	s_territories_globals_update *update = (s_territories_globals_update *)data;
	dword read = 0;
	bool valid = game_engine_globals_read_update(this, &update->globals, stream, &read) != 0;
	if (function_1957d0(stream))
	{
		long k = 0;
		do
		{
			update->controller_indices[k] = function_1959c0(stream, 5) - 1;
			k++;
		}
		while (k < 8);
		read |= 0x20;
	}
	for (long i = 0; i < 16; i++)
	{
		if (stream_read_bit(stream))
		{
			update->player_teams[i] = (char)(function_1959c0(stream, 4) - 1);
			update->player_counts[i] = (byte)function_1959c0(stream, 6);
			read |= 1 << (i + 6);
		}
	}
	bool result;
	if (valid && read)
		result = true;
	else
		result = false;
	*flags = read;
	return result;
}

// @retail 0x9a650
void c_territories_globals_entity_definition::v26(long a, dword *flags, long size, char *buffer)
{
	real relevance = 0.0f;
	long period = 0;
	char flags_string[1024];
	game_engine_globals_describe_update(this, flags, sizeof(flags_string), flags_string);
	function_abac0(&relevance, (s_creation_request const *)a, (s_update_state const *)flags, &period);
	function_11c9c0(buffer, size, "territories update: %s relevance=%5.3f: period=%d", flags_string, relevance, period);
}

// ---- juggernaut ----

// @retail 0x9a9d0
const char *c_juggernaut_globals_entity_definition::v1()
{
	return "juggernaut-engine-globals";
}

// @retail 0x9a9e0
long c_juggernaut_globals_entity_definition::v2()
{
	return 0x26;
}

// @retail 0x9aa00
void c_juggernaut_globals_entity_definition::v11(long a, dword *flags, long *size)
{
	long result = function_a4950(this, flags) + 1;
	if (*flags & 0x20)
		result = MIN(result, 0x16);
	*size = result;
}

// @retail 0x9aad0
bool c_juggernaut_globals_entity_definition::v14(long a1, long a2, long a3, long a4, long a5, long a6, long a7, long a8)
{
	s_juggernaut_globals_update const *update = (s_juggernaut_globals_update const *)a5;
	s_bitstream *stream = (s_bitstream *)a7;
	bool result = false;
	if (game_engine_globals_write_update(this, a8 + 1, a2 & 0x1f, (dword *)a3, &update->globals, stream))
	{
		s_flags_writer writer;
		flags_writer_initialize(&writer, stream, 5, 1, a2 & 0x20, a8);
		if (writer.space)
		{
			if (flags_writer_begin(&writer, 5, "juggernaut-bitvector-exists"))
				stream_write_checked(stream, update->juggernaut_bitvector, 16);
			flags_writer_end(&writer);
			*(dword *)a3 |= writer.written;
			result = true;
		}
	}
	return result;
}

// @retail 0x9ac00
bool c_juggernaut_globals_entity_definition::v15(long a, dword *flags, long c, void *data, s_bitstream *stream)
{
	s_juggernaut_globals_update *update = (s_juggernaut_globals_update *)data;
	dword read = 0;
	bool valid = game_engine_globals_read_update(this, &update->globals, stream, &read) != 0;
	if (function_1957d0(stream))
	{
		update->juggernaut_bitvector = (word)function_1959c0(stream, 16);
		read |= 0x20;
	}
	bool result;
	if (valid && read)
		result = true;
	else
		result = false;
	*flags = read;
	return result;
}

// @retail 0x9aa40
void c_juggernaut_globals_entity_definition::v26(long a, dword *flags, long size, char *buffer)
{
	real relevance = 0.0f;
	long period = 0;
	char flags_string[1024];
	game_engine_globals_describe_update(this, flags, sizeof(flags_string), flags_string);
	function_abac0(&relevance, (s_creation_request const *)a, (s_update_state const *)flags, &period);
	function_11c9c0(buffer, size, "juggernaut update: %s relevance=%5.3f: period=%d", flags_string, relevance, period);
}

// ---- the game engine statborg ----

/* the statborg of the multiplayer globals, or none without a game engine */
static void *statborg_get()
{
	s_statborg_globals_view *globals = (s_statborg_globals_view *)g_4e9ae8;
	void *statborg = 0;
	if (g_55e4d0[g_4e9ae8->engine_index])
		statborg = globals->statborg;
	return statborg;
}

// @retail 0x9b920
const char *c_game_engine_statborg_entity_definition::v1()
{
	return "game-engine-statborg";
}

// @retail 0x9b930
long c_game_engine_statborg_entity_definition::v2()
{
	return 0x1b0;
}

// @retail 0x9b960
void c_game_engine_statborg_entity_definition::v10(s_creation_request *request, long parameter, long size, char *buffer)
{
	real relevance = -1.0f;
	s_creation_weight *entry = &g_4cef68[request->definition_index];
	if (!(entry->weight > g_45dbd8))
		relevance = function_aa4d0(1, &request->entity_index, entry->maximum_distance, (s_relevance_observers const *)parameter, 0);
	function_11c9c0(buffer, size, "statborg creation: relevance=%5.3f", relevance);
}

// @retail 0x9ba30
void c_game_engine_statborg_entity_definition::v26(long a, dword *flags, long size, char *buffer)
{
	real relevance = 0.0f;
	long period = 0;
	function_abac0(&relevance, (s_creation_request const *)a, (s_update_state const *)flags, &period);
	function_11c9c0(buffer, size, "statborg update: relevance=%5.3f:period=%d", relevance, period);
}

// @retail 0x9b9d0
void c_game_engine_statborg_entity_definition::v11(long a, dword *flags, long *size)
{
	dword update_flags = *flags;
	long result = 0x18;
	long i;
	for (i = 0; i < 16; i++)
	{
		if (update_flags & (1 << i))
			result = MIN(result, 0x12);
	}
	for (i = 0; i < 8; i++)
	{
		if (update_flags & (1 << (i + 16)))
			result = MIN(result, 0x12);
	}
	*size = result;
}

// @retail 0x9bb50
bool c_game_engine_statborg_entity_definition::v19(long a, long b, long c, void *data)
{
	memset(data, 0, 0x1b0);
	return true;
}

// @retail 0x9bb70
bool c_game_engine_statborg_entity_definition::v20(s_entity_slot *entity, long b, long c, void *data)
{
	function_1e98e0(statborg_get(), b, data);
	return true;
}

// @retail 0x9bb10
void c_game_engine_statborg_entity_definition::v21(s_entity_slot *entity)
{
	if (g_55e4d0[g_4e9ae8->engine_index] && g_4e9ae8->value28 != NONE)
	{
		if (((entity->id ^ g_4e9ae8->value28) & 0x3ff) == 0)
			g_4e9ae8->value28 = entity->id;
	}
}

// @retail 0x9ba90
bool c_game_engine_statborg_entity_definition::v22(s_entity_slot *entity, long b, long c, long d, long e, long f)
{
	s_mp_globals *globals = g_4e9ae8;
	long id = NONE;
	if (g_55e4d0[globals->engine_index])
		id = globals->value28;
	if (id != NONE)
		globals->value28 = NONE;
	globals->value28 = entity->id;
	return true;
}

// @retail 0x9bbb0
bool c_game_engine_statborg_entity_definition::v23(s_entity_slot *entity, long b, long c, void *data)
{
	bool result = false;
	if (function_1e9ad0(statborg_get(), b, data))
		result = true;
	return result;
}

// @retail 0x9bad0
bool c_game_engine_statborg_entity_definition::v24(s_entity_slot *entity)
{
	long id = NONE;
	if (g_55e4d0[g_4e9ae8->engine_index])
		id = g_4e9ae8->value28;
	long local_1 = entity->id;
	bool result = false;
	if (local_1 == id)
	{
		g_4e9ae8->value28 = NONE;
		result = true;
	}
	return result;
}

/* the statborg's statistics: nine per player and per team */
struct s_statborg_statistics
{
	short values[9];
};

struct s_statborg_data
{
	s_statborg_statistics players[16];
	s_statborg_statistics teams[8];
};

/* writes a signed value of the given size */
static inline void stream_write_signed(s_bitstream *stream, long value, long size)
{
	function_195720(stream, value & ((1 << size) - 1), size);
}

// @retail 0x9bbf0
bool c_game_engine_statborg_entity_definition::v14(long a1, long a2, long a3, long a4, long a5, long a6, long a7, long a8)
{
	s_statborg_data const *statistics = (s_statborg_data const *)a5;
	s_bitstream *stream = (s_bitstream *)a7;
	bool result = false;
	s_flags_writer writer;
	flags_writer_initialize(&writer, stream, 0, 0x18, a2, a8);
	if (writer.space)
	{
		long i;
		for (i = 0; i < 16; i++)
		{
			if (flags_writer_begin(&writer, i, "player-update-exists"))
			{
				for (long j = 0; j < 9; j++)
					stream_write_signed(stream, statistics->players[i].values[j], 16);
			}
			flags_writer_end(&writer);
		}
		for (i = 0; i < 8; i++)
		{
			if (flags_writer_begin(&writer, i + 16, "team-update-exists"))
			{
				for (long j = 0; j < 9; j++)
					stream_write_signed(stream, statistics->teams[i].values[j], 16);
			}
			flags_writer_end(&writer);
		}
		*(dword *)a3 |= writer.written;
		result = true;
	}
	return result;
}

/* reads a signed value of the given size */
static inline long stream_read_signed(s_bitstream *stream, long size)
{
	long value = function_1959c0(stream, size);
	if (value & (1 << (size - 1)))
		value |= ~((1 << size) - 1);
	return value;
}

// @retail 0x9bd40
bool c_game_engine_statborg_entity_definition::v15(long a, dword *flags, long c, void *data, s_bitstream *stream)
{
	s_statborg_data *statistics = (s_statborg_data *)data;
	bool result;
	dword mask = 0;
	long i;
	for (i = 0; i < 16; i++)
	{
		if (stream_read_bit(stream))
		{
			for (long j = 0; j < 9; j++)
				statistics->players[i].values[j] = (short)stream_read_signed(stream, 16);
			mask |= 1 << i;
		}
	}
	for (i = 0; i < 8; i++)
	{
		if (stream_read_bit(stream))
		{
			for (long j = 0; j < 9; j++)
				statistics->teams[i].values[j] = (short)stream_read_signed(stream, 16);
			mask |= 1 << (i + 16);
		}
	}
	if (mask)
		result = true;
	else
		result = false;
	*flags = mask;
	return result;
}

// @retail 0xa4800
void c_game_engine_entity_definition::v10(s_creation_request *request, long parameter, long size, char *buffer)
{
 real relevance = -1.0f;
 s_creation_weight *entry = &g_4cef68[request->definition_index];
 if (!(entry->weight > g_45dbd8))
  relevance = function_aa4d0(1, &request->entity_index, entry->maximum_distance, (s_relevance_observers const *)parameter, 0);
 function_11c9c0(buffer, size, "game engine global creation: relevance=%5.3f", relevance);
}


// @retail 0xa4880
bool c_game_engine_entity_definition::v22(s_entity_slot *entity, long b, long c, long d, long e, long f)
{
 c_engine_peer *manager = g_55e4d0[g_4e9ae8->engine_index];
 long identifier = NONE;
 if (manager)
  identifier = g_4e9ae8->value24;
 long type = NONE;
 if (manager)
  type = manager->get_current_id();
 bool result = false;
 if (*(short *)entity->unknown04 == type)
 {
  if (identifier != NONE)
   g_4e9ae8->value24 = NONE;
  g_4e9ae8->value24 = entity->id;
  if (d)
   g_55e4d0[g_4e9ae8->engine_index]->p46(d, e, f);
  result = true;
 }
 return result;
}

// @retail 0xa4910
bool c_game_engine_entity_definition::v24(s_entity_slot *entity)
{
 c_engine_peer *manager = g_55e4d0[g_4e9ae8->engine_index];
 long identifier = NONE;
 if (manager)
  identifier = g_4e9ae8->value24;
 bool result = false;
 if (entity->id == identifier)
 {
  g_4e9ae8->value24 = NONE;
  result = true;
 }
 return result;
}
