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
