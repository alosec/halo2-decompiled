#ifndef H2_PORT_CRT_STDLIB_H
#define H2_PORT_CRT_STDLIB_H

#ifndef _SIZE_T
#define _SIZE_T
typedef __SIZE_TYPE__ size_t;
#endif

#ifdef H2_NATIVE_PORT
#define qsort h2_guest_qsort
#endif

#ifdef __cplusplus
extern "C" {
#endif

int abs(int value);
int rand(void);
void qsort(void* base, size_t count, size_t size,
           int (__cdecl* compare)(const void*, const void*));

#ifdef __cplusplus
}
#endif

#endif
