#ifndef H2_PORT_CRT_TIME_H
#define H2_PORT_CRT_TIME_H

#ifdef H2_MS_ABI_FRONTEND
typedef long time_t;
#else
typedef __INT64_TYPE__ time_t;
#endif

#ifdef __cplusplus
extern "C" {
#endif

time_t time(time_t *result);

#ifdef __cplusplus
}
#endif

#endif
