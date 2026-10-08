#ifndef H2_PORT_CRT_STDIO_H
#define H2_PORT_CRT_STDIO_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct h2_guest_FILE FILE;

FILE *fopen(const char *path, const char *mode);
int fclose(FILE *stream);
int fscanf(FILE *stream, const char *format, ...);
char *fgets(char *buffer, int count, FILE *stream);

#ifdef __cplusplus
}
#endif

#endif
