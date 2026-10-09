#ifndef H2_PORT_XDK_XONLINE_H
#define H2_PORT_XDK_XONLINE_H

#include <xtl.h>

/* Layouts below are independently recovered from retail field accesses and
 * record strides in the supported executable. They contain only fields the
 * open source currently names. */
typedef HANDLE XONLINETASK_HANDLE;
typedef XONLINETASK_HANDLE* PXONLINETASK_HANDLE;

#define XONLINE_GAMERTAG_SIZE 16
#define XONLINE_MAX_LOGON_USERS 4
#define MAX_FRIENDS 100
#define MAX_MUTELISTUSERS 250
#define XONLINE_USER_OPTION_REQUIRE_PASSCODE 1

/* Public presence-state bit values used by the recovered menu code. */
#define XONLINE_FRIENDSTATE_FLAG_ONLINE 0x00000001
#define XONLINE_FRIENDSTATE_FLAG_PLAYING 0x00000002
#define XONLINE_FRIENDSTATE_FLAG_VOICE 0x00000004
#define XONLINE_FRIENDSTATE_FLAG_JOINABLE 0x00000008
#define XONLINE_FRIENDSTATE_FLAG_RECEIVEDREQUEST 0x00000010
#define XONLINE_FRIENDSTATE_FLAG_SENTREQUEST 0x00000020
#define XONLINE_FRIENDSTATE_FLAG_RECEIVEDINVITE 0x00000040
#define XONLINE_FRIENDSTATE_FLAG_SENTINVITE 0x00000080
#define XONLINE_FRIENDSTATE_FLAG_INVITEREJECTED 0x00000100

#pragma pack(push, 4)
struct XUID {
    unsigned long long qwUserID;
    DWORD dwUserFlags;
};

/* Retail mute-list iteration advances 0x10 bytes per user, compares the first
 * 0x0c-byte XUID, and clears 0xfa0 bytes per 250-entry controller list. */
struct XONLINE_MUTELISTUSER {
    XUID xuid;
    DWORD reserved;
};

/* Recovered from retail friend conversion at 0x1a3555: the destination is
 * cleared as 0x56 bytes, then fields are written at these exact offsets. */
#pragma pack(push, 1)
struct XONLINE_FRIEND {
    XUID xuid;
    char szGamertag[XONLINE_GAMERTAG_SIZE];
    DWORD dwFriendState;
    FILETIME gameinviteTime;
    XNKID sessionID;
    DWORD dwTitleID;
    BYTE StateDataSize;
    BYTE StateData[32];
    BYTE reserved55;
};

/* The retail press-start handler reserves 0x9c bytes for an accepted invite,
 * copies its friend from offset 0, and reads InviteAcceptTime at offset 0x64.
 * Its exact 15-minute comparison establishes the public-facing interval in
 * seconds without importing an SDK layout. */
#define XONLINE_ACCEPTED_GAMEINVITE_EXPIRATION_INTERVAL 900
struct XONLINE_ACCEPTED_GAMEINVITE {
    XONLINE_FRIEND InvitingFriend;
    BYTE unknown56[0x64 - sizeof(XONLINE_FRIEND)];
    FILETIME InviteAcceptTime;
    BYTE unknown6c[0x9c - 0x6c];
};

/* Retail presence processing advances 0x3d bytes per result, reads state at
 * 0x0c and title at 0x18, matching the recovered fields below. */
struct XONLINE_PRESENCE {
    XUID xuid;
    DWORD dwUserState;
    XNKID SessionID;
    DWORD dwTitleID;
    BYTE StateDataSize;
    BYTE StateData[32];
};
#pragma pack(pop)

/* Retail virtual-keyboard accesses at 0x237b3c..0x237b72 establish four
 * UTF-16 fields at offsets 0x000, 0x020, 0x220 and 0x420.  Its constructor
 * clears the complete 0x688-byte property record at object offset 0x3730. */
struct XONLINE_TEAM_PROPERTIES {
    WCHAR wszTeamName[16];
    WCHAR wszDescription[256];
    WCHAR wszMotto[256];
    WCHAR wszURL[256];
    BYTE unknown620[0x68];
};

/* Retail team-detail code at 0xabdb0 clears exactly 0x6a2 bytes.  Named
 * fields remain intentionally unrecovered until their offsets are needed. */
struct XONLINE_TEAM {
    BYTE data[0x6a2];
};

/* Retail team-member detail code clears 0x92 bytes and reads privileges at
 * offset 0x1c; list code treats the same record as the recovered 0x92-byte
 * online-player layout. */
#pragma pack(push, 1)
struct XONLINE_TEAM_MEMBER_PROPERTIES {
    DWORD dwPrivileges;
    DWORD TeamMemberDataSize;
    BYTE TeamMemberData[0x6e];
};

struct XONLINE_TEAM_MEMBER {
    XUID xuidTeamMember;
    char szGamertag[XONLINE_GAMERTAG_SIZE];
    XONLINE_TEAM_MEMBER_PROPERTIES TeamMemberProperties;
};
#pragma pack(pop)

