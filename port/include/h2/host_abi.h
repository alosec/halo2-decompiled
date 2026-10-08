#ifndef H2_PORT_HOST_ABI_H
#define H2_PORT_HOST_ABI_H

#ifdef __cplusplus
extern "C" {
#endif

#define H2_HOST_ABI_VERSION 2u

/* Calls crossing from the 32-bit guest engine into the native host. Pointer
 * arguments are guest pointers; the compiler adapter rebases them before the
 * host implementation receives them. Keep this interface versioned and small. */
unsigned int host_h2_abi_version(void);
void host_h2_log(const char* message);
void* host_h2_physical_memory_malloc_low(int size, unsigned int protection);
int host_h2_cache_read(unsigned int offset, int size, void* buffer);
void host_h2_video_control(int command);
void host_h2_render_request(const void* request, const void* vertices);

#ifdef __cplusplus
}
#endif

#endif
