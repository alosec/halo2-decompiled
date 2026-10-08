#ifndef H2_PORT_XDK_XTL_H
#define H2_PORT_XDK_XTL_H

/* Clean native-port declarations limited to public Win32-compatible facts and
 * APIs visible in the recovered source. This is not copied from an Xbox SDK. */

typedef void* HANDLE;
typedef int BOOL;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef long HRESULT;

struct _FILETIME {
    DWORD dwLowDateTime;
    DWORD dwHighDateTime;
};
typedef struct _FILETIME FILETIME;

#define FALSE 0
#define TRUE 1
#define PAGE_READONLY 0x02
#define PAGE_READWRITE 0x04
#define PAGE_WRITECOMBINE 0x400

#ifdef __cplusplus
extern "C" {
#endif

void __stdcall GetSystemTimeAsFileTime(FILETIME* time);
void __stdcall XPhysicalProtect(void* address, DWORD size, DWORD protection);

#ifdef __cplusplus
}
#endif

#endif
