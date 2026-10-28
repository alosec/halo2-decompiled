#ifndef H2_PORT_CRT_STDIO_H
#define H2_PORT_CRT_STDIO_H

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct h2_guest_FILE FILE;

extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;

FILE *fopen(const char *path, const char *mode);
int fclose(FILE *stream);
int fprintf(FILE *stream, const char *format, ...);
int printf(const char *format, ...);
int sprintf(char *destination, const char *format, ...);
int _vsnprintf(char *destination, __SIZE_TYPE__ size, const char *format, va_list arguments);
int fscanf(FILE *stream, const char *format, ...);
int fputs(const char *text, FILE *stream);
int puts(const char *text);
char *fgets(char *buffer, int count, FILE *stream);
__SIZE_TYPE__ fread(void *data, __SIZE_TYPE__ size, __SIZE_TYPE__ count, FILE *stream);
__SIZE_TYPE__ fwrite(const void *data, __SIZE_TYPE__ size, __SIZE_TYPE__ count, FILE *stream);
int fseek(FILE *stream, long offset, int origin);
long ftell(FILE *stream);
int fflush(FILE *stream);
int vsprintf(char *destination, const char *format, va_list arguments);

#ifdef __cplusplus
}
#endif

#endif
