/* Offline Xbox networking for whole images: XNet, Winsock and Xbox Live as a
 * console with no network cable and no signed-in Live account sees them.
 *
 * The recovered transport (transport.cpp) only starts XNet, Winsock and Live
 * once XNetGetEthernetLinkStatus reports a link, so with the link down the
 * game takes its own "no network" paths: main-menu checks of the transport
 * (function_8d7c0) fail, Live and System Link report no connection, and
 * Campaign and split-screen run without any of it.  Everything below still
 * answers the way an unplugged console would if a call is reached anyway:
 * local bookkeeping (startup, keys, socket creation and options) succeeds,
 * anything that needs a peer or a Live service fails with a documented or
 * recovered error, and no Live task ever reports success.
 *
 * Written from the public Winsock reference and the recovered call sites;
 * guest_online_runtime.cpp separately overrides several recovered online_*
 * wrappers and does not define any of these SDK entry points. */
#include "h2/host_abi.h"
#include <xtl.h>
#include <xonline.h>
#include <string.h>

namespace {

/* ---- results ------------------------------------------------------------ */

/* PROVISIONAL name (XONLINE_E_LOGON_NO_NETWORK_CONNECTION): the recovered
 * task continue (0x6c670) substitutes exactly this value when the transport
 * is down or nobody is logged on, and the friends updater (0x6c560) treats it
 * as "offline, do not restart".  It is the natural offline answer. */
constexpr HRESULT k_online_e_no_network = (HRESULT)0x80151000L;
constexpr HRESULT k_e_invalidarg = (HRESULT)0x80070057L;

/* Public Winsock error not in the shim. */
constexpr int k_wsaeafnosupport = 10047;
constexpr int k_wsaeprotonosupport = 10043;

/* Halo 2's title identifier: the recovered service table (0x467178) and
 * title-server lookup (0xb3a70) use it as the title's own service id. */
constexpr DWORD k_title_id = 0x4d530064;

/* PROVISIONAL names for the four XNetGetConnectStatus results the recovered
 * switch (0x7acf0) distinguishes: idle, pending, connected, lost. */
constexpr DWORD k_xnet_connect_status_lost = 3;

/* Public Winsock ioctl that reports pending input. */
constexpr long k_fionread = 0x4004667fL;

int g_wsa_last_error;
int g_wsa_startup_count;
bool g_xnet_started;
bool g_xonline_started;
XNetStartupParams g_xnet_parameters;

int wsa_fail(int error)
{
    g_wsa_last_error = error;
    return SOCKET_ERROR;
}

/* ---- sockets: descriptor table only; nothing is ever transmitted -------- */

constexpr int k_socket_count = 64;
constexpr SOCKET k_socket_base = 0x100;

struct s_offline_socket {
    bool used;
    bool bound;
    bool nonblocking;
    bool shut_down;
    int type;
    int protocol;
    int send_buffer;
    int receive_buffer;
    int broadcast;
    int reuse_address;
};

s_offline_socket g_sockets[k_socket_count];

s_offline_socket *socket_get(SOCKET s)
{
    if (s < k_socket_base || s >= k_socket_base + k_socket_count)
        return 0;
    s_offline_socket *entry = &g_sockets[s - k_socket_base];
    return entry->used ? entry : 0;
}

/* Common prologue: Winsock must be started and the descriptor valid. */
s_offline_socket *socket_check(SOCKET s)
{
    if (g_wsa_startup_count <= 0) {
        g_wsa_last_error = WSANOTINITIALISED;
        return 0;
    }
    s_offline_socket *entry = socket_get(s);
    if (!entry)
        g_wsa_last_error = WSAENOTSOCK;
    return entry;
}

int *socket_option(s_offline_socket *entry, int level, int name)
{
    if (level != SOL_SOCKET)
        return 0;
    switch (name) {
    case SO_SNDBUF:
        return &entry->send_buffer;
    case SO_RCVBUF:
        return &entry->receive_buffer;
    case SO_BROADCAST:
        return &entry->broadcast;
    case SO_REUSEADDR:
        return &entry->reuse_address;
    }
    return 0;
}

/* ---- keys ---------------------------------------------------------------- */

DWORD g_random_state;

void random_bytes(BYTE *buffer, DWORD count)
{
    if (!g_random_state)
        g_random_state = GetTickCount() ^ 0x9e3779b9u;
    for (DWORD i = 0; i < count; i++) {
        g_random_state ^= g_random_state << 13;
        g_random_state ^= g_random_state >> 17;
        g_random_state ^= g_random_state << 5;
        buffer[i] = (BYTE)(g_random_state >> 24);
    }
}

/* ---- Live tasks ------------------------------------------------------------ */

/* The only task ever handed out: a logon that can never reach the service.
 * Every other task-starting call fails immediately and leaves a null handle,
 * which the recovered close path (0x6b640) skips. */
struct s_offline_task {
    DWORD kind;
};
s_offline_task g_logon_task = { 1 };

HRESULT task_refuse(PXONLINETASK_HANDLE task)
{
    if (task)
        *task = 0;
    return k_online_e_no_network;
}

} // namespace

