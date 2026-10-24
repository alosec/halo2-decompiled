#ifndef H2_PORT_XDK_XTL_H
#define H2_PORT_XDK_XTL_H

/* Clean native-port declarations limited to public Win32-compatible facts and
 * APIs visible in the recovered source. This is not copied from an Xbox SDK. */

typedef void* HANDLE;
typedef int BOOL;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef unsigned long long ULONGLONG;
typedef long LONG;
typedef long HRESULT;

#ifdef H2_MS_ABI_FRONTEND
typedef unsigned short wchar_t;
typedef wchar_t WCHAR;
#endif

struct _FILETIME {
    DWORD dwLowDateTime;
    DWORD dwHighDateTime;
};
typedef struct _FILETIME FILETIME;

typedef struct _SYSTEMTIME {
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
} SYSTEMTIME;

typedef union _ULARGE_INTEGER {
    struct {
        DWORD LowPart;
        DWORD HighPart;
    };
    unsigned long long QuadPart;
} ULARGE_INTEGER;

/* Independently recovered from the exact field offsets and record strides used
 * by the networking and preferences code. */
#pragma pack(push, 4)
struct XCALCSIG_SIGNATURE {
    BYTE bytes[0x14];
};

struct XNKID {
    BYTE ab[8];
};

struct XNKEY {
    BYTE ab[16];
};

struct IN_ADDR {
    DWORD s_addr;
};

struct XNADDR {
    IN_ADDR ina;
    IN_ADDR inaOnline;
    WORD wPortOnline;
    BYTE abEnet[6];
    BYTE abOnline[20];
};
#pragma pack(pop)

#ifdef __cplusplus
static_assert(sizeof(ULARGE_INTEGER) == 8, "public Win32 integer layout");
static_assert(sizeof(SYSTEMTIME) == 16, "public Win32 system-time layout");
static_assert(sizeof(XCALCSIG_SIGNATURE) == 0x14, "retail signature layout");
static_assert(sizeof(XNKID) == 0x08, "retail key identifier layout");
static_assert(sizeof(XNKEY) == 0x10, "retail key layout");
static_assert(sizeof(XNADDR) == 0x24, "retail network address layout");
static_assert(__builtin_offsetof(XNADDR, wPortOnline) == 0x08, "retail network port offset");
static_assert(__builtin_offsetof(XNADDR, abEnet) == 0x0a, "retail ethernet address offset");
static_assert(__builtin_offsetof(XNADDR, abOnline) == 0x10, "retail online address offset");
#endif

#define FALSE 0
#define TRUE 1
#define S_OK ((HRESULT)0)
#define E_FAIL ((HRESULT)0x80004005L)
#define SUCCEEDED(result) ((HRESULT)(result) >= 0)
#define FAILED(result) ((HRESULT)(result) < 0)
#define PAGE_READONLY 0x02
#define PAGE_READWRITE 0x04
#define PAGE_WRITECOMBINE 0x400
#define MEM_COMMIT 0x1000
#define MEM_RELEASE 0x8000
#define FILE_BEGIN 0
#define INVALID_SET_FILE_POINTER ((DWORD)-1)
#define INFINITE ((DWORD)-1)
#define INVALID_HANDLE_VALUE ((HANDLE)(long)-1)
#define GENERIC_READ 0x80000000ul
#define GENERIC_WRITE 0x40000000ul
#define FILE_SHARE_READ 0x00000001ul
#define FILE_SHARE_WRITE 0x00000002ul
#define CREATE_ALWAYS 2
#define OPEN_EXISTING 3
#define OPEN_ALWAYS 4
#define FILE_ATTRIBUTE_NORMAL 0x00000080ul
#define FILE_FLAG_NO_BUFFERING 0x20000000ul
#define FILE_FLAG_OVERLAPPED 0x40000000ul
#define MEM_RESERVE 0x2000
#define MEM_DECOMMIT 0x4000
#define MEM_TOP_DOWN 0x100000
#define ERROR_SUCCESS 0
#define ERROR_FILE_NOT_FOUND 2
#define ERROR_IO_PENDING 997
#define WINAPI __stdcall
/* Public Xbox network QoS listener flags used by recovered session code. */
#define XNET_QOS_LISTEN_ENABLE 0x01
#define XNET_QOS_LISTEN_RELEASE 0x10

#ifdef H2_PORT_SHIMS
/* Minimal native declarations for the recovered Xbox immediate-mode calls.
 * Retail function_22a664 passes primitive value 7 for TRIANGLEFAN; the native
 * boundary keeps the vertex register explicit instead of relying on the Xbox
 * compiler intrinsic. */
#define D3DPT_TRIANGLELIST 5 /* the NV2A begin code; see d3d8.h */
#define D3DPT_TRIANGLEFAN 7
#define D3DVSDE_VERTEX 0
#endif

