#ifndef H2_PORT_HOST_ABI_H
#define H2_PORT_HOST_ABI_H

#ifdef __cplusplus
extern "C" {
#endif

#define H2_HOST_ABI_VERSION 17u

/* Calls crossing from the 32-bit guest engine into the native host. Pointer
 * arguments are guest pointers; the compiler adapter rebases them before the
 * host implementation receives them. Keep this interface versioned and small. */
unsigned int host_h2_abi_version(void);
void host_h2_log(const char* message);
/* the title left (a dashboard or another image launched): the host exits */
void host_h2_exit(int code);
void* host_h2_physical_memory_malloc_low(int size, unsigned int protection);
void* host_h2_physical_alloc(unsigned int size, unsigned int physical_address,
                             unsigned int alignment, unsigned int protection);
int host_h2_physical_protect(void* address, unsigned int size,
                            unsigned int protection);
void* host_h2_virtual_alloc(void* address, unsigned int size,
                            unsigned int allocation_type,
                            unsigned int protection);
int host_h2_virtual_free(void* address, unsigned int size,
                         unsigned int free_type);
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
unsigned int host_h2_unimplemented(const char* symbol);
unsigned int host_h2_thread_create(unsigned int entry, unsigned int parameter,
                                   unsigned int stack_size);
double host_h2_cos(double value);
double host_h2_math_acos(double value);
double host_h2_math_exp(double value);
float host_h2_math_expf(float value);
double host_h2_math_fmod(double numerator, double denominator);
double host_h2_math_log(double value);
double host_h2_math_log10(double value);
float host_h2_math_logf(float value);
double host_h2_math_pow(double base, double exponent);
float host_h2_math_sinf(float value);
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
int host_h2_async_try_admit(void);
int host_h2_async_publish_admitted(void);
int host_h2_async_abort_admitted(void);

/* Xbox system services for whole images (port/src/xbox_system.cpp). Guest
 * addresses that the host maps itself travel as unsigned integers. */
unsigned int host_h2_xbox_memory_alloc(unsigned int size, unsigned int alignment,
                                       unsigned int physical, unsigned int top_down);
int host_h2_xbox_memory_free(unsigned int address);
unsigned int host_h2_xbox_memory_size(unsigned int address);
unsigned int host_h2_last_error(void);
void host_h2_set_last_error(unsigned int error);
unsigned int host_h2_file_open(const char* path, unsigned int access, unsigned int disposition);
int host_h2_file_is_handle(unsigned int handle);
int host_h2_file_read(unsigned int handle, void* buffer, unsigned int size, unsigned int* read);
int host_h2_file_write(unsigned int handle, const void* buffer, unsigned int size,
                       unsigned int* written);
int host_h2_file_seek(unsigned int handle, long long distance, unsigned int method,
                      unsigned long long* position);
int host_h2_file_size(unsigned int handle, unsigned long long* size);
int host_h2_file_truncate(unsigned int handle);
int host_h2_file_flush(unsigned int handle);
int host_h2_file_close(unsigned int handle);
unsigned int host_h2_file_attributes(const char* path, unsigned long long* size,
                                     unsigned long long* write_time);
int host_h2_file_delete(const char* path);
int host_h2_directory_create(const char* path);
int host_h2_directory_remove(const char* path);
unsigned int host_h2_find_first(const char* pattern, void* data);
int host_h2_find_next(unsigned int handle, void* data);
unsigned long long host_h2_system_time(void);
int host_h2_local_time_offset(void);
unsigned long long host_h2_performance_counter(void);
double host_h2_math_atan(double value);
double host_h2_math_atan2(double y, double x);
double host_h2_math_asin(double value);
/* the game session (port/src/main.cpp): one call per presented frame, and the
 * controller state for a port as an XINPUT_STATE record */
void host_h2_frame(void);
/* the renderer (port/src/gpu/): an h2_gpu_draw or h2_gpu_clear packet
 * (h2/gpu_protocol.h) in guest memory */
void host_h2_gpu_draw(const void* packet);
void host_h2_gpu_clear(const void* packet);
/* closes the open visibility test as the game's test `index`; its result,
   the samples that passed in the game's pixels, is ready once the frame is
   presented */
void host_h2_gpu_visibility_end(unsigned int index);
unsigned int host_h2_gpu_visibility_result(unsigned int index);
/* the mixer (port/src/audio/): an h2_audio_command (h2/audio_protocol.h), and
 * a voice's playback progress as an h2_audio_status (0: unknown voice) */
void host_h2_audio_command(const void* command);
int host_h2_audio_status(unsigned int voice, void* status);
/* WMA music (port/src/audio/wma_decoder.cpp): open a whole ASF sound chunk
 * (copied; 0: no decoder), pull it as 16-bit 44.1 kHz stereo (bytes produced,
 * short at the end), close it */
/* the port's level select (port/src/main.cpp): H2_START_MAP's solo map name
 * (e.g. 01b_spacestation) into buffer; 0 when unset */
unsigned int host_h2_start_map(char* buffer, unsigned int size);
/* the next "guest ..." line of the H2_CONTROL file (test hooks), or 0 */
unsigned int host_h2_guest_command(char* buffer, unsigned int size);
unsigned int host_h2_wma_open(const void* data, unsigned int size);
unsigned int host_h2_wma_decode(unsigned int handle, void* pcm, unsigned int size);
void host_h2_wma_close(unsigned int handle);
/* H2_TRACE: "d3d" = bit 0 */
unsigned int host_h2_trace_flags(void);
int host_h2_input_state(unsigned int port, void* state);
unsigned int host_h2_input_connected(void);
/* the mouse's look since the last call, in radians (two floats: yaw right,
   pitch down), for the player control update to add to the stick's */
void host_h2_mouse_take(void* look);

#ifdef __cplusplus
}
#endif

#endif
