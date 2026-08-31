/* Created by Julien Rodot on 30/8/26. Licensed under MIT */
.include "Microkernel/include/mefi/assembly.inc"
.section .text.kernel.context, "ax", %progbits
.extern kernel_pendsv_dispatch
ASM_FUNCTION kernel_pendsv_handler
    tst lr, #4
    beq .Lcontext_select
    mrs r0, psp
    tst lr, #16
    it eq
    vstmdbeq r0!, {s16-s31}
    stmdb r0!, {r4-r11}
    mrs r2, control
    stmdb r0!, {r2, lr}
    b .Lcontext_dispatch
.Lcontext_select:
    movs r0, #0
.Lcontext_dispatch:
    bl kernel_pendsv_dispatch
    ldmia r0!, {r2, lr}
    msr control, r2
    isb
    ldmia r0!, {r4-r11}
    tst lr, #16
    it eq
    vldmiaeq r0!, {s16-s31}
    msr psp, r0
    bx lr
ASM_END_FUNCTION kernel_pendsv_handler
