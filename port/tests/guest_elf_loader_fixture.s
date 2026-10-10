.globl "?g_557c90@@3PAUs_texture_cache_file@@A"
.set "?g_557c90@@3PAUs_texture_cache_file@@A", 0x00557c90
.globl "?g_55aca8@@3JA"
.set "?g_55aca8@@3JA", 0x0055aca8
.globl "?g_4e6454@@3PAUs_record_pool@@A"
.set "?g_4e6454@@3PAUs_record_pool@@A", 0x004e6454
.globl "?g_4e6460@@3KA"
.set "?g_4e6460@@3KA", 0x004e6460
.globl "?g_4e6464@@3PAUs_physical_object@@A"
.set "?g_4e6464@@3PAUs_physical_object@@A", 0x004e6464
.globl "?g_4e6484@@3JA"
.set "?g_4e6484@@3JA", 0x004e6484

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

.p2align 3
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

.section .text.h2_async_host_trampolines,"ax",@progbits
.p2align 2
.macro h2_async_host_trampoline name
.globl \name
.type \name, @function
\name:
    adrp x16, h2_host_slot_\name
    ldr x16, [x16, :lo12:h2_host_slot_\name]
    br x16
.size \name, .-\name
.endm
h2_async_host_trampoline host_h2_async_create_mutex
h2_async_host_trampoline host_h2_async_create_semaphore
h2_async_host_trampoline host_h2_async_wait
h2_async_host_trampoline host_h2_async_release_mutex
h2_async_host_trampoline host_h2_async_release_semaphore
h2_async_host_trampoline host_h2_async_set_file_pointer
h2_async_host_trampoline host_h2_async_read_file
h2_async_host_trampoline host_h2_async_set_thread_priority
h2_async_host_trampoline host_h2_async_thread_exit_code
h2_async_host_trampoline host_h2_async_close
h2_async_host_trampoline host_h2_async_yield
h2_async_host_trampoline host_h2_async_try_admit
h2_async_host_trampoline host_h2_async_publish_admitted
h2_async_host_trampoline host_h2_async_abort_admitted

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

.section .text.h2_queue_lifecycle_probe,"ax",@progbits
.p2align 2
.globl h2_queue_lifecycle_probe
.type h2_queue_lifecycle_probe, @function
h2_queue_lifecycle_probe:
    adrp x9, h2_queue_lifecycle_calls
    add x9, x9, :lo12:h2_queue_lifecycle_calls
    ldr w10, [x9]
    add w10, w10, #1
    str w10, [x9]
    mov w0, #1
    ret
.size h2_queue_lifecycle_probe, .-h2_queue_lifecycle_probe

.section .data.h2_worker_step_probe,"aw",@progbits
.p2align 2
.globl h2_worker_step_calls
.type h2_worker_step_calls, @object
h2_worker_step_calls:
    .word 0
.size h2_worker_step_calls, 4

.globl h2_queue_lifecycle_calls
.type h2_queue_lifecycle_calls, @object
h2_queue_lifecycle_calls:
    .word 0
.size h2_queue_lifecycle_calls, 4

.section .data.h2_async_admission_slots,"aw",@progbits
.p2align 3
.globl h2_host_slot_host_h2_async_try_admit
h2_host_slot_host_h2_async_try_admit: .xword 0
.globl h2_host_slot_host_h2_async_publish_admitted
h2_host_slot_host_h2_async_publish_admitted: .xword 0
.globl h2_host_slot_host_h2_async_abort_admitted
h2_host_slot_host_h2_async_abort_admitted: .xword 0