struct XONLINE_USER {
    XUID xuid;
    char szGamertag[XONLINE_GAMERTAG_SIZE];
    DWORD dwUserOptions;
    BYTE passcode[4];
    BYTE unknown24[0x6c - 0x24];
    HRESULT hr;
};
#pragma pack(pop)

static_assert(sizeof(XUID) == 0x0c, "retail XUID layout");
static_assert(sizeof(XONLINE_MUTELISTUSER) == 0x10, "retail mute-list user layout");
static_assert(sizeof(XONLINE_MUTELISTUSER) * MAX_MUTELISTUSERS == 0xfa0,
              "retail per-controller mute-list size");
static_assert(sizeof(XONLINE_FRIEND) == 0x56, "retail XONLINE_FRIEND layout");
static_assert(sizeof(XONLINE_ACCEPTED_GAMEINVITE) == 0x9c,
              "retail accepted-invite layout");
static_assert(sizeof(XONLINE_TEAM_PROPERTIES) == 0x688,
              "retail team-properties layout");
static_assert(__builtin_offsetof(XONLINE_TEAM_PROPERTIES, wszDescription) == 0x20,
              "retail team description offset");
static_assert(__builtin_offsetof(XONLINE_TEAM_PROPERTIES, wszMotto) == 0x220,
              "retail team motto offset");
static_assert(__builtin_offsetof(XONLINE_TEAM_PROPERTIES, wszURL) == 0x420,
              "retail team URL offset");
static_assert(__builtin_offsetof(XONLINE_ACCEPTED_GAMEINVITE, InviteAcceptTime) == 0x64,
              "retail accepted-invite time offset");
static_assert(__builtin_offsetof(XONLINE_FRIEND, dwFriendState) == 0x1c,
              "retail friend-state offset");
static_assert(__builtin_offsetof(XONLINE_FRIEND, gameinviteTime) == 0x20,
              "retail invite-time offset");
static_assert(__builtin_offsetof(XONLINE_FRIEND, sessionID) == 0x28,
              "retail friend-session offset");
static_assert(__builtin_offsetof(XONLINE_FRIEND, dwTitleID) == 0x30,
              "retail friend-title offset");
static_assert(__builtin_offsetof(XONLINE_FRIEND, StateDataSize) == 0x34,
              "retail friend-state-data size offset");
static_assert(__builtin_offsetof(XONLINE_FRIEND, StateData) == 0x35,
              "retail friend-state-data offset");
static_assert(sizeof(XONLINE_TEAM) == 0x6a2, "retail team-detail layout");
static_assert(sizeof(XONLINE_TEAM_MEMBER_PROPERTIES) == 0x76,
              "retail team-member properties layout");
static_assert(sizeof(XONLINE_TEAM_MEMBER) == 0x92, "retail team-member layout");
static_assert(__builtin_offsetof(XONLINE_TEAM_MEMBER, TeamMemberProperties) == 0x1c,
              "retail team-member properties offset");
static_assert(sizeof(XONLINE_PRESENCE) == 0x3d, "retail XONLINE_PRESENCE layout");
static_assert(__builtin_offsetof(XONLINE_PRESENCE, dwUserState) == 0x0c,
              "retail presence-state offset");
static_assert(__builtin_offsetof(XONLINE_PRESENCE, SessionID) == 0x10,
              "retail presence-session offset");
static_assert(__builtin_offsetof(XONLINE_PRESENCE, dwTitleID) == 0x18,
              "retail presence-title offset");
static_assert(__builtin_offsetof(XONLINE_PRESENCE, StateDataSize) == 0x1c,
              "retail presence-state-data size offset");
static_assert(__builtin_offsetof(XONLINE_PRESENCE, StateData) == 0x1d,
              "retail presence-state-data offset");
static_assert(__builtin_offsetof(XONLINE_USER, passcode) == 0x20,
              "retail online-user passcode offset");
static_assert(sizeof(XONLINE_USER) == 0x70, "retail XONLINE_USER layout");

BOOL __stdcall XOnlineAreUsersIdentical(const XUID* first, const XUID* second);
HRESULT __stdcall XOnlineFriendsGetAcceptedGameInvite(XONLINE_ACCEPTED_GAMEINVITE* invite);
HRESULT __stdcall XOnlineMutelistAdd(DWORD controller, XUID user);
HRESULT __stdcall XOnlineMutelistGet(DWORD controller, DWORD capacity, void* event,
                                     PXONLINETASK_HANDLE task,
                                     XONLINE_MUTELISTUSER* users, DWORD* count);
HRESULT __stdcall XOnlineMutelistRemove(DWORD controller, XUID user);
HRESULT __stdcall XOnlineMutelistStartup(void* event, PXONLINETASK_HANDLE task);
DWORD __stdcall XOnlineUserGuestNumber(DWORD user_flags);

inline BOOL XOnlineIsUserGuest(DWORD user_flags) {
    return (user_flags & 3) != 0;
}

#endif
