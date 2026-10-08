#ifndef H2_PORT_CRT_STRING_H
#define H2_PORT_CRT_STRING_H

#ifndef _SIZE_T
#define _SIZE_T
typedef __SIZE_TYPE__ size_t;
#endif

#ifdef H2_NATIVE_PORT
#define memchr h2_guest_memchr
#define memcmp h2_guest_memcmp
#define memcpy h2_guest_memcpy
#define memmove h2_guest_memmove
#define memset h2_guest_memset
#define strchr h2_guest_strchr
#define strcmp h2_guest_strcmp
#define strcpy h2_guest_strcpy
#define strlen h2_guest_strlen
#define strncmp h2_guest_strncmp
#define strncpy h2_guest_strncpy
#define strrchr h2_guest_strrchr
#define strspn h2_guest_strspn
#define strstr h2_guest_strstr
#endif

#ifdef __cplusplus
extern "C" {
#endif

void* memchr(const void* data, int value, size_t size);
int memcmp(const void* first, const void* second, size_t size);
void* memcpy(void* destination, const void* source, size_t size);
void* memmove(void* destination, const void* source, size_t size);
void* memset(void* destination, int value, size_t size);
char* strchr(const char* text, int character);
int strcmp(const char* first, const char* second);
char* strcpy(char* destination, const char* source);
size_t strlen(const char* text);
int strncmp(const char* first, const char* second, size_t size);
char* strncpy(char* destination, const char* source, size_t size);
char* strrchr(const char* text, int character);
char* strpbrk(const char* text, const char* accepted);
size_t strspn(const char* text, const char* accepted);
char* strstr(const char* text, const char* pattern);

#ifdef __cplusplus
}
#endif

#endif
