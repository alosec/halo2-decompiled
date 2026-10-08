#ifndef H2_PORT_XDK_XONLINE_H
#define H2_PORT_XDK_XONLINE_H

#include <xtl.h>

/* Layouts below are independently recovered from retail field accesses and
 * record strides in the supported executable. They contain only fields the
 * open source currently names. */
#define XONLINE_GAMERTAG_SIZE 16
#define XONLINE_MAX_LOGON_USERS 4
#define XONLINE_USER_OPTION_REQUIRE_PASSCODE 1

#pragma pack(push, 4)
struct XUID {
    unsigned long long qwUserID;
    DWORD dwUserFlags;
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
static_assert(sizeof(XONLINE_USER) == 0x70, "retail XONLINE_USER layout");

inline BOOL XOnlineIsUserGuest(DWORD user_flags) {
    return (user_flags & 3) != 0;
}

#endif