#ifdef __cplusplus
extern "C" {
#endif

int __cdecl abs(int value);
#ifdef H2_PORT_SHIMS
typedef struct D3DResource D3DResource;
DWORD __stdcall D3DResource_Release(D3DResource *resource);

void __stdcall D3DDevice_Begin(DWORD primitive_type);
void __stdcall D3DDevice_End(void);
void __stdcall D3DDevice_SetVertexData2f(int vertex_register, float x, float y);
void __stdcall D3DDevice_SetVertexShaderConstant(int register_index, void const *constant_data, DWORD constant_count);
void __stdcall D3DDevice_SetVertexData4f(DWORD vertex_register, float x, float y,
                                         float z, float w);
void __stdcall D3DDevice_SetVertexDataColor(DWORD vertex_register, DWORD color);
void __stdcall D3DDevice_DrawIndexedVertices(DWORD primitive_type, DWORD count,
                                               WORD const *indices);
void __stdcall D3DDevice_SetTexture(DWORD stage, void *texture);
#endif
LONG __stdcall CompareFileTime(const FILETIME* first, const FILETIME* second);
BOOL __stdcall GetDiskFreeSpaceExA(const char* root, ULARGE_INTEGER* available,
                                   ULARGE_INTEGER* total, ULARGE_INTEGER* free_bytes);
BOOL WINAPI FileTimeToLocalFileTime(const FILETIME* file_time, FILETIME* local_file_time);
BOOL WINAPI FileTimeToSystemTime(const FILETIME* file_time, SYSTEMTIME* system_time);
void* WINAPI VirtualAlloc(void* address, DWORD size, DWORD allocation_type,
                          DWORD protection);
BOOL WINAPI VirtualFree(void* address, DWORD size, DWORD free_type);
DWORD WINAPI SetFilePointer(HANDLE file, LONG distance, LONG* distance_high, DWORD method);
BOOL WINAPI ReadFile(HANDLE file, void* buffer, DWORD bytes_to_read,
                     DWORD* bytes_read, void* overlapped);
DWORD WINAPI WaitForSingleObject(HANDLE handle, DWORD milliseconds);
BOOL WINAPI ReleaseMutex(HANDLE mutex);
BOOL WINAPI ReleaseSemaphore(HANDLE semaphore, LONG release_count,
                             LONG* previous_count);
typedef DWORD (WINAPI *LPTHREAD_START_ROUTINE)(void* parameter);
HANDLE WINAPI CreateMutexA(void* attributes, BOOL initial_owner, const char* name);
HANDLE WINAPI CreateSemaphoreA(void* attributes, LONG initial_count,
                               LONG maximum_count, const char* name);
HANDLE WINAPI CreateThread(void* attributes, DWORD stack_size,
                           LPTHREAD_START_ROUTINE start, void* parameter,
                           DWORD creation_flags, DWORD* thread_id);
BOOL WINAPI SetThreadPriority(HANDLE thread, int priority);
BOOL WINAPI GetExitCodeThread(HANDLE thread, DWORD* exit_code);
BOOL WINAPI CloseHandle(HANDLE handle);
void __stdcall GetSystemTimeAsFileTime(FILETIME* time);
DWORD __stdcall GetCurrentThreadId(void);
DWORD __stdcall GetTickCount(void);
DWORD __stdcall GetLastError(void);
/* The retail time-stamp counter, in 733.33 MHz ticks (see timing_ticks_to_seconds). */
__int64 h2_native_time_stamp(void);
/* x87 fld/fistp under the default control word: round to nearest, ties to even. */
static inline long h2_native_fistp(double value) { return (long)__builtin_rint(value); }
void __stdcall Sleep(DWORD milliseconds);
BOOL __stdcall SwitchToThread(void);
DWORD __stdcall XCreateSaveGame(const char* root, const wchar_t* name,
                                DWORD disposition, DWORD flags,
                                char* directory, DWORD directory_size);
DWORD __stdcall XDeleteSaveGame(const char* root, const wchar_t* name);
DWORD __stdcall XGetDiskClusterSize(const char* root);
DWORD __stdcall XGetLanguage(void);
int __stdcall XNetQosListen(const XNKID* key_id, const BYTE* data,
                            DWORD data_size, DWORD bits_per_second, DWORD flags);
int __stdcall XNetUnregisterKey(const XNKID* key_id);
void* __stdcall XPhysicalAlloc(DWORD size, DWORD physical_address,
                              DWORD alignment, DWORD protection);
void __stdcall XPhysicalProtect(void* address, DWORD size, DWORD protection);

#ifdef __cplusplus
}
#endif

/* ---- Public Win32 base types and COM helpers ------------------------------
 * Names and values below are documented in the public Win32/COM reference. */
typedef void VOID;
typedef void* PVOID;
typedef void* LPVOID;
typedef const void* LPCVOID;
typedef char CHAR;
typedef short SHORT;
typedef unsigned short USHORT;
typedef int INT;
typedef unsigned int UINT;
typedef unsigned long ULONG;
typedef unsigned long ULONG_PTR;
typedef ULONG_PTR SIZE_T;
typedef BYTE BOOLEAN;
typedef float FLOAT;
typedef BYTE* PBYTE;
typedef BYTE* LPBYTE;
typedef WORD* PWORD;
typedef DWORD* PDWORD;
typedef DWORD* LPDWORD;
typedef LONG* PLONG;
typedef char* LPSTR;
typedef const char* LPCSTR;
#ifdef H2_MS_ABI_FRONTEND
typedef WCHAR* LPWSTR;
typedef const WCHAR* LPCWSTR;
#endif
typedef __int64 LONGLONG;

