#ifndef H2_PORT_CRT_WCHAR_H
#define H2_PORT_CRT_WCHAR_H

#ifndef _SIZE_T
#define _SIZE_T
typedef __SIZE_TYPE__ size_t;
#endif

typedef __builtin_va_list va_list;
#if !defined(__cplusplus) || defined(H2_MS_ABI_FRONTEND)
typedef unsigned short wchar_t;
#endif

#ifdef H2_NATIVE_PORT
#define wcschr h2_guest_wcschr
#define wcscmp h2_guest_wcscmp
#define wcslen h2_guest_wcslen
#define wcsncmp h2_guest_wcsncmp
#define wcsncpy h2_guest_wcsncpy
#define _wcsicmp h2_guest_wcsicmp
#define _vsnwprintf h2_guest_vsnwprintf
#define iswalnum h2_guest_iswalnum
#define iswspace h2_guest_iswspace
#endif

#ifdef __cplusplus
extern "C" {
#endif

wchar_t* wcschr(const wchar_t* text, wchar_t character);
int wcscmp(const wchar_t* first, const wchar_t* second);
size_t wcslen(const wchar_t* text);
int wcsncmp(const wchar_t* first, const wchar_t* second, size_t size);
wchar_t* wcsncpy(wchar_t* destination, const wchar_t* source, size_t size);
int _wcsicmp(const wchar_t* first, const wchar_t* second);
int _vsnwprintf(wchar_t* destination, size_t size, const wchar_t* format, va_list arguments);
int iswalnum(wchar_t character);
int iswspace(wchar_t character);

#ifdef __cplusplus
}
#endif

#endif
