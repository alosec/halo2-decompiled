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

struct XONLINE_TEAM;
struct XONLINE_TEAM_MEMBER;

struct XONLINE_USER {
    XUID xuid;
    char szGamertag[XONLINE_GAMERTAG_SIZE];
    DWORD dwUserOptions;
    BYTE unknown20[0x6c - 0x20];
    HRESULT hr;
};
#pragma pack(pop)

static_assert(sizeof(XUID) == 0x0c, "retail XUID layout");
static_assert(sizeof(XONLINE_MUTELISTUSER) == 0x10, "retail mute-list user layout");
static_assert(sizeof(XONLINE_MUTELISTUSER) * MAX_MUTELISTUSERS == 0xfa0,
              "retail per-controller mute-list size");
static_assert(sizeof(XONLINE_USER) == 0x70, "retail XONLINE_USER layout");

BOOL __stdcall XOnlineAreUsersIdentical(const XUID* first, const XUID* second);
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