#ifndef CALLBACK
#define CALLBACK __stdcall
#endif
#ifndef STDMETHODCALLTYPE
#define STDMETHODCALLTYPE __stdcall
#endif
#ifndef STDMETHOD
#define STDMETHOD(method) virtual HRESULT STDMETHODCALLTYPE method
#endif
#ifndef STDMETHOD_
#define STDMETHOD_(type, method) virtual type STDMETHODCALLTYPE method
#endif
#ifndef STDMETHODIMP
#define STDMETHODIMP HRESULT STDMETHODCALLTYPE
#endif
#ifndef STDMETHODIMP_
#define STDMETHODIMP_(type) type STDMETHODCALLTYPE
#endif
#ifndef PURE
#define PURE = 0
#endif

#ifndef C_ASSERT
#define C_ASSERT(expression) typedef char h2_c_assert_##__LINE__[(expression) ? 1 : -1]
#endif

#ifndef MAKEWORD
#define MAKEWORD(low, high) ((WORD)(((BYTE)((DWORD)(low) & 0xff)) | ((WORD)((BYTE)((DWORD)(high) & 0xff))) << 8))
#endif
#ifndef MAKELONG
#define MAKELONG(low, high) ((LONG)(((WORD)((DWORD)(low) & 0xffff)) | ((DWORD)((WORD)((DWORD)(high) & 0xffff))) << 16))
#endif
#ifndef LOWORD
#define LOWORD(value) ((WORD)((DWORD)(value) & 0xffff))
#endif
#ifndef HIWORD
#define HIWORD(value) ((WORD)((DWORD)(value) >> 16))
#endif

#define S_FALSE ((HRESULT)1)
#define E_NOTIMPL ((HRESULT)0x80004001L)
#define E_OUTOFMEMORY ((HRESULT)0x8007000EL)
#define E_INVALIDARG ((HRESULT)0x80070057L)
#define E_PENDING ((HRESULT)0x8000000AL)
#define HRESULT_FROM_WIN32(error) \
    ((HRESULT)(error) <= 0 ? (HRESULT)(error) : (HRESULT)(((error) & 0x0000ffff) | 0x80070000))

/* ---- Public Win32 file, error and memory-status declarations ------------- */
#define MAX_PATH 260
#define CREATE_NEW 1
#define TRUNCATE_EXISTING 5
#define FILE_CURRENT 1
#define FILE_END 2
#define INVALID_FILE_SIZE ((DWORD)0xffffffff)
#define INVALID_FILE_ATTRIBUTES ((DWORD)-1)
#define FILE_SHARE_DELETE 0x00000004ul
#define FILE_ATTRIBUTE_READONLY 0x00000001ul
#define FILE_ATTRIBUTE_HIDDEN 0x00000002ul
#define FILE_ATTRIBUTE_SYSTEM 0x00000004ul
#define FILE_ATTRIBUTE_DIRECTORY 0x00000010ul
#define FILE_ATTRIBUTE_ARCHIVE 0x00000020ul
#define FILE_ATTRIBUTE_TEMPORARY 0x00000100ul
#define FILE_FLAG_WRITE_THROUGH 0x80000000ul
#define FILE_FLAG_RANDOM_ACCESS 0x10000000ul
#define FILE_FLAG_SEQUENTIAL_SCAN 0x08000000ul
#define FILE_FLAG_DELETE_ON_CLOSE 0x04000000ul
#define FILE_FLAG_BACKUP_SEMANTICS 0x02000000ul
#define ERROR_INVALID_FUNCTION 1
#define ERROR_PATH_NOT_FOUND 3
#define ERROR_TOO_MANY_OPEN_FILES 4
#define ERROR_ACCESS_DENIED 5
#define ERROR_INVALID_HANDLE 6
#define ERROR_NOT_ENOUGH_MEMORY 8
#define ERROR_INVALID_DRIVE 15
#define ERROR_NO_MORE_FILES 18
#define ERROR_NOT_READY 21
#define ERROR_SHARING_VIOLATION 32
#define ERROR_HANDLE_EOF 38
#define ERROR_HANDLE_DISK_FULL 39
#define ERROR_FILE_EXISTS 80
#define ERROR_INVALID_PARAMETER 87
#define ERROR_DISK_FULL 112
#define ERROR_INSUFFICIENT_BUFFER 122
#define ERROR_INVALID_NAME 123
#define ERROR_ALREADY_EXISTS 183
#define ERROR_IO_INCOMPLETE 996
#define ERROR_CANCELLED 1223

typedef struct _WIN32_FILE_ATTRIBUTE_DATA {
    DWORD dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD nFileSizeHigh;
    DWORD nFileSizeLow;
} WIN32_FILE_ATTRIBUTE_DATA, *LPWIN32_FILE_ATTRIBUTE_DATA;

typedef enum _GET_FILEEX_INFO_LEVELS {
    GetFileExInfoStandard,
    GetFileExMaxInfoLevel
} GET_FILEEX_INFO_LEVELS;

typedef struct _WIN32_FIND_DATAA {
    DWORD dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD nFileSizeHigh;
    DWORD nFileSizeLow;
    DWORD dwReserved0;
    DWORD dwReserved1;
    CHAR cFileName[MAX_PATH];
    CHAR cAlternateFileName[14];
} WIN32_FIND_DATAA, *PWIN32_FIND_DATAA, *LPWIN32_FIND_DATAA;
typedef WIN32_FIND_DATAA WIN32_FIND_DATA;
typedef PWIN32_FIND_DATAA PWIN32_FIND_DATA;
typedef LPWIN32_FIND_DATAA LPWIN32_FIND_DATA;

