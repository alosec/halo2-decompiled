.section .text.h2_loader_probe,"ax",@progbits
.p2align 2
.globl h2_loader_probe
.type h2_loader_probe, @function
h2_loader_probe:
    mov w0, #77
    ret
.size h2_loader_probe, .-h2_loader_probe

.section .data.h2_loader_probe,"aw",@progbits
.p2align 3
.globl h2_host_slot_host_h2_probe
.type h2_host_slot_host_h2_probe, @object
h2_host_slot_host_h2_probe:
    .xword 0
.size h2_host_slot_host_h2_probe, 8

.globl h2_host_slot_host_h2_physical_memory_malloc_low
h2_host_slot_host_h2_physical_memory_malloc_low:
    .xword 0
.globl h2_host_slot_host_h2_cache_read
h2_host_slot_host_h2_cache_read:
    .xword 0
.globl h2_host_slot_host_h2_map_preflight
h2_host_slot_host_h2_map_preflight:
    .xword 0
.globl h2_host_slot_host_h2_map_select
h2_host_slot_host_h2_map_select:
    .xword 0
.globl h2_host_slot_host_h2_map_is_active
h2_host_slot_host_h2_map_is_active:
    .xword 0
.globl h2_host_slot_host_h2_video_control
h2_host_slot_host_h2_video_control:
    .xword 0
.globl h2_host_slot_host_h2_render_request
h2_host_slot_host_h2_render_request:
    .xword 0
.globl h2_host_slot_host_h2_text_request
h2_host_slot_host_h2_text_request:
    .xword 0
.globl h2_host_slot_host_h2_get_milliseconds
h2_host_slot_host_h2_get_milliseconds:
    .xword 0
.globl h2_host_slot_host_h2_sleep
h2_host_slot_host_h2_sleep:
    .xword 0
.globl h2_host_slot_host_h2_cos
h2_host_slot_host_h2_cos:
    .xword 0
.globl h2_host_slot_host_h2_profile_list
h2_host_slot_host_h2_profile_list:
    .xword 0
.globl h2_host_slot_host_h2_profile_create
h2_host_slot_host_h2_profile_create:
    .xword 0
.globl h2_host_slot_host_h2_profile_read
h2_host_slot_host_h2_profile_read:
    .xword 0
.globl h2_host_slot_host_h2_profile_write
h2_host_slot_host_h2_profile_write:
    .xword 0
.globl h2_host_slot_host_h2_profile_delete
h2_host_slot_host_h2_profile_delete:
    .xword 0

.globl h2_loader_data
.type h2_loader_data, @object
h2_loader_data:
    .word 0x12345678
.size h2_loader_data, 4

.globl h2_host_slot_host_h2_async_create_mutex
h2_host_slot_host_h2_async_create_mutex: .xword 0
.globl h2_host_slot_host_h2_async_create_semaphore
h2_host_slot_host_h2_async_create_semaphore: .xword 0
.globl h2_host_slot_host_h2_async_wait
h2_host_slot_host_h2_async_wait: .xword 0
.globl h2_host_slot_host_h2_async_release_mutex
h2_host_slot_host_h2_async_release_mutex: .xword 0
.globl h2_host_slot_host_h2_async_release_semaphore
h2_host_slot_host_h2_async_release_semaphore: .xword 0
.globl h2_host_slot_host_h2_async_set_file_pointer
h2_host_slot_host_h2_async_set_file_pointer: .xword 0
.globl h2_host_slot_host_h2_async_read_file
h2_host_slot_host_h2_async_read_file: .xword 0
.globl h2_host_slot_host_h2_async_set_thread_priority
h2_host_slot_host_h2_async_set_thread_priority: .xword 0
.globl h2_host_slot_host_h2_async_thread_exit_code
h2_host_slot_host_h2_async_thread_exit_code: .xword 0
.globl h2_host_slot_host_h2_async_close
h2_host_slot_host_h2_async_close: .xword 0
.globl h2_host_slot_host_h2_async_yield
h2_host_slot_host_h2_async_yield: .xword 0

.section .text.h2_worker_step_probe,"ax",@progbits
.p2align 2
.globl h2_worker_step_probe
.type h2_worker_step_probe, @function
h2_worker_step_probe:
    adrp x9, h2_worker_step_calls
    add x9, x9, :lo12:h2_worker_step_calls
    ldr w10, [x9]
    add w10, w10, #1
    str w10, [x9]
    cmp w10, #2
    cset w0, hs
    ret
.size h2_worker_step_probe, .-h2_worker_step_probe

.section .data.h2_worker_step_probe,"aw",@progbits
.p2align 2
.globl h2_worker_step_calls
.type h2_worker_step_calls, @object
h2_worker_step_calls:
    .word 0
.size h2_worker_step_calls, 4
