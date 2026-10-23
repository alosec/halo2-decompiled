.text
.p2align 4
.global h2_main_menu_guest_bootstrap
.type h2_main_menu_guest_bootstrap, %function
h2_main_menu_guest_bootstrap:
    stp x29, x30, [sp, #-32]!
    stp x19, x20, [sp, #16]
    mov x29, sp

    adrp x19, __guest_init_start
    add x19, x19, :lo12:__guest_init_start
    adrp x20, __guest_init_end
    add x20, x20, :lo12:__guest_init_end
.Lrun_initializers:
    cmp x19, x20
    b.hs .Linitializers_done
    ldr w9, [x19], #4
    cbz w9, .Lrun_initializers
    movz x10, #0x100, lsl #32
    add x9, x10, w9, uxtw
    blr x9
    b .Lrun_initializers

.Linitializers_done:
    mov x0, xzr
    bl "?cache_files_load_map%%YA_NPBD%Z"
    cbz w0, .Lmap_load_failed
    bl h2_main_menu_guest_load_strings
    cbnz w0, .Lreturn
    bl h2_main_menu_guest_open_press_start
    cbnz w0, .Lreturn
    bl h2_main_menu_guest_load_campaign_metadata
    cbnz w0, .Lreturn
    bl h2_main_menu_guest_frame
    b .Lreturn

.Lmap_load_failed:
    mov w0, #3
.Lreturn:
    ldp x19, x20, [sp, #16]
    ldp x29, x30, [sp], #32
    ret
.size h2_main_menu_guest_bootstrap, .-h2_main_menu_guest_bootstrap

/* Run the static constructors, then the recovered game main() (retail
   0x12190), which initializes the game and enters its main loop. */
.text
.p2align 4
.global h2_whole_game_main
.type h2_whole_game_main, %function
h2_whole_game_main:
    stp x29, x30, [sp, #-32]!
    stp x19, x20, [sp, #16]
    mov x29, sp
    adrp x19, __guest_init_start
    add x19, x19, :lo12:__guest_init_start
    adrp x20, __guest_init_end
    add x20, x20, :lo12:__guest_init_end
.Lgame_initializers:
    cmp x19, x20
    b.hs .Lgame_initializers_done
    ldr w9, [x19], #4
    cbz w9, .Lgame_initializers
    movz x10, #0x100, lsl #32
    add x9, x10, w9, uxtw
    blr x9
    b .Lgame_initializers
.Lgame_initializers_done:
    bl "?function_12190%%YAXXZ"
    mov w0, wzr
    ldp x19, x20, [sp, #16]
    ldp x29, x30, [sp], #32
    ret
.size h2_whole_game_main, .-h2_whole_game_main