typedef struct _MEMORYSTATUS {
    DWORD dwLength;
    DWORD dwMemoryLoad;
    SIZE_T dwTotalPhys;
    SIZE_T dwAvailPhys;
    SIZE_T dwTotalPageFile;
    SIZE_T dwAvailPageFile;
    SIZE_T dwTotalVirtual;
    SIZE_T dwAvailVirtual;
} MEMORYSTATUS, *LPMEMORYSTATUS;

#ifdef __cplusplus
static_assert(sizeof(WIN32_FILE_ATTRIBUTE_DATA) == 0x24, "public Win32 attribute-data layout");
static_assert(sizeof(WIN32_FIND_DATAA) == 0x140, "public Win32 find-data layout");
static_assert(__builtin_offsetof(WIN32_FIND_DATAA, cFileName) == 0x2c, "public Win32 find-data name offset");
static_assert(sizeof(MEMORYSTATUS) == 0x20, "public Win32 memory-status layout");
#endif

/* ---- Public Winsock declarations (Windows Sockets 2 reference) ----------- */
typedef unsigned char u_char;
typedef unsigned short u_short;
typedef unsigned int u_int;
typedef unsigned long u_long;
typedef UINT SOCKET;
#define INVALID_SOCKET ((SOCKET)(~0))
#define SOCKET_ERROR (-1)
#define AF_UNSPEC 0
#define AF_INET 2
#define SOCK_STREAM 1
#define SOCK_DGRAM 2
#define IPPROTO_IP 0
#define IPPROTO_TCP 6
#define IPPROTO_UDP 17
/* The Xbox secure-datagram protocol: retail socket creation (0xb53e0) passes
 * 0xfe as the protocol for its datagram endpoints. */
#define IPPROTO_VDP 254
#define SOL_SOCKET 0xffff
#define SO_DEBUG 0x0001
#define SO_ACCEPTCONN 0x0002
#define SO_REUSEADDR 0x0004
#define SO_KEEPALIVE 0x0008
#define SO_DONTROUTE 0x0010
#define SO_BROADCAST 0x0020
#define SO_LINGER 0x0080
#define SO_OOBINLINE 0x0100
#define SO_SNDBUF 0x1001
#define SO_RCVBUF 0x1002
#define SO_ERROR 0x1007
#define SO_TYPE 0x1008
#define TCP_NODELAY 0x0001
#define FIONBIO ((long)0x8004667eul)
#define FIONREAD ((long)0x4004667ful)
#define INADDR_ANY ((u_long)0x00000000)
#define INADDR_BROADCAST ((u_long)0xffffffff)
#define INADDR_NONE 0xffffffff
#define SD_RECEIVE 0x00
#define SD_SEND 0x01
#define SD_BOTH 0x02
#define WSABASEERR 10000
#define WSAEINTR 10004
#define WSAEBADF 10009
#define WSAEACCES 10013
#define WSAEFAULT 10014
#define WSAEINVAL 10022
#define WSAEMFILE 10024
#define WSAEWOULDBLOCK 10035
#define WSAEINPROGRESS 10036
#define WSAEALREADY 10037
#define WSAENOTSOCK 10038
#define WSAEMSGSIZE 10040
#define WSAEADDRINUSE 10048
#define WSAENETDOWN 10050
#define WSAENETUNREACH 10051
#define WSAENETRESET 10052
#define WSAECONNABORTED 10053
#define WSAECONNRESET 10054
#define WSAENOBUFS 10055
#define WSAEISCONN 10056
#define WSAENOTCONN 10057
#define WSAESHUTDOWN 10058
#define WSAETIMEDOUT 10060
#define WSAECONNREFUSED 10061
#define WSAEHOSTUNREACH 10065
#define WSANOTINITIALISED 10093

struct sockaddr {
    u_short sa_family;
    char sa_data[14];
};

struct sockaddr_in {
    short sin_family;
    u_short sin_port;
    IN_ADDR sin_addr;
    char sin_zero[8];
};

struct linger {
    u_short l_onoff;
    u_short l_linger;
};

struct timeval {
    long tv_sec;
    long tv_usec;
};

#ifndef FD_SETSIZE
#define FD_SETSIZE 64
#endif
typedef struct fd_set {
    u_int fd_count;
    SOCKET fd_array[FD_SETSIZE];
} fd_set;

#define WSADESCRIPTION_LEN 256
#define WSASYS_STATUS_LEN 128
typedef struct WSAData {
    WORD wVersion;
    WORD wHighVersion;
    char szDescription[WSADESCRIPTION_LEN + 1];
    char szSystemStatus[WSASYS_STATUS_LEN + 1];
    unsigned short iMaxSockets;
    unsigned short iMaxUdpDg;
    char* lpVendorInfo;
} WSADATA, *LPWSADATA;

typedef struct sockaddr SOCKADDR;
typedef struct sockaddr* PSOCKADDR;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct linger LINGER;
typedef struct timeval TIMEVAL;

#ifdef __cplusplus
static_assert(sizeof(sockaddr) == 0x10, "public Winsock address layout");
static_assert(sizeof(sockaddr_in) == 0x10, "public Winsock IPv4 address layout");
static_assert(sizeof(fd_set) == 0x104, "public Winsock descriptor-set layout");
static_assert(sizeof(WSADATA) == 0x190, "public Winsock startup-data layout");
#endif

