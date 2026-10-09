.text
.globl _h2_call_guest_bool_1
.globl _h2_call_guest_pointer_1
.globl _h2_call_guest_int_0
.p2align 2
_h2_call_guest_bool_1:
_h2_call_guest_pointer_1:
    stp x19, x20, [sp, #-16]!
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    mov x19, sp
    mov x20, x0
    mov x9, x2
    mov w0, w1
    mov sp, x9
    blr x20
    mov sp, x19
    ldp x29, x30, [sp], #16
    ldp x19, x20, [sp], #16
    ret

.p2align 2
_h2_call_guest_int_0:
    stp x19, x20, [sp, #-16]!
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    mov x19, sp
    mov x20, x0
    mov sp, x1
    blr x20
    mov sp, x19
    ldp x29, x30, [sp], #16
    ldp x19, x20, [sp], #16
    ret
