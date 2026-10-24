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

/* Friend-state bits, from the retail friend-flag conversions (0x8cf00 and
 * its inverse 0x8cf80) and the request answer (0x8ce40). */
#define XONLINE_FRIENDSTATE_FLAG_ONLINE 0x00000001
#define XONLINE_FRIENDSTATE_FLAG_PLAYING 0x00000002
#define XONLINE_FRIENDSTATE_FLAG_VOICE 0x00000008
#define XONLINE_FRIENDSTATE_FLAG_JOINABLE 0x00000010
#define XONLINE_FRIENDSTATE_FLAG_SENTINVITE 0x04000000
#define XONLINE_FRIENDSTATE_FLAG_RECEIVEDINVITE 0x08000000
#define XONLINE_FRIENDSTATE_FLAG_INVITEREJECTED 0x20000000
#define XONLINE_FRIENDSTATE_FLAG_SENTREQUEST 0x40000000
#define XONLINE_FRIENDSTATE_FLAG_RECEIVEDREQUEST 0x80000000

/* Presence-state bits, from the retail presence-flag conversion (0x8c280). */
#define XONLINE_PRESENCE_FLAG_ONLINE 0x00000001
#define XONLINE_PRESENCE_FLAG_PLAYING 0x00000002
#define XONLINE_PRESENCE_FLAG_VOICE 0x00000008
#define XONLINE_PRESENCE_FLAG_JOINABLE 0x00000010
#define XONLINE_PRESENCE_FLAG_RECEIVEDTEAMRECRUIT 0x00000100
#define XONLINE_PRESENCE_FLAG_RECEIVEDCOMPREMINDER 0x00000200
#define XONLINE_PRESENCE_FLAG_RECEIVEDCOMPREQUEST 0x00000400
#define XONLINE_PRESENCE_FLAG_RECEIVEDTITLECUSTOM 0x00000800
#define XONLINE_PRESENCE_FLAG_RECEIVEDINVITE 0x08000000
#define XONLINE_PRESENCE_FLAG_RECEIVEDREQUEST 0x80000000

/* Task results: retail task update (0x6c450) and close (0x6b640) treat 0 and
 * 0x1500f2 as still running and 0x1500f1 as results available. */
#define XONLINETASK_S_RUNNING ((HRESULT)0x00000000)
#define XONLINETASK_S_RESULTS_AVAIL ((HRESULT)0x001500f1)
#define XONLINETASK_S_RUNNING_IDLE ((HRESULT)0x001500f2)
#define XONLINETASK_S_SUCCESS ((HRESULT)0x001500f0) /* PROVISIONAL */

/* Logon results, from the retail logon-result switch and its jump table
 * (0x6cd50) and the logged-on user checks (0x6c6c0, 0x6c750). */
#define XONLINE_S_LOGON_CONNECTION_ESTABLISHED ((HRESULT)0x001510f0)
#define XONLINE_S_LOGON_USER_HAS_MESSAGE ((HRESULT)0x001512f0)
#define XONLINE_E_LOGON_CANNOT_ACCESS_SERVICE ((HRESULT)0x80151001)
#define XONLINE_E_LOGON_UPDATE_REQUIRED ((HRESULT)0x80151002)
#define XONLINE_E_LOGON_SERVERS_TOO_BUSY ((HRESULT)0x80151003)
#define XONLINE_E_LOGON_CONNECTION_LOST ((HRESULT)0x80151004)
#define XONLINE_E_LOGON_KICKED_BY_DUPLICATE_LOGON ((HRESULT)0x80151005)
#define XONLINE_E_LOGON_INVALID_USER ((HRESULT)0x80151006)
#define XONLINE_E_LOGON_USER_ACCOUNT_REQUIRES_MANAGEMENT ((HRESULT)0x80151200)

/* Passcode buttons, from the retail button translation (0x6c850). */
#define XONLINE_PASSCODE_DPAD_UP 1
#define XONLINE_PASSCODE_DPAD_DOWN 2
#define XONLINE_PASSCODE_DPAD_LEFT 3
#define XONLINE_PASSCODE_DPAD_RIGHT 4
#define XONLINE_PASSCODE_GAMEPAD_X 5
#define XONLINE_PASSCODE_GAMEPAD_Y 6
#define XONLINE_PASSCODE_GAMEPAD_LEFT_TRIGGER 9
#define XONLINE_PASSCODE_GAMEPAD_RIGHT_TRIGGER 10

