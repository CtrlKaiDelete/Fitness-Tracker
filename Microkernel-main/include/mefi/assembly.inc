/* Created by Julien Rodot on 29/8/26. Licensed under MIT */

/*
 * Common assembly conventions for the firmware and kernel.
 *
 * Public symbols use lower_snake_case and the AAPCS register convention.
 * Every callable routine is emitted as a Thumb function in a named section.
 */

.syntax unified
.cpu cortex-m4
.thumb

.equ MEFI_REVISION,                   0x00010000
.equ MEFI_SYSTEM_TABLE_SIGNATURE,     0x4946454D
.equ MEFI_BOOT_SERVICES_SIGNATURE,    0x5342454D
.equ MEFI_GRAPHICS_OUTPUT_SIGNATURE,  0x4F47454D

.equ MEFI_SUCCESS,                    0
.equ MEFI_INVALID_PARAMETER,          1

.macro ASM_FUNCTION name
    .balign 4
    .global \name
    .type \name, %function
    .thumb_func
\name:
.endm

.macro ASM_END_FUNCTION name
    .size \name, . - \name
.endm

/* The core frame starts at the selected stack; an FP extension follows it. */
.macro ASM_EXCEPTION_FRAME output
    tst lr, #4
    ite eq
    mrseq \output, msp
    mrsne \output, psp
.endm