/* ---- Xbox network (XNet), from the recovered networking code ------------- */
/* Opaque title-server address; the recovered code only passes pointers. */
struct TSADDR;

/* Twelve one-byte fields: retail transport startup (0x8d840) zero-fills a
 * 0x0c-byte record and stores each value below at consecutive offsets. */
struct XNetStartupParams {
    BYTE cfgSizeOfStruct;
    BYTE cfgFlags;
    BYTE cfgPrivatePoolSizeInPages;
    BYTE cfgEnetReceiveQueueLength;
    BYTE cfgIpFragMaxSimultaneous;
    BYTE cfgIpFragMaxPacketDiv256;
    BYTE cfgSockMaxSockets;
    BYTE cfgSockDefaultRecvBufsizeInK;
    BYTE cfgSockDefaultSendBufsizeInK;
    BYTE cfgKeyRegMax;
    BYTE cfgSecRegMax;
    BYTE cfgQosDataLimitDiv4;
};

/* QoS results: retail (0x7b6c0..0x7b7b0) reads the pending count at +4 and
 * 0x18-byte per-target records from +8 with the fields below. */
struct XNQOSINFO {
    BYTE bFlags;
    BYTE bReserved;
    WORD cProbesXmit;
    WORD cProbesRecv;
    WORD cbData;
    BYTE* pbData;
    WORD wRttMinInMsecs;
    WORD wRttMedInMsecs;
    DWORD dwUpBitsPerSec;
    DWORD dwDnBitsPerSec;
};

struct XNQOS {
    UINT cxnqos;
    UINT cxnqosPending;
    XNQOSINFO axnqosinfo[1];
};

#ifdef __cplusplus
static_assert(sizeof(XNetStartupParams) == 0x0c, "retail XNet startup-parameter layout");
static_assert(sizeof(XNQOSINFO) == 0x18, "retail QoS result stride");
static_assert(__builtin_offsetof(XNQOSINFO, wRttMinInMsecs) == 0x0c, "retail QoS RTT offset");
static_assert(__builtin_offsetof(XNQOSINFO, dwUpBitsPerSec) == 0x10, "retail QoS bandwidth offset");
static_assert(__builtin_offsetof(XNQOS, axnqosinfo) == 0x08, "retail QoS result offset");
#endif

/* Bits tested by the retail QoS readers (0x7b720, 0x7b7b0). */
#define XNET_XNQOSINFO_COMPLETE 0x01
#define XNET_XNQOSINFO_TARGET_CONTACTED 0x02
#define XNET_XNQOSINFO_TARGET_DISABLED 0x04
#define XNET_XNQOSINFO_DATA_RECEIVED 0x08
#define XNET_XNQOSINFO_PARTIAL_COMPLETE 0x10

/* Key-identifier kind in the top nibble: retail 0x7a8a0 ors 0x80 for online
 * sessions and nothing for system link. */
#define XNET_XNKID_MASK 0xf0
#define XNET_XNKID_SYSTEM_LINK 0x00
#define XNET_XNKID_ONLINE_PEER 0x80

/* Title address flags: retail 0x7a9b0 compares against 0 and 1 and tests the
 * combined 0x1c mask; the individual bit order is PROVISIONAL. */
#define XNET_GET_XNADDR_PENDING 0x00
#define XNET_GET_XNADDR_NONE 0x01
#define XNET_GET_XNADDR_ETHERNET 0x02 /* PROVISIONAL */
#define XNET_GET_XNADDR_STATIC 0x04 /* PROVISIONAL */
#define XNET_GET_XNADDR_DHCP 0x08 /* PROVISIONAL */
#define XNET_GET_XNADDR_PPPOE 0x10 /* PROVISIONAL */

/* Retail transport code (0x8d690) tests bit 0 of the link status. */
#define XNET_ETHERNET_LINK_ACTIVE 0x01

/* ---- Xbox system: launch data, saves, content, input, memory ------------- */
/* Retail launch code (0x12200, 0x2238f4) reserves and clears 0xc00 bytes. */
#define MAX_LAUNCH_DATA_SIZE 0xc00
/* Retail XGetLaunchInfo caller (0x12200) accepts type 0 as title data. */
#define LDT_TITLE 0

typedef struct _LAUNCH_DATA {
    BYTE Data[MAX_LAUNCH_DATA_SIZE];
} LAUNCH_DATA, *PLAUNCH_DATA;

/* Retail dashboard launch (0x2238f4) stores the reason at +0 and the context
 * and parameters at +4, +8, +0x0c; its reason table is the six values below. */
typedef struct _LD_LAUNCH_DASHBOARD {
    DWORD dwReason;
    DWORD dwContext;
    DWORD dwParameter1;
    DWORD dwParameter2;
} LD_LAUNCH_DASHBOARD, *PLD_LAUNCH_DASHBOARD;
#define XLD_LAUNCH_DASHBOARD_MAIN_MENU 0
#define XLD_LAUNCH_DASHBOARD_MEMORY 2
#define XLD_LAUNCH_DASHBOARD_NETWORK_CONFIGURATION 6
#define XLD_LAUNCH_DASHBOARD_NEW_ACCOUNT_SIGNUP 7
#define XLD_LAUNCH_DASHBOARD_ACCOUNT_MANAGEMENT 8
#define XLD_LAUNCH_DASHBOARD_ONLINE_MENU 9

