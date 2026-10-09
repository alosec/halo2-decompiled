#ifndef H2_PORT_CRT_STDIO_H
#define H2_PORT_CRT_STDIO_H

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
int fscanf(FILE *stream, const char *format, ...);
int fputs(const char *text, FILE *stream);
int puts(const char *text);
char *fgets(char *buffer, int count, FILE *stream);

#ifdef __cplusplus
}
#endif

#endif
