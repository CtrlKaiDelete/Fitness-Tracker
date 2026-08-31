/* Created by Julien Rodot on 27/8/26. Licensed under MIT */

.include "Microkernel/include/mefi/assembly.inc"

.equ SCB_ICSR,  0xE000ED04
.equ SCB_VTOR,  0xE000ED08
.equ SYST_CSR,  0xE000E010

.section .text.mefi.exit_boot_services, "ax", %progbits

/*
 * End MEFI boot services and enter the kernel.
 *
 * r0: kernel vector table
 * r1: MEFI system table passed to the kernel reset handler
 */
ASM_FUNCTION mefi_exit_boot_services
    ldr r2, [r0, #0]
    ldr r3, [r0, #4]
    mov r4, r1

    cpsid i

    ldr r1, =SYST_CSR
    movs r12, #0
    str r12, [r1]

    ldr r1, =SCB_ICSR
    ldr r12, =0x0A000000
    str r12, [r1]

    ldr r1, =SCB_VTOR
    str r0, [r1]
    dsb
    isb

    movs r1, #0
    msr control, r1
    isb

    msr msp, r2
    dsb
    isb

    mov r0, r4
    bx r3
ASM_END_FUNCTION mefi_exit_boot_services