/* Retail game-region switch (0x11cbb0). */
#define XC_GAME_REGION_NA 0x00000001
#define XC_GAME_REGION_JAPAN 0x00000002
#define XC_GAME_REGION_RESTOFWORLD 0x00000004
#define XC_GAME_REGION_MANUFACTURING 0x80000000

/* Saved-game enumeration: retail 0x2164c0 reads the find data's file name at
 * +0x2c, the save directory at +0x140 and the display name at +0x244. */
#define MAX_GAMENAME 64 /* PROVISIONAL */
typedef struct _XGAME_FIND_DATA {
    WIN32_FIND_DATAA wfd;
    CHAR szSaveGameDirectory[MAX_PATH];
    WCHAR szSaveGameName[MAX_GAMENAME];
} XGAME_FIND_DATA, *PXGAME_FIND_DATA;

/* Downloaded content: retail 0x8d550/0x8d5a0 use a 0x350-byte record with
 * the directory at +0x14c and a 0x7f-character display name at +0x250. */
#define MAX_CONTENT_DISPLAY_NAME 128
typedef struct _XCONTENT_FIND_DATA {
    BYTE unknown000[0x14c];
    CHAR szContentDirectory[MAX_PATH];
    WCHAR szDisplayName[MAX_CONTENT_DISPLAY_NAME];
} XCONTENT_FIND_DATA, *PXCONTENT_FIND_DATA;

#ifdef __cplusplus
static_assert(sizeof(LAUNCH_DATA) == 0xc00, "retail launch-data size");
static_assert(__builtin_offsetof(XGAME_FIND_DATA, szSaveGameDirectory) == 0x140,
              "retail save-directory offset");
static_assert(__builtin_offsetof(XGAME_FIND_DATA, szSaveGameName) == 0x244,
              "retail save-name offset");
static_assert(sizeof(XCONTENT_FIND_DATA) == 0x350, "retail content find-data layout");
static_assert(__builtin_offsetof(XCONTENT_FIND_DATA, szDisplayName) == 0x250,
              "retail content display-name offset");
#endif

/* Signature calculation: retail preferences code (0x121120) passes 1. */
#define XCALCSIG_FLAG_NON_ROAMABLE 0x00000001

/* Allocation attributes: retail XMemAlloc (0x12280) switches on bits 16..23,
 * the 0x82 allocator (0x12c090) tests bits 0..12 and bit 30. */
typedef struct _XALLOC_ATTRIBUTES {
    DWORD dwObjectType : 13;
    DWORD unknown13 : 3;
    DWORD dwAllocatorId : 8;
    DWORD unknown24 : 6;
    DWORD dwZeroInitialize : 1;
    DWORD unknown31 : 1;
} XALLOC_ATTRIBUTES, *PXALLOC_ATTRIBUTES;

/* Input devices. On the Xbox a device type is the address of a descriptor;
 * retail 0x1248b0 passes three distinct descriptor addresses. The native port
 * supplies its own descriptor objects. */
typedef struct h2_native_device_type XPP_DEVICE_TYPE, *PXPP_DEVICE_TYPE;
extern XPP_DEVICE_TYPE h2_native_device_type_gamepad;
extern XPP_DEVICE_TYPE h2_native_device_type_memory_unit;
extern XPP_DEVICE_TYPE h2_native_device_type_voice_microphone;
extern XPP_DEVICE_TYPE h2_native_device_type_voice_headphone;
#define XDEVICE_TYPE_GAMEPAD (&h2_native_device_type_gamepad)
#define XDEVICE_TYPE_MEMORY_UNIT (&h2_native_device_type_memory_unit)
#define XDEVICE_TYPE_VOICE_MICROPHONE (&h2_native_device_type_voice_microphone)
#define XDEVICE_TYPE_VOICE_HEADPHONE (&h2_native_device_type_voice_headphone)
/* Retail XInputOpen callers (0x124aa0) pass slot 0. */
#define XDEVICE_NO_SLOT 0
#define XDEVICE_TOP_SLOT 0
#define XDEVICE_BOTTOM_SLOT 1
/* Retail 0x124aa0 compares the capability subtype byte with 1 and 2. */
#define XINPUT_DEVSUBTYPE_GC_GAMEPAD 0x01
#define XINPUT_DEVSUBTYPE_GC_GAMEPAD_ALT 0x02

typedef struct _XDEVICE_PREALLOC_TYPE {
    PXPP_DEVICE_TYPE DeviceType;
    DWORD dwPreallocCount;
} XDEVICE_PREALLOC_TYPE, *PXDEVICE_PREALLOC_TYPE;

/* Retail input polling (0x124c00) reads the button word at +4, eight analog
 * bytes from +6 and the four stick axes from +0x0e of the state record. */
typedef struct _XINPUT_GAMEPAD {
    WORD wButtons;
    BYTE bAnalogButtons[8];
    SHORT sThumbLX;
    SHORT sThumbLY;
    SHORT sThumbRX;
    SHORT sThumbRY;
} XINPUT_GAMEPAD, *PXINPUT_GAMEPAD;

typedef struct _XINPUT_STATE {
    DWORD dwPacketNumber;
    XINPUT_GAMEPAD Gamepad;
} XINPUT_STATE, *PXINPUT_STATE;

/* Retail 0x124aa0 reads only the subtype byte at offset 0; the remainder of
 * the record's size is PROVISIONAL. */
