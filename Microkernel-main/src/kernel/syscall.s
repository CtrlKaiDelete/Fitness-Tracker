/* Created by Julien Rodot on 29/8/26. Licensed under MIT */

.include "Microkernel/include/mefi/assembly.inc"

.section .text.kernel.syscall, "ax", %progbits

.extern kernel_svc_dispatch

ASM_FUNCTION kernel_svc_handler
    ASM_EXCEPTION_FRAME r0
    sub sp, sp, #4
    push {r4-r11, lr}
    mov r1, sp
    mov r2, lr
    bl kernel_svc_dispatch
    pop {r4-r11, lr}
    add sp, sp, #4
    bx lr
ASM_END_FUNCTION kernel_svc_handler
