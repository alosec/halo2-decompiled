#ifndef H2_PORT_HOST_ABI_H
#define H2_PORT_HOST_ABI_H

#ifdef __cplusplus
extern "C" {
#endif

#define H2_HOST_ABI_VERSION 11u

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
unsigned int host_h2_async_create_mutex(int initial_owner);
unsigned int host_h2_async_create_semaphore(int initial_count, int maximum_count);
unsigned int host_h2_async_wait(unsigned int handle, unsigned int milliseconds);
int host_h2_async_release_mutex(unsigned int handle);
int host_h2_async_release_semaphore(unsigned int handle, int release_count,
                                    int* previous_count);
int host_h2_async_set_file_pointer(unsigned int handle, int distance,
                                   int* distance_high, unsigned int method,
                                   unsigned int* position_low);
int host_h2_async_read_file(unsigned int handle, void* buffer,
                            unsigned int bytes_to_read, unsigned int* bytes_read);
int host_h2_async_set_thread_priority(unsigned int handle, int priority);
int host_h2_async_thread_exit_code(unsigned int handle, unsigned int* exit_code);
int host_h2_async_close(unsigned int handle);
void host_h2_async_yield(void);

#ifdef __cplusplus
}
#endif

#endif