/* Message details: retail 0x8e950 passes 0x10 as the details flags. */
#define XONLINE_MSG_FLAG_READ 0x00000010

/* Match attribute types: retail 0xb4a90 stores 0 for an integer and
 * 0x00f00000 for a parameter left out; the result specs at 0x44050c are
 * {0, 8} pairs. */
#define X_ATTRIBUTE_DATATYPE_INTEGER 0x00000000
#define X_ATTRIBUTE_DATATYPE_NULL 0x00f00000

/* Feedback kinds, from the retail table built by 0x8d3d0. */
typedef enum _XONLINE_FEEDBACK_TYPE {
    XONLINE_FEEDBACK_NEG_NICKNAME = 0,
    XONLINE_FEEDBACK_NEG_GAMEPLAY = 1,
    XONLINE_FEEDBACK_NEG_SCREAMING = 2,
    XONLINE_FEEDBACK_NEG_HARASSMENT = 3,
    XONLINE_FEEDBACK_NEG_LEWDNESS = 4,
    XONLINE_FEEDBACK_POS_ATTITUDE = 5,
    XONLINE_FEEDBACK_POS_SESSION = 6
} XONLINE_FEEDBACK_TYPE;

/* Answers, from the retail answer tables (0x8ce40, 0x8d050, 0x8d0b0). */
typedef enum _XONLINE_REQUEST_ANSWER_TYPE {
    XONLINE_REQUEST_NO = 0,
    XONLINE_REQUEST_YES = 1,
    XONLINE_REQUEST_BLOCK = 2
} XONLINE_REQUEST_ANSWER_TYPE;

typedef enum _XONLINE_GAMEINVITE_ANSWER_TYPE {
    XONLINE_GAMEINVITE_NO = 0,
    XONLINE_GAMEINVITE_YES = 1,
    XONLINE_GAMEINVITE_REMOVE = 2
} XONLINE_GAMEINVITE_ANSWER_TYPE;

typedef enum _XONLINE_PEER_ANSWER_TYPE {
    XONLINE_PEER_ANSWER_NO = 0,
    XONLINE_PEER_ANSWER_YES = 1,
    XONLINE_PEER_ANSWER_NEVER = 2
} XONLINE_PEER_ANSWER_TYPE;

/* NAT kinds, from the retail NAT switch (0x6b550). */
typedef enum _XONLINE_NAT_TYPE {
    XONLINE_NAT_OPEN = 1,
    XONLINE_NAT_MODERATE = 2,
    XONLINE_NAT_STRICT = 3
} XONLINE_NAT_TYPE;

typedef HANDLE XONLINE_MSG_HANDLE;

#pragma pack(push, 4)
/* The retail mangled names of the voice engine use the _XUID tag. */
typedef struct _XUID {
    unsigned long long qwUserID;
    DWORD dwUserFlags;
} XUID;

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
/* Statistics uploads (src/unknown_072db0.cpp function_73e00).  PROVISIONAL:
 * the public shape of these records; the retail code shows a 16-byte stat
 * with its id at +0, its type as a dword at +4 (2 = 64-bit) and its value at
 * +8, and the procedure ids and comparisons below.  Unused offline. */
#define XONLINE_STAT_LONG 1
#define XONLINE_STAT_LONGLONG 2
#define XONLINE_STAT_PROCID_CONDITIONAL 0x8007
#define XONLINE_STAT_PROCID_UPDATE_REPLACE 0x8001
#define XONLINE_STAT_PROCID_UPDATE_INCREMENT 0x8003
/* comparisons, in the order function_73e00 stores them (7, 6, 4) */
#define XONLINE_STAT_COMPTYPE_NOT_EXISTS 7
#define XONLINE_STAT_COMPTYPE_EXISTS 6
#define XONLINE_STAT_COMPTYPE_LESS 4
typedef struct XONLINE_STAT {
    WORD wID;
    WORD reserved;
    DWORD type;
    union {
        LONG lValue;
        LONGLONG llValue;
        double dValue;
    };
} XONLINE_STAT;

struct XONLINE_TEAM_PROPERTIES {
    WCHAR wszTeamName[16];
    WCHAR wszDescription[256];
    WCHAR wszMotto[256];
    WCHAR wszURL[256];
    BYTE unknown620[0x68];
};

