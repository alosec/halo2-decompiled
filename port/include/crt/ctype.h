/* ctype.h for the guest: the recovered units that include it want the wide
 * character tests of wchar.h, and the narrow ones below. */
#ifndef H2_PORT_CRT_CTYPE_H
#define H2_PORT_CRT_CTYPE_H
#include <wchar.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline int isdigit(int c) { return c >= '0' && c <= '9'; }
static inline int isupper(int c) { return c >= 'A' && c <= 'Z'; }
static inline int islower(int c) { return c >= 'a' && c <= 'z'; }
static inline int isalpha(int c) { return isupper(c) || islower(c); }
static inline int isalnum(int c) { return isalpha(c) || isdigit(c); }
static inline int isspace(int c) { return c == ' ' || (c >= '\t' && c <= '\r'); }
static inline int isxdigit(int c) { return isdigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'); }
static inline int toupper(int c) { return islower(c) ? c - 'a' + 'A' : c; }
static inline int tolower(int c) { return isupper(c) ? c - 'A' + 'a' : c; }

#ifdef __cplusplus
}
#endif

#endif
