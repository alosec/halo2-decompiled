#ifndef H2_PORT_HOST_ABI_H
#define H2_PORT_HOST_ABI_H

#ifdef __cplusplus
extern "C" {
#endif

#define H2_HOST_ABI_VERSION 10u

/* Calls crossing from the 32-bit guest engine into the native host. Pointer
 * arguments are guest pointers; the compiler adapter rebases them before the
 * host implementation receives them. Keep this interface versioned and small. */
unsigned int host_h2_abi_version(void);
void host_h2_log(const char* message);
void* host_h2_physical_memory_malloc_low(int size, unsigned int protection);
int host_h2_cache_read(unsigned int offset, int size, void* buffer);
int host_h2_map_preflight(const char* map_name);
int host_h2_map_select(const char* map_name);
int host_h2_map_is_active(const char* map_name);
void host_h2_video_control(int command);
void host_h2_render_request(const void* request, const void* vertices);
void host_h2_text_request(const unsigned int* text, const void* bounds,
                          const void* clip, const void* position, int line_gap,
                          float scale, unsigned int color, const void* shadow);
unsigned int host_h2_get_milliseconds(void);
void host_h2_sleep(unsigned int milliseconds);
double host_h2_cos(double value);
int host_h2_profile_list(unsigned int* identifiers, unsigned int capacity);
int host_h2_profile_create(const void* record, unsigned int size);
int host_h2_profile_read(unsigned int identifier, void* record, unsigned int size);
int host_h2_profile_write(unsigned int identifier, const void* record,
                          unsigned int size);
int host_h2_profile_delete(unsigned int identifier);

#ifdef __cplusplus
}
#endif

#endif