extern "C" {

/* ---- Winsock ------------------------------------------------------------- */

int WINAPI WSAStartup(WORD version, LPWSADATA data)
{
    if (!data)
        return WSAEFAULT;
    memset(data, 0, sizeof(*data));
    const WORD highest = 0x0202;
    data->wVersion = version < highest ? version : highest;
    data->wHighVersion = highest;
    strcpy(data->szDescription, "Xbox offline sockets");
    strcpy(data->szSystemStatus, "No network link");
    data->iMaxSockets = k_socket_count;
    data->iMaxUdpDg = 1264;
    g_wsa_startup_count++;
    return 0;
}

int WINAPI WSACleanup(void)
{
    if (g_wsa_startup_count <= 0)
        return wsa_fail(WSANOTINITIALISED);
    if (--g_wsa_startup_count == 0)
        memset(g_sockets, 0, sizeof(g_sockets));
    return 0;
}

int WINAPI WSAGetLastError(void)
{
    return g_wsa_last_error;
}

void WINAPI WSASetLastError(int error)
{
    g_wsa_last_error = error;
}

u_long WINAPI htonl(u_long value)
{
    return __builtin_bswap32(value);
}

u_short WINAPI htons(u_short value)
{
    return __builtin_bswap16(value);
}

u_long WINAPI ntohl(u_long value)
{
    return __builtin_bswap32(value);
}

u_short WINAPI ntohs(u_short value)
{
    return __builtin_bswap16(value);
}

/* Creating a socket is local and succeeds, so endpoint code keeps its normal
 * bookkeeping; only IPv4 UDP, VDP and TCP exist on the console. */
SOCKET WINAPI socket(int family, int type, int protocol)
{
    if (g_wsa_startup_count <= 0) {
        g_wsa_last_error = WSANOTINITIALISED;
        return INVALID_SOCKET;
    }
    if (family != AF_INET) {
        g_wsa_last_error = k_wsaeafnosupport;
        return INVALID_SOCKET;
    }
    const bool datagram = type == SOCK_DGRAM &&
        (protocol == 0 || protocol == IPPROTO_UDP || protocol == IPPROTO_VDP);
    const bool stream = type == SOCK_STREAM && (protocol == 0 || protocol == IPPROTO_TCP);
    if (!datagram && !stream) {
        g_wsa_last_error = k_wsaeprotonosupport;
        return INVALID_SOCKET;
    }
    for (int i = 0; i < k_socket_count; i++) {
        s_offline_socket *entry = &g_sockets[i];
        if (entry->used)
            continue;
        memset(entry, 0, sizeof(*entry));
        entry->used = true;
        entry->type = type;
        entry->protocol = protocol;
        const int receive_k = g_xnet_parameters.cfgSockDefaultRecvBufsizeInK;
        const int send_k = g_xnet_parameters.cfgSockDefaultSendBufsizeInK;
        entry->receive_buffer = (receive_k ? receive_k : 16) * 1024;
        entry->send_buffer = (send_k ? send_k : 16) * 1024;
        return k_socket_base + i;
    }
    g_wsa_last_error = WSAEMFILE;
    return INVALID_SOCKET;
}

int WINAPI closesocket(SOCKET s)
{
    s_offline_socket *entry = socket_check(s);
    if (!entry)
        return SOCKET_ERROR;
    entry->used = false;
    return 0;
}

int WINAPI bind(SOCKET s, const struct sockaddr *address, int address_length)
{
    s_offline_socket *entry = socket_check(s);
    if (!entry)
        return SOCKET_ERROR;
    if (!address || address_length < (int)sizeof(sockaddr_in))
        return wsa_fail(WSAEFAULT);
    if (entry->bound)
        return wsa_fail(WSAEINVAL);
    entry->bound = true;
    return 0;
}

/* No route exists, so a connection can never start.  Not WSAEWOULDBLOCK:
 * the recovered connect (0xb51b0) would then wait for it forever. */
int WINAPI connect(SOCKET s, const struct sockaddr *address, int address_length)
{
    if (!socket_check(s))
        return SOCKET_ERROR;
    if (!address || address_length < (int)sizeof(sockaddr_in))
        return wsa_fail(WSAEFAULT);
    return wsa_fail(WSAENETUNREACH);
}

int WINAPI shutdown(SOCKET s, int how)
{
    s_offline_socket *entry = socket_check(s);
    if (!entry)
        return SOCKET_ERROR;
    if (how < 0 || how > 2)
        return wsa_fail(WSAEINVAL);
    if (entry->type == SOCK_STREAM)
        return wsa_fail(WSAENOTCONN);
    entry->shut_down = true;
    return 0;
}

/* Nothing ever arrives: datagram sockets report no data (the recovered
 * readers map WSAEWOULDBLOCK to "try later"); a stream was never connected. */
int WINAPI recv(SOCKET s, char *buffer, int length, int flags)
{
    s_offline_socket *entry = socket_check(s);
    if (!entry)
        return SOCKET_ERROR;
    return wsa_fail(entry->type == SOCK_STREAM ? WSAENOTCONN : WSAEWOULDBLOCK);
}

int WINAPI recvfrom(SOCKET s, char *buffer, int length, int flags,
                    struct sockaddr *from, int *from_length)
{
    s_offline_socket *entry = socket_check(s);
    if (!entry)
        return SOCKET_ERROR;
    if (entry->type == SOCK_STREAM)
        return wsa_fail(WSAENOTCONN);
    return wsa_fail(WSAEWOULDBLOCK);
}

int WINAPI send(SOCKET s, const char *buffer, int length, int flags)
{
    s_offline_socket *entry = socket_check(s);
    if (!entry)
        return SOCKET_ERROR;
    return wsa_fail(WSAENOTCONN);
}

/* With the link down a datagram cannot leave the console.  WSAENETDOWN maps
 * to the recovered sender's plain failure (-3), which drops the packet; it
 * avoids WSAEHOSTUNREACH, which would mark the route for key exchange. */
int WINAPI sendto(SOCKET s, const char *buffer, int length, int flags,
                  const struct sockaddr *to, int to_length)
{
    s_offline_socket *entry = socket_check(s);
    if (!entry)
        return SOCKET_ERROR;
    if (entry->type == SOCK_STREAM)
        return wsa_fail(WSAENOTCONN);
    if (entry->shut_down)
        return wsa_fail(WSAESHUTDOWN);
    if (!to || to_length < (int)sizeof(sockaddr_in))
        return wsa_fail(WSAEFAULT);
    return wsa_fail(WSAENETDOWN);
}

/* No socket ever becomes readable, writable or failed: empty every set and
 * report a timeout. */
int WINAPI select(int count, fd_set *read_set, fd_set *write_set, fd_set *error_set,
                  const struct timeval *timeout)
{
    if (g_wsa_startup_count <= 0)
        return wsa_fail(WSANOTINITIALISED);
    if (read_set)
        read_set->fd_count = 0;
    if (write_set)
        write_set->fd_count = 0;
    if (error_set)
        error_set->fd_count = 0;
    return 0;
}

int WINAPI ioctlsocket(SOCKET s, long command, u_long *argument)
{
    s_offline_socket *entry = socket_check(s);
    if (!entry)
        return SOCKET_ERROR;
    if (!argument)
        return wsa_fail(WSAEFAULT);
    if (command == FIONBIO)
        entry->nonblocking = *argument != 0;
    else if (command == k_fionread)
        *argument = 0;
    return 0;
}

int WINAPI getsockopt(SOCKET s, int level, int name, char *value, int *length)
{
    s_offline_socket *entry = socket_check(s);
    if (!entry)
        return SOCKET_ERROR;
    if (!value || !length || *length < (int)sizeof(int))
        return wsa_fail(WSAEFAULT);
    int result = 0;
    if (int *option = socket_option(entry, level, name))
        result = *option;
    else if (level == SOL_SOCKET && name == SO_TYPE)
        result = entry->type;
    /* SO_ERROR and anything unmodelled read as zero. */
    memcpy(value, &result, sizeof(result));
    *length = sizeof(result);
    return 0;
}

int WINAPI setsockopt(SOCKET s, int level, int name, const char *value, int length)
{
    s_offline_socket *entry = socket_check(s);
    if (!entry)
        return SOCKET_ERROR;
    if (!value && length > 0)
        return wsa_fail(WSAEFAULT);
    if (int *option = socket_option(entry, level, name)) {
        if (length < (int)sizeof(int))
            return wsa_fail(WSAEFAULT);
        memcpy(option, value, sizeof(int));
    }
    return 0;
}

/* ---- XNet ---------------------------------------------------------------- */

int WINAPI XNetStartup(const XNetStartupParams *parameters)
{
    memset(&g_xnet_parameters, 0, sizeof(g_xnet_parameters));
    if (parameters)
        g_xnet_parameters = *parameters;
    g_xnet_started = true;
    return 0;
}

int WINAPI XNetCleanup(void)
{
    if (!g_xnet_started)
        return WSANOTINITIALISED;
    g_xnet_started = false;
    return 0;
}

/* random bytes for keys and nonces: a xorshift seeded from the clock is
 * plenty with no network to protect */
int WINAPI XNetRandom(BYTE *buffer, UINT count)
{
    static unsigned long long state;
    if (!state)
        state = 0x9e3779b97f4a7c15ull ^ host_h2_performance_counter();
    for (UINT index = 0; index < count; ++index) {
        state ^= state << 13;
        state ^= state >> 7;
        state ^= state << 17;
        buffer[index] = static_cast<BYTE>(state >> 24);
    }
    return 0;
}

/* No cable: XNET_ETHERNET_LINK_ACTIVE clear.  The recovered transport never
 * starts and its link-up transition never fires. */
DWORD WINAPI XNetGetEthernetLinkStatus(void)
{
    return 0;
}

/* Without a link the console never acquires an address; NONE (not PENDING)
 * tells the recovered address cache (0x7a9b0) that none is coming. */
DWORD WINAPI XNetGetTitleXnAddr(XNADDR *address)
{
    if (address)
        memset(address, 0, sizeof(*address));
    return XNET_GET_XNADDR_NONE;
}

/* Key generation and registration are local operations. */
int WINAPI XNetCreateKey(XNKID *key_id, XNKEY *key)
{
    if (!g_xnet_started)
        return WSANOTINITIALISED;
    if (!key_id || !key)
        return WSAEFAULT;
    random_bytes(key_id->ab, sizeof(key_id->ab));
    random_bytes(key->ab, sizeof(key->ab));
    return 0;
}

int WINAPI XNetRegisterKey(const XNKID *key_id, const XNKEY *key)
{
    if (!g_xnet_started)
        return WSANOTINITIALISED;
    return key_id && key ? 0 : WSAEFAULT;
}

int WINAPI XNetUnregisterKey(const XNKID *key_id)
{
    if (!g_xnet_started)
        return WSANOTINITIALISED;
    return key_id ? 0 : WSAEFAULT;
}

/* Mapping a peer to a secure address needs the network. */
int WINAPI XNetXnAddrToInAddr(const XNADDR *address, const XNKID *key_id, IN_ADDR *in_address)
{
    if (in_address)
        in_address->s_addr = 0;
    return g_xnet_started ? WSAENETDOWN : WSANOTINITIALISED;
}

int WINAPI XNetInAddrToXnAddr(IN_ADDR in_address, XNADDR *address, XNKID *key_id)
{
    if (address)
        memset(address, 0, sizeof(*address));
    if (key_id)
        memset(key_id, 0, sizeof(*key_id));
    return g_xnet_started ? WSAEINVAL : WSANOTINITIALISED;
}

int WINAPI XNetTsAddrToInAddr(const TSADDR *ts_address, DWORD service_id,
                              const XNKID *key_id, IN_ADDR *in_address)
{
    if (in_address)
        in_address->s_addr = 0;
    return g_xnet_started ? WSAENETDOWN : WSANOTINITIALISED;
}

int WINAPI XNetConnect(IN_ADDR in_address)
{
    return g_xnet_started ? WSAENETDOWN : WSANOTINITIALISED;
}

/* The connection to any address is terminally lost, never pending. */
DWORD WINAPI XNetGetConnectStatus(IN_ADDR in_address)
{
    return k_xnet_connect_status_lost;
}

/* Listening registers local QoS reply data and succeeds; probes cannot run
 * and return no result block, so the recovered pool (0x7b4c0) allocates none. */
int WINAPI XNetQosListen(const XNKID *key_id, const BYTE *data,
                         DWORD data_size, DWORD bits_per_second, DWORD flags)
{
    if (!g_xnet_started)
        return WSANOTINITIALISED;
    return key_id ? 0 : WSAEFAULT;
}

int WINAPI XNetQosLookup(UINT address_count, const XNADDR *addresses[],
                         const XNKID *key_ids[], const XNKEY *keys[],
                         UINT in_address_count, const IN_ADDR in_addresses[],
                         const DWORD service_ids[], UINT probe_count,
                         DWORD bits_per_second, DWORD flags, HANDLE event,
                         XNQOS **qos)
{
    if (qos)
        *qos = 0;
    return g_xnet_started ? WSAENETDOWN : WSANOTINITIALISED;
}

int WINAPI XNetQosServiceLookup(DWORD flags, HANDLE event, XNQOS **qos)
{
    if (qos)
        *qos = 0;
    return g_xnet_started ? WSAENETDOWN : WSANOTINITIALISED;
}

int WINAPI XNetQosRelease(XNQOS *qos)
{
    return qos ? 0 : WSAEINVAL;
}

/* ---- downloadable content: none on this console --------------------------- */

HANDLE WINAPI XFindFirstContent(LPCSTR root, DWORD title_id, XCONTENT_FIND_DATA *data)
{
    /* ERROR_FILE_NOT_FOUND is the recovered enumerator's (0x8d5a0) "nothing
     * installed", which it does not count as a failure. */
    SetLastError(ERROR_FILE_NOT_FOUND);
    return INVALID_HANDLE_VALUE;
}

BOOL WINAPI XFindNextContent(HANDLE find, XCONTENT_FIND_DATA *data)
{
    SetLastError(ERROR_NO_MORE_FILES);
    return FALSE;
}

/* ---- Xbox Live: startup, logon and users ----------------------------------- */

HRESULT __stdcall XOnlineStartup(const XONLINE_STARTUP_PARAMS *parameters)
{
    g_xonline_started = true;
    return S_OK;
}

HRESULT __stdcall XOnlineCleanup(void)
{
    g_xonline_started = false;
    return S_OK;
}

/* A logon is accepted as a task and then fails: the recovered logon status
 * (0x6cd50) maps the result to its generic failure (10) and the logon
 * updater (0x6c2a0) finishes the task as failed. */
HRESULT __stdcall XOnlineLogon(const XONLINE_USER *users, const DWORD *service_ids,
                               DWORD service_count, HANDLE event, PXONLINETASK_HANDLE task)
{
    if (!task)
        return k_e_invalidarg;
    if (!g_xonline_started)
        return task_refuse(task);
    *task = (XONLINETASK_HANDLE)&g_logon_task;
    return S_OK;
}

HRESULT __stdcall XOnlineLogonTaskGetResults(XONLINETASK_HANDLE task)
{
    return k_online_e_no_network;
}

/* Any task (only the logon can exist) is complete and failed. */
HRESULT __stdcall XOnlineTaskContinue(XONLINETASK_HANDLE task)
{
    return task ? k_online_e_no_network : k_e_invalidarg;
}

HRESULT __stdcall XOnlineTaskClose(XONLINETASK_HANDLE task)
{
    return S_OK;
}

HRESULT __stdcall XOnlineChangeLogonUsers(const XONLINE_USER *users, HANDLE event,
                                          PXONLINETASK_HANDLE task)
{
    return task_refuse(task);
}

HRESULT __stdcall XOnlineChangeLogonUsersTaskGetResults(XONLINETASK_HANDLE task, HRESULT *results)
{
    if (results)
        for (int i = 0; i < XONLINE_MAX_LOGON_USERS; i++)
            results[i] = k_online_e_no_network;
    return k_online_e_no_network;
}

/* Nobody is logged on; every recovered caller accepts null. */
XONLINE_USER *__stdcall XOnlineGetLogonUsers(void)
{
    return 0;
}

/* Enumerating the hard-disk accounts is local and succeeds: the native port
 * has none. */
HRESULT __stdcall XOnlineGetUsers(XONLINE_USER *users, DWORD *count)
{
    if (!count)
        return k_e_invalidarg;
    *count = 0;
    return S_OK;
}

HRESULT __stdcall XOnlineGetServiceInfo(DWORD service_id, XONLINE_SERVICE_INFO *info)
{
    if (info)
        memset(info, 0, sizeof(*info));
    return k_online_e_no_network;
}

/* Not one of the recovered open/moderate/strict values (0x6b550): unknown. */
DWORD __stdcall XOnlineGetNatType(void)
{
    return 0;
}

HRESULT __stdcall XOnlineSaveLogonState(XONLINE_LOGON_STATE *state)
{
    return k_online_e_no_network;
}

BOOL __stdcall XOnlineTitleIdIsSameTitle(DWORD title_id)
{
    return title_id == k_title_id;
}

/* Identity comparisons are local: same account and same guest number. */
BOOL __stdcall XOnlineAreUsersIdentical(const XUID *first, const XUID *second)
{
    if (!first || !second)
        return FALSE;
    return first->qwUserID == second->qwUserID &&
        XOnlineUserGuestNumber(first->dwUserFlags) == XOnlineUserGuestNumber(second->dwUserFlags);
}

/* The guest number lives in the two bits XOnlineIsUserGuest tests. */
DWORD __stdcall XOnlineUserGuestNumber(DWORD user_flags)
{
    return user_flags & 3;
}

BOOL __stdcall XOnlineGetNotificationEx(DWORD user_index, XONLINE_NOTIFICATION_EX_INFO *info,
                                        DWORD *state_flags)
{
    return FALSE;
}

HRESULT __stdcall XOnlineNotificationSetState(DWORD user_index, DWORD state_flags, XNKID session_id,
                                              DWORD state_data_size, const BYTE *state_data)
{
    return k_online_e_no_network;
}

HRESULT __stdcall XOnlineFeedbackSend(DWORD user_index, XUID user, XONLINE_FEEDBACK_TYPE type,
                                      const XONLINE_FEEDBACK_PARAMS *parameters, HANDLE event,
                                      PXONLINETASK_HANDLE task)
{
    return task_refuse(task);
}

HRESULT __stdcall XOnlineOfferingIsNewContentAvailable(DWORD categories, HANDLE event,
                                                       PXONLINETASK_HANDLE task)
{
    return task_refuse(task);
}

/* ---- friends, invites, mute list --------------------------------------------- */

HRESULT __stdcall XOnlineFriendsStartup(HANDLE event, PXONLINETASK_HANDLE task)
{
    return task_refuse(task);
}

HRESULT __stdcall XOnlineFriendsEnumerate(DWORD user_index, HANDLE event, PXONLINETASK_HANDLE task)
{
    return task_refuse(task);
}

HRESULT __stdcall XOnlineFriendsEnumerateFinish(XONLINETASK_HANDLE task)
{
    return k_online_e_no_network;
}

DWORD __stdcall XOnlineFriendsGetLatest(DWORD user_index, DWORD capacity, XONLINE_FRIEND *friends)
{
    return 0;
}

HRESULT __stdcall XOnlineFriendsRemove(DWORD user_index, const XONLINE_FRIEND *friend_)
{
    return k_online_e_no_network;
}

HRESULT __stdcall XOnlineFriendsAnswerRequest(DWORD user_index, const XONLINE_FRIEND *friend_,
                                              XONLINE_REQUEST_ANSWER_TYPE answer)
{
    return k_online_e_no_network;
}

HRESULT __stdcall XOnlineFriendsGameInvite(DWORD user_index, XNKID session_id, DWORD friend_count,
                                           const XONLINE_FRIEND *friends)
{
    return k_online_e_no_network;
}

HRESULT __stdcall XOnlineFriendsAnswerGameInvite(DWORD user_index, const XONLINE_FRIEND *friend_,
                                                 XONLINE_GAMEINVITE_ANSWER_TYPE answer)
{
    return k_online_e_no_network;
}

HRESULT __stdcall XOnlineFriendsJoinGame(DWORD user_index, const XONLINE_FRIEND *friend_)
{
    return k_online_e_no_network;
}

/* Failure makes the recovered caller (0x6b590) show "?" for the title. */
HRESULT __stdcall XOnlineFriendsGetTitleName(DWORD title_id, DWORD language, DWORD name_length,
                                             WCHAR *name)
{
    return k_online_e_no_network;
}

/* No invitation was accepted from the dashboard; the press-start handler only
 * acts on success. */
HRESULT __stdcall XOnlineFriendsGetAcceptedGameInvite(XONLINE_ACCEPTED_GAMEINVITE *invite)
{
    if (invite)
        memset(invite, 0, sizeof(*invite));
    return E_FAIL;
}

HRESULT __stdcall XOnlineFriendsRequestEx(DWORD user_index, XUID user, XONLINE_MSG_HANDLE message,
                                          HANDLE event, PXONLINETASK_HANDLE task)
{
    return task_refuse(task);
}

HRESULT __stdcall XOnlineFriendsRequestByNameEx(DWORD user_index, const char *gamertag,
                                                XONLINE_MSG_HANDLE message, HANDLE event,
                                                PXONLINETASK_HANDLE task)
{
    return task_refuse(task);
}

HRESULT __stdcall XOnlineGameInviteAnswer(DWORD user_index, const XONLINE_GAMEINVITE_ANSWER_INFO *info,
                                          XONLINE_PEER_ANSWER_TYPE answer, HANDLE event,
                                          PXONLINETASK_HANDLE task)
{
    return task_refuse(task);
}

HRESULT __stdcall XOnlineGameInviteSend(DWORD user_index, DWORD recipient_count,
                                        const XUID *recipients, XNKID session_id, DWORD flags,
                                        XONLINE_MSG_HANDLE message, HANDLE event,
                                        PXONLINETASK_HANDLE task)
{
    return task_refuse(task);
}

HRESULT __stdcall XOnlineMutelistStartup(void *event, PXONLINETASK_HANDLE task)
{
    return task_refuse(task);
}

/* The caller's count stays NONE ("not loaded") on failure. */
HRESULT __stdcall XOnlineMutelistGet(DWORD controller, DWORD capacity, void *event,
                                     PXONLINETASK_HANDLE task,
                                     XONLINE_MUTELISTUSER *users, DWORD *count)
{
    return task_refuse(task);
}

HRESULT __stdcall XOnlineMutelistAdd(DWORD controller, XUID user)
{
    return k_online_e_no_network;
}

HRESULT __stdcall XOnlineMutelistRemove(DWORD controller, XUID user)
{
    return k_online_e_no_network;
}

/* ---- presence ------------------------------------------------------------------- */

HRESULT __stdcall XOnlinePresenceInit(DWORD user_index, HANDLE event, PXONLINETASK_HANDLE task)
{
    return task_refuse(task);
}

HRESULT __stdcall XOnlinePresenceAdd(XONLINETASK_HANDLE task, DWORD group_id, DWORD user_count,
                                     const XUID *users)
{
    return k_online_e_no_network;
}

HRESULT __stdcall XOnlinePresenceSubmit(XONLINETASK_HANDLE task)
{
    return k_online_e_no_network;
}

HRESULT __stdcall XOnlinePresenceGetLatest(XONLINETASK_HANDLE task, DWORD group_id, DWORD count,
                                           XONLINE_PRESENCE *presences)
{
    return k_online_e_no_network;
}

HRESULT __stdcall XOnlinePresenceClear(XONLINETASK_HANDLE task)
{
    return k_online_e_no_network;
}

/* ---- messages --------------------------------------------------------------- */

HRESULT __stdcall XOnlineMessageEnumerate(DWORD user_index, XONLINE_MSG_SUMMARY *summaries,
                                          DWORD *count)
{
    if (count)
        *count = 0;
    return k_online_e_no_network;
}

HRESULT __stdcall XOnlineMessageDetails(DWORD user_index, DWORD message_id, DWORD flags_to_set,
                                        DWORD flags_to_clear, HANDLE event,
                                        PXONLINETASK_HANDLE task)
{
    return task_refuse(task);
}

HRESULT __stdcall XOnlineMessageDetailsGetResultsProperty(XONLINETASK_HANDLE task, WORD tag,
                                                          DWORD size, void *buffer,
                                                          DWORD *required_size, void *reserved)
{
    return k_online_e_no_network;
}

HRESULT __stdcall XOnlineMessageDownloadAttachmentToMemory(XONLINETASK_HANDLE details_task, WORD tag,
                                                           BYTE *buffer, DWORD size, HANDLE event,
                                                           PXONLINETASK_HANDLE task)
{
    return task_refuse(task);
}

HRESULT __stdcall XOnlineMessageDownloadAttachmentToMemoryGetResults(XONLINETASK_HANDLE task,
                                                                     BYTE **data,
                                                                     DWORD *received_size,
                                                                     DWORD *total_size)
{
    return k_online_e_no_network;
}

HRESULT __stdcall XOnlineMessageDelete(DWORD user_index, DWORD message_id, BOOL block_sender)
{
    return k_online_e_no_network;
}

/* No message object is built: the recovered builder (0x8f070) clears its
 * handle on failure and never sends. */
HRESULT __stdcall XOnlineMessageCreate(DWORD message_type, DWORD property_count,
                                       DWORD attachment_size, ULONGLONG context, DWORD flags,
                                       DWORD expire_minutes, XONLINE_MSG_HANDLE *message)
{
    if (message)
        *message = 0;
    return k_online_e_no_network;
}

HRESULT __stdcall XOnlineMessageSetProperty(XONLINE_MSG_HANDLE message, WORD tag, DWORD size,
                                            const void *value, DWORD flags)
{
    return k_online_e_no_network;
}

HRESULT __stdcall XOnlineMessageSend(DWORD user_index, XONLINE_MSG_HANDLE message,
                                     DWORD recipient_count, const XUID *recipients,
                                     HANDLE event, PXONLINETASK_HANDLE task)
{
    return task_refuse(task);
}

HRESULT __stdcall XOnlineMessageDestroy(XONLINE_MSG_HANDLE message)
{
    return S_OK;
}

/* ---- matchmaking and title-server queries -------------------------------------- */

/* PROVISIONAL sizing, a pure computation: per result a pointer, the fixed
 * record, and each attribute's record plus its declared payload. Nothing is
 * ever written into the buffer offline. */
DWORD __stdcall XOnlineMatchSearchResultsLen(DWORD result_count, DWORD attribute_count,
                                             const XONLINE_ATTRIBUTE_SPEC *specs)
{
    DWORD per_result = sizeof(PXONLINE_MATCH_SEARCHRESULT) + sizeof(XONLINE_MATCH_SEARCHRESULT);
    for (DWORD i = 0; i < attribute_count; i++)
        per_result += sizeof(XONLINE_ATTRIBUTE) + (specs ? specs[i].length : 0);
    return result_count * per_result;
}

HRESULT __stdcall XOnlineMatchSearch(DWORD procedure, DWORD maximum_results,
                                     DWORD attribute_count, const XONLINE_ATTRIBUTE *attributes,
                                     DWORD results_size, HANDLE event, PXONLINETASK_HANDLE task)
{
    return task_refuse(task);
}

HRESULT __stdcall XOnlineMatchSearchGetResults(XONLINETASK_HANDLE task,
                                               PXONLINE_MATCH_SEARCHRESULT **results,
                                               DWORD *count)
{
    if (results)
        *results = 0;
    if (count)
        *count = 0;
    return k_online_e_no_network;
}

HRESULT __stdcall XOnlineMatchSearchParse(const XONLINE_MATCH_SEARCHRESULT *result,
                                          DWORD spec_count, const XONLINE_ATTRIBUTE_SPEC *specs,
                                          void *values)
{
    return k_online_e_no_network;
}

HRESULT __stdcall XOnlineMatchSessionCreate(DWORD public_filled, DWORD public_open,
                                            DWORD private_filled, DWORD private_open,
                                            DWORD attribute_count,
                                            const XONLINE_ATTRIBUTE *attributes,
                                            HANDLE event, PXONLINETASK_HANDLE task)
{
    return task_refuse(task);
}

HRESULT __stdcall XOnlineMatchSessionUpdate(XNKID session_id, DWORD public_filled,
                                            DWORD public_open, DWORD private_filled,
                                            DWORD private_open, DWORD attribute_count,
                                            const XONLINE_ATTRIBUTE *attributes,
                                            HANDLE event, PXONLINETASK_HANDLE task)
{
    return task_refuse(task);
}

HRESULT __stdcall XOnlineMatchSessionGetInfo(XONLINETASK_HANDLE task, XNKID *session_id, XNKEY *key)
{
    return k_online_e_no_network;
}

HRESULT __stdcall XOnlineMatchSessionDelete(XNKID session_id, HANDLE event, PXONLINETASK_HANDLE task)
{
    return task_refuse(task);
}

HRESULT __stdcall XOnlineMatchSessionFindFromID(XNKID session_id, HANDLE event,
                                                PXONLINETASK_HANDLE task)
{
    return task_refuse(task);
}

/* Not in the shim; declared here as the recovered caller (0xb37f0) does. */
long __stdcall XOnlineQuerySearch(DWORD procedure, DWORD query, DWORD flags,
                                  DWORD maximum_results, DWORD attribute_count,
                                  const void *attributes, DWORD parameter_count,
                                  const void *parameters, void *event, void **task)
{
    if (task)
        *task = 0;
    return k_online_e_no_network;
}

long __stdcall XOnlineQuerySearchGetResults(void *task, DWORD *returned_size, DWORD *count,
                                            DWORD *buffer_size, void *buffer)
{
    if (returned_size)
        *returned_size = 0;
    return k_online_e_no_network;
}

/* ---- statistics and arbitration ------------------------------------------------- */

HRESULT __stdcall XOnlineStatRead(DWORD count, XONLINE_STAT_SPEC *specs, HANDLE event,
                                  PXONLINETASK_HANDLE task)
{
    return task_refuse(task);
}

HRESULT __stdcall XOnlineStatReadGetResult(XONLINETASK_HANDLE task, DWORD count,
                                           XONLINE_STAT_SPEC *specs, DWORD extra_size,
                                           BYTE *extra_buffer)
{
    return k_online_e_no_network;
}

HRESULT __stdcall XOnlineStatWrite(DWORD count, const XONLINE_STAT_SPEC *specs, HANDLE event,
                                   PXONLINETASK_HANDLE task)
{
    return task_refuse(task);
}

HRESULT __stdcall XOnlineStatWriteGetResult(XONLINETASK_HANDLE task, HANDLE *result,
                                            PXONLINE_STAT_ATTACHMENT_REFERENCE *references,
                                            DWORD *count)
{
    return k_online_e_no_network;
}

HRESULT __stdcall XOnlineArbitrationCreateRoundID(ULONGLONG *round_id)
{
    return k_online_e_no_network;
}

HRESULT __stdcall XOnlineArbitrationRegister(const XONLINE_ARB_ID *id, DWORD seconds, DWORD flags,
                                             HANDLE event, PXONLINETASK_HANDLE task)
{
    return task_refuse(task);
}

HRESULT __stdcall XOnlineArbitrationReport(const XONLINE_ARB_ID *id, DWORD procedure_count,
                                           const XONLINE_STAT_PROC *procedures,
                                           const void *attachments, DWORD flags, HANDLE event,
                                           PXONLINETASK_HANDLE task)
{
    return task_refuse(task);
}

HRESULT __stdcall XOnlineArbitrationExtendRound(const XONLINE_ARB_ID *id, DWORD seconds,
                                                HANDLE event, PXONLINETASK_HANDLE task)
{
    return task_refuse(task);
}

/* ---- storage ---------------------------------------------------------------------- */

HRESULT __stdcall XOnlineStorageCreateServerPath(DWORD facility, ULONGLONG user_id,
                                                 ULONGLONG team_id, const WCHAR *path,
                                                 WCHAR *server_path, DWORD *server_path_size)
{
    if (server_path && server_path_size && *server_path_size)
        server_path[0] = 0;
    return k_online_e_no_network;
}

HRESULT __stdcall XOnlineStorageDownload(DWORD facility, DWORD user_index, const WCHAR *server_path,
                                         const char *local_path, DWORD flags, HANDLE event,
                                         PXONLINETASK_HANDLE task)
{
    return task_refuse(task);
}

HRESULT __stdcall XOnlineStorageDownloadToMemory(DWORD facility, DWORD user_index,
                                                 const WCHAR *server_path, BYTE *buffer,
                                                 DWORD size, DWORD flags, HANDLE event,
                                                 PXONLINETASK_HANDLE task)
{
    return task_refuse(task);
}

HRESULT __stdcall XOnlineStorageDownloadToMemoryGetResults(XONLINETASK_HANDLE task, BYTE **data,
                                                           DWORD *received_size,
                                                           DWORD *total_size,
                                                           ULONGLONG *owner,
                                                           FILETIME *creation_time)
{
    if (data)
        *data = 0;
    if (received_size)
        *received_size = 0;
    if (total_size)
        *total_size = 0;
    return k_online_e_no_network;
}

HRESULT __stdcall XOnlineStorageUploadByServerPath(DWORD facility, DWORD user_index,
                                                   const WCHAR *server_path, FILETIME expiration,
                                                   const char *local_path, DWORD flags,
                                                   HANDLE event, PXONLINETASK_HANDLE task)
{
    return task_refuse(task);
}

HRESULT __stdcall XOnlineStorageUploadFromMemory(DWORD facility, DWORD user_index,
                                                 const WCHAR *server_path, FILETIME expiration,
                                                 const BYTE *buffer, DWORD size, DWORD flags,
                                                 HANDLE event, PXONLINETASK_HANDLE task)
{
    return task_refuse(task);
}

/* ---- teams ------------------------------------------------------------------------- */

HRESULT __stdcall XOnlineTeamCreate(DWORD user_index, const XONLINE_TEAM_PROPERTIES *properties,
                                    const XONLINE_TEAM_MEMBER_PROPERTIES *creator,
                                    DWORD maximum_members, HANDLE event, PXONLINETASK_HANDLE task)
{
    return task_refuse(task);
}

HRESULT __stdcall XOnlineTeamDelete(DWORD user_index, XUID team, HANDLE event,
                                    PXONLINETASK_HANDLE task)
{
    return task_refuse(task);
}

HRESULT __stdcall XOnlineTeamEnumerateByUserXUID(DWORD user_index, XUID user, HANDLE event,
                                                 PXONLINETASK_HANDLE task)
{
    return task_refuse(task);
}

HRESULT __stdcall XOnlineTeamEnumerateGetResults(XONLINETASK_HANDLE task, DWORD *count, XUID *teams)
{
    if (count)
        *count = 0;
    return k_online_e_no_network;
}

HRESULT __stdcall XOnlineTeamGetDetails(XONLINETASK_HANDLE task, XUID team, XONLINE_TEAM *details)
{
    if (details)
        memset(details, 0, sizeof(*details));
    return k_online_e_no_network;
}

HRESULT __stdcall XOnlineTeamMembersEnumerate(DWORD user_index, XUID team, BOOL with_recruits,
                                              HANDLE event, PXONLINETASK_HANDLE task)
{
    return task_refuse(task);
}

HRESULT __stdcall XOnlineTeamMembersEnumerateGetResults(XONLINETASK_HANDLE task, DWORD *count,
                                                        XUID *members)
{
    if (count)
        *count = 0;
    return k_online_e_no_network;
}

HRESULT __stdcall XOnlineTeamMemberGetDetails(XONLINETASK_HANDLE task, XUID member,
                                              XONLINE_TEAM_MEMBER *details)
{
    if (details)
        memset(details, 0, sizeof(*details));
    return k_online_e_no_network;
}

HRESULT __stdcall XOnlineTeamMemberSetProperties(DWORD user_index, XUID team, XUID member,
                                                 const XONLINE_TEAM_MEMBER_PROPERTIES *properties,
                                                 HANDLE event, PXONLINETASK_HANDLE task)
{
    return task_refuse(task);
}

HRESULT __stdcall XOnlineTeamMemberRemove(DWORD user_index, XUID team, XUID member, HANDLE event,
                                          PXONLINETASK_HANDLE task)
{
    return task_refuse(task);
}

HRESULT __stdcall XOnlineTeamMemberAnswerRecruit(DWORD user_index, XUID team,
                                                 XONLINE_PEER_ANSWER_TYPE answer, HANDLE event,
                                                 PXONLINETASK_HANDLE task)
{
    return task_refuse(task);
}

HRESULT __stdcall XOnlineTeamMemberRecruit(DWORD user_index, XUID team, XUID user,
                                           const XONLINE_TEAM_MEMBER_PROPERTIES *properties,
                                           XONLINE_MSG_HANDLE message, HANDLE event,
                                           PXONLINETASK_HANDLE task)
{
    return task_refuse(task);
}

HRESULT __stdcall XOnlineTeamMemberRecruitByName(DWORD user_index, XUID team, const char *gamertag,
                                                 const XONLINE_TEAM_MEMBER_PROPERTIES *properties,
                                                 XONLINE_MSG_HANDLE message, HANDLE event,
                                                 PXONLINETASK_HANDLE task)
{
    return task_refuse(task);
}

} // extern "C"
