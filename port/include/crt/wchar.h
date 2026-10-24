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

#ifdef H2_PORT_SHIMS
#define wcschr h2_guest_wcschr
#define wcscmp h2_guest_wcscmp
#define wcslen h2_guest_wcslen
#define wcsncmp h2_guest_wcsncmp
#define wcsncpy h2_guest_wcsncpy
#define _wcsicmp h2_guest_wcsicmp
#define _vsnwprintf h2_guest_vsnwprintf
#define iswalnum h2_guest_iswalnum
#define iswspace h2_guest_iswspace
#define iswalpha h2_guest_iswalpha
#define iswdigit h2_guest_iswdigit
#define iswupper h2_guest_iswupper
#define iswlower h2_guest_iswlower
#define towlower h2_guest_towlower
#define towupper h2_guest_towupper
#define wcscpy h2_guest_wcscpy
#define wcscat h2_guest_wcscat
#define wcsrchr h2_guest_wcsrchr
#define wcsstr h2_guest_wcsstr
#define _wcsnicmp h2_guest_wcsnicmp
#define swprintf h2_guest_swprintf
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
int iswalpha(wchar_t character);
int iswdigit(wchar_t character);
int iswupper(wchar_t character);
int iswlower(wchar_t character);
wchar_t towlower(wchar_t character);
wchar_t towupper(wchar_t character);
wchar_t* wcscpy(wchar_t* destination, const wchar_t* source);
wchar_t* wcscat(wchar_t* destination, const wchar_t* source);
wchar_t* wcsrchr(const wchar_t* text, wchar_t character);
wchar_t* wcsstr(const wchar_t* text, const wchar_t* pattern);
int _wcsnicmp(const wchar_t* first, const wchar_t* second, size_t size);
int swprintf(wchar_t* destination, const wchar_t* format, ...);

#ifdef __cplusplus
}
#endif

#endif
