/* Created by Julien Rodot on 27/8/26. Licensed under MIT */

.include "Microkernel/include/mefi/assembly.inc"

.equ P0_OUTSET,       0x50000508
.equ P0_OUTCLR,       0x5000050C
.equ P0_DIRSET,       0x50000518
.equ P1_OUTSET,       0x50000808
.equ P1_OUTCLR,       0x5000080C
.equ P1_DIRSET,       0x50000818

.equ SYST_CSR,        0xE000E010
.equ SYST_RVR,        0xE000E014
.equ SYST_CVR,        0xE000E018

.equ ROW_MASK,        0x01688000
.equ COLUMN_P0_MASK,  0xD0000800
.equ COLUMN_P1_MASK,  0x00000020
.equ MATRIX_P0_MASK,  0xD1688800

.section .text.mefi.graphics, "ax", %progbits

ASM_FUNCTION mefi_graphics_initialize
    push {r4, lr}

    ldr r0, =P0_DIRSET
    ldr r1, =MATRIX_P0_MASK
    str r1, [r0]

    ldr r0, =P1_DIRSET
    ldr r1, =COLUMN_P1_MASK
    str r1, [r0]

    ldr r0, =P0_OUTCLR
    ldr r1, =ROW_MASK
    str r1, [r0]

    ldr r0, =P0_OUTSET
    ldr r1, =COLUMN_P0_MASK
    str r1, [r0]

    ldr r0, =P1_OUTSET
    ldr r1, =COLUMN_P1_MASK
    str r1, [r0]

    bl mefi_graphics_clear

    ldr r0, =mefi_graphics_current_row
    movs r1, #0
    strb r1, [r0]

    /* 64 MHz core clock divided into one-millisecond display scans. */
    ldr r0, =SYST_RVR
    ldr r1, =63999
    str r1, [r0]

    ldr r0, =SYST_CVR
    movs r1, #0
    str r1, [r0]

    ldr r0, =SYST_CSR
    movs r1, #7
    str r1, [r0]

    pop {r4, pc}
ASM_END_FUNCTION mefi_graphics_initialize

/* r0: x, r1: y, r2: nonzero to set; returns MEFI_STATUS in r0. */
ASM_FUNCTION mefi_graphics_set_pixel
    cmp r0, #4
    bhi .Lpixel_invalid
    cmp r1, #4
    bhi .Lpixel_invalid

    ldr r3, =mefi_graphics_framebuffer
    ldrb r12, [r3, r1]
    movs r3, #1
    lsls r3, r3, r0

    cbz r2, .Lpixel_clear
    orrs r12, r12, r3
    b .Lpixel_store

.Lpixel_clear:
    bics r12, r12, r3

.Lpixel_store:
    ldr r3, =mefi_graphics_framebuffer
    strb r12, [r3, r1]
    movs r0, #MEFI_SUCCESS
    bx lr

.Lpixel_invalid:
    movs r0, #MEFI_INVALID_PARAMETER
    bx lr
ASM_END_FUNCTION mefi_graphics_set_pixel

ASM_FUNCTION mefi_graphics_clear
    ldr r0, =mefi_graphics_framebuffer
    movs r1, #0
    strb r1, [r0, #0]
    strb r1, [r0, #1]
    strb r1, [r0, #2]
    strb r1, [r0, #3]
    strb r1, [r0, #4]
    bx lr
ASM_END_FUNCTION mefi_graphics_clear

/* r0: pointer to five row bytes; returns MEFI_STATUS in r0. */
ASM_FUNCTION mefi_graphics_write_frame
    cbz r0, .Lframe_invalid

    ldr r1, =mefi_graphics_framebuffer
    ldrb r2, [r0, #0]
    strb r2, [r1, #0]
    ldrb r2, [r0, #1]
    strb r2, [r1, #1]
    ldrb r2, [r0, #2]
    strb r2, [r1, #2]
    ldrb r2, [r0, #3]
    strb r2, [r1, #3]
    ldrb r2, [r0, #4]
    strb r2, [r1, #4]
    movs r0, #MEFI_SUCCESS
    bx lr

.Lframe_invalid:
    movs r0, #MEFI_INVALID_PARAMETER
    bx lr
ASM_END_FUNCTION mefi_graphics_write_frame

/* SysTick multiplexes one physical display row per millisecond. */
ASM_FUNCTION mefi_graphics_systick_handler
    ldr r0, =P0_OUTCLR
    ldr r1, =ROW_MASK
    str r1, [r0]

    ldr r0, =P0_OUTSET
    ldr r1, =COLUMN_P0_MASK
    str r1, [r0]

    ldr r0, =P1_OUTSET
    ldr r1, =COLUMN_P1_MASK
    str r1, [r0]

    ldr r0, =mefi_graphics_current_row
    ldrb r3, [r0]
    ldr r0, =mefi_graphics_framebuffer
    ldrb r2, [r0, r3]

    tst r2, #0x01
    beq .Lcolumn_one_done
    ldr r0, =P0_OUTCLR
    ldr r1, =0x10000000
    str r1, [r0]
.Lcolumn_one_done:

    tst r2, #0x02
    beq .Lcolumn_two_done
    ldr r0, =P0_OUTCLR
    ldr r1, =0x00000800
    str r1, [r0]
.Lcolumn_two_done:

    tst r2, #0x04
    beq .Lcolumn_three_done
    ldr r0, =P0_OUTCLR
    ldr r1, =0x80000000
    str r1, [r0]
.Lcolumn_three_done:

    tst r2, #0x08
    beq .Lcolumn_four_done
    ldr r0, =P1_OUTCLR
    ldr r1, =COLUMN_P1_MASK
    str r1, [r0]
.Lcolumn_four_done:

    tst r2, #0x10
    beq .Lcolumn_five_done
    ldr r0, =P0_OUTCLR
    ldr r1, =0x40000000
    str r1, [r0]
.Lcolumn_five_done:

    ldr r0, =mefi_graphics_row_masks
    ldr r1, [r0, r3, lsl #2]
    ldr r0, =P0_OUTSET
    str r1, [r0]

    adds r3, r3, #1
    cmp r3, #5
    blo .Lrow_valid
    movs r3, #0
.Lrow_valid:

    ldr r0, =mefi_graphics_current_row
    strb r3, [r0]
    bx lr
ASM_END_FUNCTION mefi_graphics_systick_handler

.section .rodata.mefi.graphics, "a", %progbits
.balign 4
mefi_graphics_row_masks:
    .word 0x00200000
    .word 0x00400000
    .word 0x00008000
    .word 0x01000000
    .word 0x00080000

.section .bss.mefi.graphics, "aw", %nobits
.balign 4
.global mefi_graphics_framebuffer
.type mefi_graphics_framebuffer, %object
mefi_graphics_framebuffer:
    .space 5
.size mefi_graphics_framebuffer, . - mefi_graphics_framebuffer

mefi_graphics_current_row:
    .space 1