/* Retail team-detail code at 0xabdb0 clears exactly 0x6a2 bytes; the
 * presence cache (0x8bf70) copies the team name from +0x0c, after the team's
 * XUID. */
#pragma pack(push, 2)
struct XONLINE_TEAM {
    XUID xuidTeam;
    XONLINE_TEAM_PROPERTIES TeamProperties;
    BYTE unknown694[0x6a2 - 0x694];
};
#pragma pack(pop)

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

typedef struct _XONLINE_USER {
    XUID xuid;
    char szGamertag[XONLINE_GAMERTAG_SIZE];
    DWORD dwUserOptions;
    BYTE passcode[4];
    BYTE unknown24[0x6c - 0x24];
    HRESULT hr;
} XONLINE_USER;
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
static_assert(__builtin_offsetof(XONLINE_TEAM, TeamProperties) == 0x0c,
              "retail team-properties offset");
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

#pragma pack(push, 4)
/* Startup parameters: retail transport startup (0x8d840) passes a single
 * zeroed dword. */
typedef struct _XONLINE_STARTUP_PARAMS {
    DWORD unknown00; /* PROVISIONAL */
} XONLINE_STARTUP_PARAMS;

/* Service information: retail 0x6d220 reserves 0x0c bytes and connects to
 * the address at +4. */
typedef struct _XONLINE_SERVICE_INFO {
    DWORD dwServiceID;
    IN_ADDR serviceIP;
    BYTE unknown08[4];
} XONLINE_SERVICE_INFO;

/* Saved logon state: retail 0x22387b copies 0x204 bytes into the downloader
 * launch data at +0x10. */
typedef struct _XONLINE_LOGON_STATE {
    BYTE data[0x204];
} XONLINE_LOGON_STATE;

typedef struct _LD_DOWNLOADER {
    BYTE unknown000[0x10];
    XONLINE_LOGON_STATE LogonState;
    BYTE unknown214[MAX_LAUNCH_DATA_SIZE - 0x214];
} LD_DOWNLOADER;

/* Feedback parameters: the recovered code sets only the string. */
typedef struct _XONLINE_FEEDBACK_PARAMS {
    const WCHAR* lpStringParam;
} XONLINE_FEEDBACK_PARAMS;

/* Game-invite answer: retail 0x8d0b0 fills a 0x30-byte record with the
 * inviter at +0, the gamertag at +0x0c, the title at +0x1c, the session at
 * +0x20 and the invite time at +0x28. */
typedef struct _XONLINE_GAMEINVITE_ANSWER_INFO {
    XUID xuidInvitingUser;
    char szInvitingUserGamertag[XONLINE_GAMERTAG_SIZE];
    DWORD dwTitleID;
    XNKID SessionID;
    FILETIME GameInviteTime;
} XONLINE_GAMEINVITE_ANSWER_INFO;

/* Message summaries: retail enumeration (0x8e750) reserves 125 records of
 * 0x40 bytes; the recovered code reads them through its own view. */
typedef struct _XONLINE_MSG_SUMMARY {
    BYTE data[0x40];
} XONLINE_MSG_SUMMARY;

/* Statistics: retail 0x92220 walks 0x18-byte specs reading the user flags
 * at +8; 0x925c0 walks 0x88-byte procedures, reading the procedure at +0
 * and the update's user at +8. */
typedef struct _XONLINE_STAT_SPEC {
    XUID xuidUser;
    BYTE unknown0c[0x0c];
} XONLINE_STAT_SPEC, *PXONLINE_STAT_SPEC;

/* The update and conditional parts' other fields: PROVISIONAL placement
   after the user (function_73e00 names them; unused offline). */
typedef struct _XONLINE_STAT_UPDATE {
    XUID xuid;
    DWORD dwLeaderBoardID;
    DWORD dwConditionalIndex;
    DWORD dwNumStats;
    XONLINE_STAT *pStats;
    BYTE unknown1c[0x88 - 0x08 - 0x0c - 0x10];
} XONLINE_STAT_UPDATE;

typedef struct _XONLINE_STAT_CONDITIONAL {
    XUID xuid;
    DWORD dwLeaderBoardID;
    BYTE bComparisonType;
    BYTE unknown0d[3];
    XONLINE_STAT StatToCompare;
} XONLINE_STAT_CONDITIONAL;