typedef struct _XINPUT_CAPABILITIES {
    BYTE SubType;
    BYTE unknown01[0x2b]; /* PROVISIONAL */
} XINPUT_CAPABILITIES, *PXINPUT_CAPABILITIES;

/* Rumble feedback: retail 0x124ee0 walks 0x46-byte records, testing the
 * status at +0 and writing the motor speeds at +0x42 and +0x44. */
#pragma pack(push, 2)
typedef struct _XINPUT_RUMBLE {
    WORD wLeftMotorSpeed;
    WORD wRightMotorSpeed;
} XINPUT_RUMBLE, *PXINPUT_RUMBLE;

typedef struct _XINPUT_FEEDBACK_HEADER {
    DWORD dwStatus;
    BYTE unknown04[0x3e];
} XINPUT_FEEDBACK_HEADER, *PXINPUT_FEEDBACK_HEADER;

typedef struct _XINPUT_FEEDBACK {
    XINPUT_FEEDBACK_HEADER Header;
    XINPUT_RUMBLE Rumble;
} XINPUT_FEEDBACK, *PXINPUT_FEEDBACK;
#pragma pack(pop)

#ifdef __cplusplus
static_assert(sizeof(XDEVICE_PREALLOC_TYPE) == 0x08, "retail device-preallocation stride");
static_assert(__builtin_offsetof(XINPUT_STATE, Gamepad) == 0x04, "retail input-state gamepad offset");
static_assert(__builtin_offsetof(XINPUT_GAMEPAD, bAnalogButtons) == 0x02, "retail analog-button offset");
static_assert(__builtin_offsetof(XINPUT_GAMEPAD, sThumbLX) == 0x0a, "retail thumbstick offset");
static_assert(sizeof(XINPUT_FEEDBACK) == 0x46, "retail rumble-feedback stride");
static_assert(__builtin_offsetof(XINPUT_FEEDBACK, Rumble) == 0x42, "retail rumble offset");
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Win32 files, errors, time and memory. */
HANDLE WINAPI CreateFileA(LPCSTR path, DWORD access, DWORD share_mode, void* security,
                          DWORD creation_disposition, DWORD flags, HANDLE template_file);
BOOL WINAPI WriteFile(HANDLE file, LPCVOID buffer, DWORD bytes_to_write,
                      DWORD* bytes_written, void* overlapped);
DWORD WINAPI GetFileSize(HANDLE file, DWORD* size_high);
BOOL WINAPI SetEndOfFile(HANDLE file);
BOOL WINAPI FlushFileBuffers(HANDLE file);
BOOL WINAPI DeleteFileA(LPCSTR path);
BOOL WINAPI CreateDirectoryA(LPCSTR path, void* security);
BOOL WINAPI RemoveDirectoryA(LPCSTR path);
DWORD WINAPI GetFileAttributesA(LPCSTR path);
BOOL WINAPI SetFileAttributesA(LPCSTR path, DWORD attributes);
BOOL WINAPI GetFileAttributesExA(LPCSTR path, GET_FILEEX_INFO_LEVELS level, LPVOID information);
HANDLE WINAPI FindFirstFileA(LPCSTR pattern, WIN32_FIND_DATAA* data);
BOOL WINAPI FindNextFileA(HANDLE find, WIN32_FIND_DATAA* data);
BOOL WINAPI FindClose(HANDLE find);
void WINAPI SetLastError(DWORD error);
void WINAPI GetLocalTime(SYSTEMTIME* time);
void WINAPI GetSystemTime(SYSTEMTIME* time);
void WINAPI GlobalMemoryStatus(MEMORYSTATUS* status);
#define CreateFile CreateFileA
#define DeleteFile DeleteFileA
#define CreateDirectory CreateDirectoryA
#define RemoveDirectory RemoveDirectoryA
#define GetFileAttributes GetFileAttributesA
#define SetFileAttributes SetFileAttributesA
#define GetFileAttributesEx GetFileAttributesExA
#define FindFirstFile FindFirstFileA
#define FindNextFile FindNextFileA

/* Winsock. */
SOCKET WINAPI socket(int family, int type, int protocol);
int WINAPI bind(SOCKET socket, const struct sockaddr* address, int address_length);
int WINAPI connect(SOCKET socket, const struct sockaddr* address, int address_length);
int WINAPI closesocket(SOCKET socket);
int WINAPI shutdown(SOCKET socket, int how);
int WINAPI recv(SOCKET socket, char* buffer, int length, int flags);
int WINAPI send(SOCKET socket, const char* buffer, int length, int flags);
int WINAPI recvfrom(SOCKET socket, char* buffer, int length, int flags,
                    struct sockaddr* from, int* from_length);
int WINAPI sendto(SOCKET socket, const char* buffer, int length, int flags,
                  const struct sockaddr* to, int to_length);
int WINAPI select(int count, fd_set* read_set, fd_set* write_set, fd_set* error_set,
                  const struct timeval* timeout);
int WINAPI ioctlsocket(SOCKET socket, long command, u_long* argument);
int WINAPI getsockopt(SOCKET socket, int level, int name, char* value, int* length);
int WINAPI setsockopt(SOCKET socket, int level, int name, const char* value, int length);
int WINAPI WSAStartup(WORD version, LPWSADATA data);
int WINAPI WSACleanup(void);
int WINAPI WSAGetLastError(void);
void WINAPI WSASetLastError(int error);
u_long WINAPI htonl(u_long value);
u_short WINAPI htons(u_short value);
u_long WINAPI ntohl(u_long value);
u_short WINAPI ntohs(u_short value);

