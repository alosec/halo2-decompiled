#ifndef H2_PORT_CRT_STDLIB_H
#define H2_PORT_CRT_STDLIB_H

#ifndef _SIZE_T
#define _SIZE_T
typedef __SIZE_TYPE__ size_t;
#endif

#ifdef H2_NATIVE_PORT
#define abs h2_guest_abs
#define qsort h2_guest_qsort
#define labs h2_guest_labs
#define atoi h2_guest_atoi
#define atol h2_guest_atol
#define strtol h2_guest_strtol
#define strtoul h2_guest_strtoul
#endif

#ifdef __cplusplus
extern "C" {
#endif

int abs(int value);
long labs(long value);
int atoi(const char* text);
long atol(const char* text);
long strtol(const char* text, char** end, int base);
unsigned long strtoul(const char* text, char** end, int base);
int rand(void);
void qsort(void* base, size_t count, size_t size,
           int (__cdecl* compare)(const void*, const void*));

#ifdef __cplusplus
}
#endif

#endif