typedef struct _XONLINE_STAT_PROC {
    WORD wProcedureID;
    BYTE unknown02[6];
    union {
        XONLINE_STAT_UPDATE Update;
        XONLINE_STAT_CONDITIONAL Conditional;
    };
} XONLINE_STAT_PROC, *PXONLINE_STAT_PROC;

typedef struct _XONLINE_STAT_ATTACHMENT_REFERENCE XONLINE_STAT_ATTACHMENT_REFERENCE,
    *PXONLINE_STAT_ATTACHMENT_REFERENCE;

/* Arbitration: retail 0x924e0 builds a 0x10-byte session and round key. */
typedef struct _XONLINE_ARB_ID {
    XNKID SessionID;
    ULONGLONG qwRoundID;
} XONLINE_ARB_ID;

/* Match attributes: the recovered builders write 0x10-byte records of an
 * identifier, a changed flag and a 64-bit value; the result specs at
 * 0x44050c are 8-byte type and length pairs. */
typedef struct _XONLINE_ATTRIBUTE {
    DWORD attribute_id;
    BOOL changed;
    ULONGLONG value;
} XONLINE_ATTRIBUTE, *PXONLINE_ATTRIBUTE;

typedef struct _XONLINE_ATTRIBUTE_SPEC {
    DWORD type;
    DWORD length;
} XONLINE_ATTRIBUTE_SPEC, *PXONLINE_ATTRIBUTE_SPEC;

/* Search results: retail 0x8fb30 reads the session at +4, the host address
 * at +0x0c, the key at +0x30, the open and filled counts at +0x40..+0x4c
 * and the attribute count at +0x50. */
typedef struct _XONLINE_MATCH_SEARCHRESULT {
    DWORD unknown00;
    XNKID SessionID;
    XNADDR HostAddress;
    XNKEY KeyExchangeKey;
    DWORD dwPublicOpen;
    DWORD dwPrivateOpen;
    DWORD dwPublicFilled;
    DWORD dwPrivateFilled;
    DWORD dwNumAttributes;
} XONLINE_MATCH_SEARCHRESULT, *PXONLINE_MATCH_SEARCHRESULT;
#pragma pack(pop)

/* Notifications: retail 0x6d080 reads the message type byte at +0, the
 * message at +1 and the notify flags at +5 of a record below 0x0c bytes. */
#pragma pack(push, 1)
typedef struct _XONLINE_NOTIFICATION_EX_INFO {
    BYTE bMessageType;
    DWORD dwMessageID;
    DWORD dwNotifyFlags;
    BYTE unknown09[3]; /* PROVISIONAL */
} XONLINE_NOTIFICATION_EX_INFO;
#pragma pack(pop)

static_assert(sizeof(XONLINE_SERVICE_INFO) == 0x0c, "retail service-info layout");
static_assert(sizeof(LD_DOWNLOADER) == MAX_LAUNCH_DATA_SIZE, "retail downloader launch-data size");
static_assert(__builtin_offsetof(LD_DOWNLOADER, LogonState) == 0x10, "retail logon-state offset");
static_assert(sizeof(XONLINE_GAMEINVITE_ANSWER_INFO) == 0x30, "retail invite-answer layout");
static_assert(__builtin_offsetof(XONLINE_GAMEINVITE_ANSWER_INFO, dwTitleID) == 0x1c,
              "retail invite-answer title offset");
static_assert(__builtin_offsetof(XONLINE_GAMEINVITE_ANSWER_INFO, GameInviteTime) == 0x28,
              "retail invite-answer time offset");
static_assert(sizeof(XONLINE_MSG_SUMMARY) == 0x40, "retail message-summary stride");
static_assert(sizeof(XONLINE_STAT_SPEC) == 0x18, "retail statistic-spec stride");
static_assert(sizeof(XONLINE_STAT_PROC) == 0x88, "retail statistic-procedure stride");
static_assert(__builtin_offsetof(XONLINE_STAT_PROC, Update) == 0x08, "retail procedure update offset");
static_assert(sizeof(XONLINE_ARB_ID) == 0x10, "retail arbitration-id layout");
static_assert(sizeof(XONLINE_ATTRIBUTE) == 0x10, "recovered attribute layout");
static_assert(sizeof(XONLINE_ATTRIBUTE_SPEC) == 0x08, "retail attribute-spec layout");
static_assert(__builtin_offsetof(XONLINE_MATCH_SEARCHRESULT, HostAddress) == 0x0c,
              "retail search-result address offset");
