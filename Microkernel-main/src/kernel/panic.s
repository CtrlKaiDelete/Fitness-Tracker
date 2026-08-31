/* Created by Julien Rodot on 29/8/26. Licensed under MIT */

.include "Microkernel/include/mefi/assembly.inc"

.section .text.kernel.fault, "ax", %progbits

.extern kernel_fault_dispatch
.extern kernel_memory_fault_dispatch

.macro KERNEL_FAULT_HANDLER name, reason
ASM_FUNCTION \name
    ASM_EXCEPTION_FRAME r1
    movs r0, #\reason
    mov r2, lr
    b kernel_fault_dispatch
ASM_END_FUNCTION \name
.endm

KERNEL_FAULT_HANDLER kernel_nmi_handler, 1
KERNEL_FAULT_HANDLER kernel_hard_fault_handler, 2
KERNEL_FAULT_HANDLER kernel_bus_fault_handler, 4
KERNEL_FAULT_HANDLER kernel_usage_fault_handler, 5
KERNEL_FAULT_HANDLER kernel_debug_monitor_handler, 6
KERNEL_FAULT_HANDLER kernel_systick_fault_handler, 8

ASM_FUNCTION kernel_memory_fault_handler
    ASM_EXCEPTION_FRAME r0
    mov r1, lr
    push {r4, lr}
    bl kernel_memory_fault_dispatch
    pop {r4, lr}
    bx lr
ASM_END_FUNCTION kernel_memory_fault_handler
