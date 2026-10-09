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
#define WINAPI __stdcall
/* Public Xbox network QoS listener flags used by recovered session code. */
#define XNET_QOS_LISTEN_ENABLE 0x01
#define XNET_QOS_LISTEN_RELEASE 0x10

#ifdef H2_NATIVE_PORT
/* Minimal native declarations for the recovered Xbox immediate-mode calls.
 * Retail function_22a664 passes primitive value 7 for TRIANGLEFAN; the native
 * boundary keeps the vertex register explicit instead of relying on the Xbox
 * compiler intrinsic. */
#define D3DPT_TRIANGLEFAN 7
#define D3DVSDE_VERTEX 0
#endif

#ifdef __cplusplus
extern "C" {
#endif

int __cdecl abs(int value);
#ifdef H2_NATIVE_PORT
void __stdcall D3DDevice_Begin(DWORD primitive_type);
void __stdcall D3DDevice_End(void);
void __stdcall D3DDevice_SetVertexData2f(DWORD vertex_register, float x, float y);
void __stdcall D3DDevice_SetVertexData4f(DWORD vertex_register, float x, float y,
                                         float z, float w);
void __stdcall D3DDevice_SetVertexDataColor(DWORD vertex_register, DWORD color);
#endif
LONG __stdcall CompareFileTime(const FILETIME* first, const FILETIME* second);
BOOL __stdcall GetDiskFreeSpaceExA(const char* root, ULARGE_INTEGER* available,
                                   ULARGE_INTEGER* total, ULARGE_INTEGER* free_bytes);
void __stdcall GetSystemTimeAsFileTime(FILETIME* time);
DWORD __stdcall GetTickCount(void);
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
void __stdcall XPhysicalProtect(void* address, DWORD size, DWORD protection);

#ifdef __cplusplus
}
#endif

#endif
