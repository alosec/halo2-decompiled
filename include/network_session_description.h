/* NETWORK_SESSION_DESCRIPTION.H: the description of a session that a host
   publishes (0x714 bytes), which search replies and QoS probe data carry,
   and its bit stream encoding (src/network_session_description.cpp). The
   host fills one in 0x64d70. include/unknown_058ee0.h declares a partial
   view of the same layout under the same name. */

#ifndef NETWORK_SESSION_DESCRIPTION_H
#define NETWORK_SESSION_DESCRIPTION_H

#include "unknown_11c920.h"
#include "unknown_07f720.h"
#include <stddef.h>
#include <xtl.h>

struct s_bitstream;

enum
{
	k_session_description_maximum_players = 16,
	k_session_description_maximum_teams = 8
};

/* the first 12 bytes of a player's record, which identify the player */
struct s_session_player_identifier
{
	word words[6];
};

struct s_session_description
{
	short version;
	short unlisted;          /* the online search lists only descriptions with 0 */
	long protocol;
	long build;              /* NONE or 1..0xfffe */
	long compatible_build;   /* NONE or 1..0xfffe */
	short type;
	short local;
	short mode;              /* 0 public, 1 private, 2 private and closed */
	byte unknown16[2];
	wchar_t name[32];
	XNKID session_id;
	XNKEY key;
	XNADDR host_address;
	short public_open;
	short private_open;
	short public_used;
	short private_used;
	short variant;
	short state;
	long game_type;
	long map;
	long option;             /* NONE or 0..3 */
	long map_variant;
	bool teams;
	byte unknownb1[3];
	long scoring_kind;
	long score_limit;
	short player_count;
	s_session_player_identifier player_id_list[k_session_description_maximum_players];
	wchar_t player_names[k_session_description_maximum_players][32];
	byte unknown57e[2];
	long player_scores[k_session_description_maximum_players];
	short player_teams[k_session_description_maximum_players];
	s_player_appearance player_appearances[k_session_description_maximum_players];
	dword team_mask;         /* the teams whose scores follow */
	long team_scores[k_session_description_maximum_teams];
	bool optional_data_present;
	byte optional_data[12];  /* sent only when the flag is set; the host leaves both zero */
};

typedef char assert_session_player_identifier_size[(sizeof(s_session_player_identifier) == 12) ? 1 : -1];
typedef char assert_session_description_size[(sizeof(s_session_description) == 0x714) ? 1 : -1];
typedef char assert_session_description_layout[(
	offsetof(s_session_description, name) == 0x18 &&
	offsetof(s_session_description, session_id) == 0x58 &&
	offsetof(s_session_description, key) == 0x60 &&
	offsetof(s_session_description, host_address) == 0x70 &&
	offsetof(s_session_description, public_open) == 0x94 &&
	offsetof(s_session_description, game_type) == 0xa0 &&
	offsetof(s_session_description, teams) == 0xb0 &&
	offsetof(s_session_description, scoring_kind) == 0xb4 &&
	offsetof(s_session_description, player_count) == 0xbc &&
	offsetof(s_session_description, player_id_list) == 0xbe &&
	offsetof(s_session_description, player_names) == 0x17e &&
	offsetof(s_session_description, player_scores) == 0x580 &&
	offsetof(s_session_description, player_teams) == 0x5c0 &&
	offsetof(s_session_description, player_appearances) == 0x5e0 &&
	offsetof(s_session_description, team_mask) == 0x6e0 &&
	offsetof(s_session_description, team_scores) == 0x6e4 &&
	offsetof(s_session_description, optional_data_present) == 0x704 &&
	offsetof(s_session_description, optional_data) == 0x705) ? 1 : -1];

bool session_description_valid(const s_session_description *description);
void __stdcall session_description_encode(s_bitstream *stream, void *description);
bool __stdcall session_description_decode(s_bitstream *stream, void *description);
bool session_description_decode_buffer(long size, const byte *data, void *description);

#endif