static_assert(__builtin_offsetof(XONLINE_MATCH_SEARCHRESULT, KeyExchangeKey) == 0x30,
              "retail search-result key offset");
static_assert(__builtin_offsetof(XONLINE_MATCH_SEARCHRESULT, dwNumAttributes) == 0x50,
              "retail search-result attribute-count offset");
static_assert(__builtin_offsetof(XONLINE_NOTIFICATION_EX_INFO, dwNotifyFlags) == 0x05,
              "retail notification flags offset");

#ifdef __cplusplus
extern "C" {
#endif

BOOL __stdcall XOnlineAreUsersIdentical(const XUID* first, const XUID* second);
HRESULT __stdcall XOnlineFriendsGetAcceptedGameInvite(XONLINE_ACCEPTED_GAMEINVITE* invite);
HRESULT __stdcall XOnlineMutelistAdd(DWORD controller, XUID user);
HRESULT __stdcall XOnlineMutelistGet(DWORD controller, DWORD capacity, void* event,
                                     PXONLINETASK_HANDLE task,
                                     XONLINE_MUTELISTUSER* users, DWORD* count);
HRESULT __stdcall XOnlineMutelistRemove(DWORD controller, XUID user);
HRESULT __stdcall XOnlineMutelistStartup(void* event, PXONLINETASK_HANDLE task);
DWORD __stdcall XOnlineUserGuestNumber(DWORD user_flags);

/* Signatures below follow the recovered call sites. */
HRESULT __stdcall XOnlineStartup(const XONLINE_STARTUP_PARAMS* parameters);
HRESULT __stdcall XOnlineCleanup(void);
HRESULT __stdcall XOnlineTaskContinue(XONLINETASK_HANDLE task);
HRESULT __stdcall XOnlineTaskClose(XONLINETASK_HANDLE task);
HRESULT __stdcall XOnlineLogon(const XONLINE_USER* users, const DWORD* service_ids,
                               DWORD service_count, HANDLE event, PXONLINETASK_HANDLE task);
HRESULT __stdcall XOnlineLogonTaskGetResults(XONLINETASK_HANDLE task);
HRESULT __stdcall XOnlineChangeLogonUsers(const XONLINE_USER* users, HANDLE event,
                                          PXONLINETASK_HANDLE task);
HRESULT __stdcall XOnlineChangeLogonUsersTaskGetResults(XONLINETASK_HANDLE task, HRESULT* results);
XONLINE_USER* __stdcall XOnlineGetLogonUsers(void);
HRESULT __stdcall XOnlineGetUsers(XONLINE_USER* users, DWORD* count);
HRESULT __stdcall XOnlineGetServiceInfo(DWORD service_id, XONLINE_SERVICE_INFO* info);
DWORD __stdcall XOnlineGetNatType(void);
HRESULT __stdcall XOnlineSaveLogonState(XONLINE_LOGON_STATE* state);
BOOL __stdcall XOnlineTitleIdIsSameTitle(DWORD title_id);
BOOL __stdcall XOnlineGetNotificationEx(DWORD user_index, XONLINE_NOTIFICATION_EX_INFO* info,
                                        DWORD* state_flags);
HRESULT __stdcall XOnlineNotificationSetState(DWORD user_index, DWORD state_flags, XNKID session_id,
                                              DWORD state_data_size, const BYTE* state_data);
HRESULT __stdcall XOnlineFeedbackSend(DWORD user_index, XUID user, XONLINE_FEEDBACK_TYPE type,
                                      const XONLINE_FEEDBACK_PARAMS* parameters, HANDLE event,
                                      PXONLINETASK_HANDLE task);
HRESULT __stdcall XOnlineOfferingIsNewContentAvailable(DWORD categories, HANDLE event,
                                                       PXONLINETASK_HANDLE task);

HRESULT __stdcall XOnlineFriendsStartup(HANDLE event, PXONLINETASK_HANDLE task);
HRESULT __stdcall XOnlineFriendsEnumerate(DWORD user_index, HANDLE event, PXONLINETASK_HANDLE task);
HRESULT __stdcall XOnlineFriendsEnumerateFinish(XONLINETASK_HANDLE task);
DWORD __stdcall XOnlineFriendsGetLatest(DWORD user_index, DWORD capacity, XONLINE_FRIEND* friends);
HRESULT __stdcall XOnlineFriendsRemove(DWORD user_index, const XONLINE_FRIEND* friend_);
HRESULT __stdcall XOnlineFriendsAnswerRequest(DWORD user_index, const XONLINE_FRIEND* friend_,
                                              XONLINE_REQUEST_ANSWER_TYPE answer);
HRESULT __stdcall XOnlineFriendsGameInvite(DWORD user_index, XNKID session_id, DWORD friend_count,
                                           const XONLINE_FRIEND* friends);
HRESULT __stdcall XOnlineFriendsAnswerGameInvite(DWORD user_index, const XONLINE_FRIEND* friend_,
                                                 XONLINE_GAMEINVITE_ANSWER_TYPE answer);
HRESULT __stdcall XOnlineFriendsJoinGame(DWORD user_index, const XONLINE_FRIEND* friend_);
HRESULT __stdcall XOnlineFriendsGetTitleName(DWORD title_id, DWORD language, DWORD name_length,
                                             WCHAR* name);
HRESULT __stdcall XOnlineFriendsRequestEx(DWORD user_index, XUID user, XONLINE_MSG_HANDLE message,
                                          HANDLE event, PXONLINETASK_HANDLE task);
HRESULT __stdcall XOnlineFriendsRequestByNameEx(DWORD user_index, const char* gamertag,
                                                XONLINE_MSG_HANDLE message, HANDLE event,
                                                PXONLINETASK_HANDLE task);
HRESULT __stdcall XOnlineGameInviteAnswer(DWORD user_index, const XONLINE_GAMEINVITE_ANSWER_INFO* info,
                                          XONLINE_PEER_ANSWER_TYPE answer, HANDLE event,
                                          PXONLINETASK_HANDLE task);
HRESULT __stdcall XOnlineGameInviteSend(DWORD user_index, DWORD recipient_count,
                                        const XUID* recipients, XNKID session_id, DWORD flags,
                                        XONLINE_MSG_HANDLE message, HANDLE event,
                                        PXONLINETASK_HANDLE task);

HRESULT __stdcall XOnlinePresenceInit(DWORD user_index, HANDLE event, PXONLINETASK_HANDLE task);
HRESULT __stdcall XOnlinePresenceAdd(XONLINETASK_HANDLE task, DWORD group_id, DWORD user_count,
                                     const XUID* users);
HRESULT __stdcall XOnlinePresenceSubmit(XONLINETASK_HANDLE task);
HRESULT __stdcall XOnlinePresenceGetLatest(XONLINETASK_HANDLE task, DWORD group_id, DWORD count,
                                           XONLINE_PRESENCE* presences);
HRESULT __stdcall XOnlinePresenceClear(XONLINETASK_HANDLE task);

HRESULT __stdcall XOnlineMessageEnumerate(DWORD user_index, XONLINE_MSG_SUMMARY* summaries,
                                          DWORD* count);
HRESULT __stdcall XOnlineMessageDetails(DWORD user_index, DWORD message_id, DWORD flags_to_set,
                                        DWORD flags_to_clear, HANDLE event,
                                        PXONLINETASK_HANDLE task);
HRESULT __stdcall XOnlineMessageDetailsGetResultsProperty(XONLINETASK_HANDLE task, WORD tag,
                                                          DWORD size, void* buffer,
                                                          DWORD* required_size, void* reserved);
HRESULT __stdcall XOnlineMessageDownloadAttachmentToMemory(XONLINETASK_HANDLE details_task, WORD tag,
                                                           BYTE* buffer, DWORD size, HANDLE event,
                                                           PXONLINETASK_HANDLE task);
HRESULT __stdcall XOnlineMessageDownloadAttachmentToMemoryGetResults(XONLINETASK_HANDLE task,
                                                                     BYTE** data,
                                                                     DWORD* received_size,
                                                                     DWORD* total_size);
HRESULT __stdcall XOnlineMessageDelete(DWORD user_index, DWORD message_id, BOOL block_sender);
HRESULT __stdcall XOnlineMessageCreate(DWORD message_type, DWORD property_count,
                                       DWORD attachment_size, ULONGLONG context, DWORD flags,
                                       DWORD expire_minutes, XONLINE_MSG_HANDLE* message);
HRESULT __stdcall XOnlineMessageSetProperty(XONLINE_MSG_HANDLE message, WORD tag, DWORD size,
                                            const void* value, DWORD flags);
HRESULT __stdcall XOnlineMessageSend(DWORD user_index, XONLINE_MSG_HANDLE message,
                                     DWORD recipient_count, const XUID* recipients,
                                     HANDLE event, PXONLINETASK_HANDLE task);
HRESULT __stdcall XOnlineMessageDestroy(XONLINE_MSG_HANDLE message);

DWORD __stdcall XOnlineMatchSearchResultsLen(DWORD result_count, DWORD attribute_count,
                                             const XONLINE_ATTRIBUTE_SPEC* specs);
HRESULT __stdcall XOnlineMatchSearch(DWORD procedure, DWORD maximum_results,
                                     DWORD attribute_count, const XONLINE_ATTRIBUTE* attributes,
                                     DWORD results_size, HANDLE event, PXONLINETASK_HANDLE task);
HRESULT __stdcall XOnlineMatchSearchGetResults(XONLINETASK_HANDLE task,
                                               PXONLINE_MATCH_SEARCHRESULT** results,
                                               DWORD* count);
HRESULT __stdcall XOnlineMatchSearchParse(const XONLINE_MATCH_SEARCHRESULT* result,
                                          DWORD spec_count, const XONLINE_ATTRIBUTE_SPEC* specs,
                                          void* values);
HRESULT __stdcall XOnlineMatchSessionCreate(DWORD public_filled, DWORD public_open,
                                            DWORD private_filled, DWORD private_open,
                                            DWORD attribute_count,
                                            const XONLINE_ATTRIBUTE* attributes,
                                            HANDLE event, PXONLINETASK_HANDLE task);
HRESULT __stdcall XOnlineMatchSessionUpdate(XNKID session_id, DWORD public_filled,
                                            DWORD public_open, DWORD private_filled,
                                            DWORD private_open, DWORD attribute_count,
                                            const XONLINE_ATTRIBUTE* attributes,
                                            HANDLE event, PXONLINETASK_HANDLE task);
HRESULT __stdcall XOnlineMatchSessionGetInfo(XONLINETASK_HANDLE task, XNKID* session_id, XNKEY* key);
HRESULT __stdcall XOnlineMatchSessionDelete(XNKID session_id, HANDLE event, PXONLINETASK_HANDLE task);
HRESULT __stdcall XOnlineMatchSessionFindFromID(XNKID session_id, HANDLE event,
                                                PXONLINETASK_HANDLE task);

HRESULT __stdcall XOnlineStatRead(DWORD count, XONLINE_STAT_SPEC* specs, HANDLE event,
                                  PXONLINETASK_HANDLE task);
HRESULT __stdcall XOnlineStatReadGetResult(XONLINETASK_HANDLE task, DWORD count,
                                           XONLINE_STAT_SPEC* specs, DWORD extra_size,
                                           BYTE* extra_buffer);
HRESULT __stdcall XOnlineStatWrite(DWORD count, const XONLINE_STAT_SPEC* specs, HANDLE event,
                                   PXONLINETASK_HANDLE task);
HRESULT __stdcall XOnlineStatWriteGetResult(XONLINETASK_HANDLE task, HANDLE* result,
                                            PXONLINE_STAT_ATTACHMENT_REFERENCE* references,
                                            DWORD* count);
HRESULT __stdcall XOnlineArbitrationCreateRoundID(ULONGLONG* round_id);
HRESULT __stdcall XOnlineArbitrationRegister(const XONLINE_ARB_ID* id, DWORD seconds, DWORD flags,
                                             HANDLE event, PXONLINETASK_HANDLE task);
HRESULT __stdcall XOnlineArbitrationReport(const XONLINE_ARB_ID* id, DWORD procedure_count,
                                           const XONLINE_STAT_PROC* procedures,
                                           const void* attachments, DWORD flags, HANDLE event,
                                           PXONLINETASK_HANDLE task);
HRESULT __stdcall XOnlineArbitrationExtendRound(const XONLINE_ARB_ID* id, DWORD seconds,
                                                HANDLE event, PXONLINETASK_HANDLE task);

HRESULT __stdcall XOnlineStorageCreateServerPath(DWORD facility, ULONGLONG user_id,
                                                 ULONGLONG team_id, const WCHAR* path,
                                                 WCHAR* server_path, DWORD* server_path_size);
HRESULT __stdcall XOnlineStorageDownload(DWORD facility, DWORD user_index, const WCHAR* server_path,
                                         const char* local_path, DWORD flags, HANDLE event,
                                         PXONLINETASK_HANDLE task);
HRESULT __stdcall XOnlineStorageDownloadToMemory(DWORD facility, DWORD user_index,
                                                 const WCHAR* server_path, BYTE* buffer,
                                                 DWORD size, DWORD flags, HANDLE event,
                                                 PXONLINETASK_HANDLE task);
HRESULT __stdcall XOnlineStorageDownloadToMemoryGetResults(XONLINETASK_HANDLE task, BYTE** data,
                                                           DWORD* received_size,
                                                           DWORD* total_size,
                                                           ULONGLONG* owner,
                                                           FILETIME* creation_time);
HRESULT __stdcall XOnlineStorageUploadByServerPath(DWORD facility, DWORD user_index,
                                                   const WCHAR* server_path, FILETIME expiration,
                                                   const char* local_path, DWORD flags,
                                                   HANDLE event, PXONLINETASK_HANDLE task);
HRESULT __stdcall XOnlineStorageUploadFromMemory(DWORD facility, DWORD user_index,
                                                 const WCHAR* server_path, FILETIME expiration,
                                                 const BYTE* buffer, DWORD size, DWORD flags,
                                                 HANDLE event, PXONLINETASK_HANDLE task);

HRESULT __stdcall XOnlineTeamCreate(DWORD user_index, const XONLINE_TEAM_PROPERTIES* properties,
                                    const XONLINE_TEAM_MEMBER_PROPERTIES* creator,
                                    DWORD maximum_members, HANDLE event, PXONLINETASK_HANDLE task);
HRESULT __stdcall XOnlineTeamDelete(DWORD user_index, XUID team, HANDLE event,
                                    PXONLINETASK_HANDLE task);
HRESULT __stdcall XOnlineTeamEnumerateByUserXUID(DWORD user_index, XUID user, HANDLE event,
                                                 PXONLINETASK_HANDLE task);
HRESULT __stdcall XOnlineTeamEnumerateGetResults(XONLINETASK_HANDLE task, DWORD* count, XUID* teams);
HRESULT __stdcall XOnlineTeamGetDetails(XONLINETASK_HANDLE task, XUID team, XONLINE_TEAM* details);
HRESULT __stdcall XOnlineTeamMembersEnumerate(DWORD user_index, XUID team, BOOL with_recruits,
                                              HANDLE event, PXONLINETASK_HANDLE task);
HRESULT __stdcall XOnlineTeamMembersEnumerateGetResults(XONLINETASK_HANDLE task, DWORD* count,
                                                        XUID* members);
HRESULT __stdcall XOnlineTeamMemberGetDetails(XONLINETASK_HANDLE task, XUID member,
                                              XONLINE_TEAM_MEMBER* details);
HRESULT __stdcall XOnlineTeamMemberSetProperties(DWORD user_index, XUID team, XUID member,
                                                 const XONLINE_TEAM_MEMBER_PROPERTIES* properties,
                                                 HANDLE event, PXONLINETASK_HANDLE task);
HRESULT __stdcall XOnlineTeamMemberRemove(DWORD user_index, XUID team, XUID member, HANDLE event,
                                          PXONLINETASK_HANDLE task);
HRESULT __stdcall XOnlineTeamMemberAnswerRecruit(DWORD user_index, XUID team,
                                                 XONLINE_PEER_ANSWER_TYPE answer, HANDLE event,
                                                 PXONLINETASK_HANDLE task);
HRESULT __stdcall XOnlineTeamMemberRecruit(DWORD user_index, XUID team, XUID user,
                                           const XONLINE_TEAM_MEMBER_PROPERTIES* properties,
                                           XONLINE_MSG_HANDLE message, HANDLE event,
                                           PXONLINETASK_HANDLE task);
HRESULT __stdcall XOnlineTeamMemberRecruitByName(DWORD user_index, XUID team, const char* gamertag,
                                                 const XONLINE_TEAM_MEMBER_PROPERTIES* properties,
                                                 XONLINE_MSG_HANDLE message, HANDLE event,
                                                 PXONLINETASK_HANDLE task);

#ifdef __cplusplus
}
#endif

inline BOOL XOnlineIsUserGuest(DWORD user_flags) {
    return (user_flags & 3) != 0;
}

#endif
