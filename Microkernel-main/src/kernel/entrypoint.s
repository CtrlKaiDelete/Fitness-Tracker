/* Created by Julien Rodot on 28/8/26. Licensed under MIT */

.include "Microkernel/include/mefi/assembly.inc"

.equ SCB_CCR,    0xE000ED14
.equ SCB_SHCSR,  0xE000ED24
.equ SCB_CPACR,  0xE000ED88
.equ FPU_FPCCR,  0xE000EF34

.section .isr_vector, "a", %progbits
.align 8

.global kernel_vector_table
.type kernel_vector_table, %object
kernel_vector_table:
    .word _stack_top
    .word kernel_entrypoint
    .word kernel_nmi_handler
    .word kernel_hard_fault_handler
    .word kernel_memory_fault_handler
    .word kernel_bus_fault_handler
    .word kernel_usage_fault_handler
    .word 0
    .word 0
    .word 0
    .word 0
    .word kernel_svc_handler
    .word kernel_debug_monitor_handler
    .word 0
    .word kernel_pendsv_handler
    .word kernel_systick_fault_handler

    .word kernel_irq_handler
    .word kernel_irq_handler
    .word kernel_irq_handler
    .word kernel_irq_handler
    .word kernel_irq_handler
    .word kernel_irq_handler
    .word kernel_irq_handler
    .word kernel_irq_handler
    .word kernel_timer_handler
    .word kernel_irq_handler
    .word kernel_watchdog_handler
.size kernel_vector_table, . - kernel_vector_table

.section .text.kernel.entrypoint, "ax", %progbits

.extern __data_load
.extern __data_start
.extern __data_end
.extern __bss_start
.extern __bss_end
.extern __preinit_array_start
.extern __preinit_array_end
.extern __init_array_start
.extern __init_array_end
.extern KernelMain

/* r0 contains the MEFI system table supplied by the firmware. */
ASM_FUNCTION kernel_entrypoint
    mov r7, r0

    ldr r0, =SCB_CPACR
    ldr r1, [r0]
    ldr r2, =0x00F00000
    orrs r1, r1, r2
    str r1, [r0]
    dsb
    isb

    ldr r0, =FPU_FPCCR
    ldr r1, [r0]
    bic r1, r1, #0x40000000
    orr r1, r1, #0x80000000
    str r1, [r0]
    dsb
    isb

    ldr r0, =__data_load
    ldr r1, =__data_start
    ldr r2, =__data_end
.Lcopy_data:
    cmp r1, r2
    bcs .Lzero_bss
    ldr r3, [r0], #4
    str r3, [r1], #4
    b .Lcopy_data

.Lzero_bss:
    ldr r1, =__bss_start
    ldr r2, =__bss_end
    movs r3, #0
.Lzero_bss_loop:
    cmp r1, r2
    bcs .Lpreinit
    str r3, [r1], #4
    b .Lzero_bss_loop

.Lpreinit:
    ldr r4, =__preinit_array_start
    ldr r5, =__preinit_array_end
.Lpreinit_loop:
    cmp r4, r5
    bcs .Linit
    ldr r6, [r4], #4
    cmp r6, #0
    beq .Lpreinit_loop
    blx r6
    b .Lpreinit_loop

.Linit:
    ldr r4, =__init_array_start
    ldr r5, =__init_array_end
.Linit_loop:
    cmp r4, r5
    bcs .Lruntime_ready
    ldr r6, [r4], #4
    cmp r6, #0
    beq .Linit_loop
    blx r6
    b .Linit_loop

.Lruntime_ready:
    ldr r0, =SCB_SHCSR
    ldr r1, [r0]
    ldr r2, =0x00070000
    orrs r1, r1, r2
    str r1, [r0]

    ldr r0, =SCB_CCR
    ldr r1, [r0]
    orrs r1, r1, #0x18
    str r1, [r0]

    cpsie i
    mov r0, r7
    bl KernelMain

.Lkernel_returned:
    b .Lkernel_returned
ASM_END_FUNCTION kernel_entrypoint