/* Xbox network. */
int WINAPI XNetStartup(const XNetStartupParams* parameters);
int WINAPI XNetCleanup(void);
/* fills the buffer with random bytes */
int WINAPI XNetRandom(BYTE* buffer, UINT count);
DWORD WINAPI XNetGetEthernetLinkStatus(void);
DWORD WINAPI XNetGetTitleXnAddr(XNADDR* address);
int WINAPI XNetCreateKey(XNKID* key_id, XNKEY* key);
int WINAPI XNetRegisterKey(const XNKID* key_id, const XNKEY* key);
int WINAPI XNetXnAddrToInAddr(const XNADDR* address, const XNKID* key_id, IN_ADDR* in_address);
int WINAPI XNetInAddrToXnAddr(IN_ADDR in_address, XNADDR* address, XNKID* key_id);
DWORD WINAPI XNetGetConnectStatus(IN_ADDR in_address);
int WINAPI XNetConnect(IN_ADDR in_address);
int WINAPI XNetTsAddrToInAddr(const TSADDR* ts_address, DWORD service_id,
                              const XNKID* key_id, IN_ADDR* in_address);
int WINAPI XNetQosLookup(UINT address_count, const XNADDR* addresses[],
                         const XNKID* key_ids[], const XNKEY* keys[],
                         UINT in_address_count, const IN_ADDR in_addresses[],
                         const DWORD service_ids[], UINT probe_count,
                         DWORD bits_per_second, DWORD flags, HANDLE event,
                         XNQOS** qos);
int WINAPI XNetQosServiceLookup(DWORD flags, HANDLE event, XNQOS** qos);
int WINAPI XNetQosRelease(XNQOS* qos);

/* Xbox system. */
DWORD WINAPI XGetLaunchInfo(DWORD* type, LAUNCH_DATA* data);
DWORD WINAPI XLaunchNewImage(LPCSTR image_path, PLAUNCH_DATA data);
DWORD WINAPI XGetGameRegion(void);
DWORD WINAPI XGetVideoStandard(void);
DWORD WINAPI XGetVideoFlags(void);
BOOL WINAPI XMountSecondaryUtilityDrive(void);
HANDLE WINAPI XFindFirstSaveGame(LPCSTR root, XGAME_FIND_DATA* data);
BOOL WINAPI XFindNextSaveGame(HANDLE find, XGAME_FIND_DATA* data);
HANDLE WINAPI XFindFirstContent(LPCSTR root, DWORD title_id, XCONTENT_FIND_DATA* data);
BOOL WINAPI XFindNextContent(HANDLE find, XCONTENT_FIND_DATA* data);
BOOL WINAPI XFindClose(HANDLE find);
HANDLE WINAPI XCalculateSignatureBegin(DWORD flags);
DWORD WINAPI XCalculateSignatureUpdate(HANDLE context, const BYTE* data, DWORD size);
DWORD WINAPI XCalculateSignatureEnd(HANDLE context, XCALCSIG_SIGNATURE* signature);
LPVOID WINAPI XMemAllocDefault(SIZE_T size, DWORD attributes);
void WINAPI XMemFreeDefault(LPVOID address, DWORD attributes);
void* WINAPI XLoadSection(const char* name);
void WINAPI XFreeSection(const char* name);
LONG WINAPI XSaveFloatingPointStateForDpc(void);
LONG WINAPI XRestoreFloatingPointStateForDpc(void);
void WINAPI XInitDevices(DWORD type_count, const XDEVICE_PREALLOC_TYPE* types);
BOOL WINAPI XGetDeviceChanges(PXPP_DEVICE_TYPE type, DWORD* insertions, DWORD* removals);
HANDLE WINAPI XInputOpen(PXPP_DEVICE_TYPE type, DWORD port, DWORD slot, void* parameters);
void WINAPI XInputClose(HANDLE device);
DWORD WINAPI XInputGetCapabilities(HANDLE device, XINPUT_CAPABILITIES* capabilities);
DWORD WINAPI XInputGetState(HANDLE device, XINPUT_STATE* state);
DWORD WINAPI XInputSetState(HANDLE device, XINPUT_FEEDBACK* feedback);

#ifdef __cplusplus
}
#endif

/* Winsock descriptor-set helpers, written from the documented semantics. */
static inline void h2_native_fd_set(SOCKET socket, fd_set* set)
{
    u_int i;
    for (i = 0; i < set->fd_count; i++)
        if (set->fd_array[i] == socket)
            return;
    if (set->fd_count < FD_SETSIZE)
        set->fd_array[set->fd_count++] = socket;
}
static inline int h2_native_fd_isset(SOCKET socket, const fd_set* set)
{
    u_int i;
    for (i = 0; i < set->fd_count; i++)
        if (set->fd_array[i] == socket)
            return 1;
    return 0;
}
#define FD_ZERO(set) (((fd_set*)(set))->fd_count = 0)
#define FD_SET(socket, set) h2_native_fd_set((SOCKET)(socket), (fd_set*)(set))
#define FD_ISSET(socket, set) h2_native_fd_isset((SOCKET)(socket), (const fd_set*)(set))

#include <float.h>

/* As on the Xbox, the umbrella header also provides Direct3D and DirectSound. */
#include <d3d8.h>
#include <dsound.h>

#endif
