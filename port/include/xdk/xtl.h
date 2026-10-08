#ifndef H2_PORT_XDK_XTL_H
#define H2_PORT_XDK_XTL_H

/* Clean native-port declarations limited to public Win32-compatible facts and
 * APIs visible in the recovered source. This is not copied from an Xbox SDK. */

typedef void* HANDLE;
typedef int BOOL;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef long LONG;
typedef long HRESULT;

#ifdef H2_MS_ABI_FRONTEND
typedef unsigned short wchar_t;
#endif

struct _FILETIME {
    DWORD dwLowDateTime;
    DWORD dwHighDateTime;
};
typedef struct _FILETIME FILETIME;

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
#define SUCCEEDED(result) ((HRESULT)(result) >= 0)
#define PAGE_READONLY 0x02
#define PAGE_READWRITE 0x04
#define PAGE_WRITECOMBINE 0x400

#ifdef __cplusplus
extern "C" {
#endif

int __cdecl abs(int value);
LONG __stdcall CompareFileTime(const FILETIME* first, const FILETIME* second);
void __stdcall GetSystemTimeAsFileTime(FILETIME* time);
DWORD __stdcall GetTickCount(void);
void __stdcall Sleep(DWORD milliseconds);
void __stdcall XPhysicalProtect(void* address, DWORD size, DWORD protection);

#ifdef __cplusplus
}
#endif

#endif
