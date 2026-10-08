#ifndef H2_PORT_CRT_STRING_H
#define H2_PORT_CRT_STRING_H

#ifndef _SIZE_T
#define _SIZE_T
typedef __SIZE_TYPE__ size_t;
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
size_t strspn(const char* text, const char* accepted);
char* strstr(const char* text, const char* pattern);

#ifdef __cplusplus
}
#endif

#endif
