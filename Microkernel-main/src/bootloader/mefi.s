/* Created by Julien Rodot on 27/8/26. Licensed under MIT */

.include "Microkernel/include/mefi/assembly.inc"

.equ MEFI_KERNEL_VECTOR_BASE,  0x00010000
.equ MEFI_FIRMWARE_BASE,       0x00000000
.equ MEFI_FIRMWARE_SIZE,       0x00010000

.section .isr_vector, "a", %progbits
.align 8

.global mefi_vector_table
.type mefi_vector_table, %object
mefi_vector_table:
    .word _stack_top
    .word mefi_entrypoint
    .word mefi_fault_handler
    .word mefi_fault_handler
    .word mefi_fault_handler
    .word mefi_fault_handler
    .word mefi_fault_handler
    .word 0
    .word 0
    .word 0
    .word 0
    .word mefi_fault_handler
    .word mefi_fault_handler
    .word 0
    .word mefi_fault_handler
    .word mefi_graphics_systick_handler
.size mefi_vector_table, . - mefi_vector_table

.section .text.mefi.boot, "ax", %progbits

.extern mefi_exit_boot_services
.extern mefi_graphics_initialize
.extern mefi_graphics_systick_handler
.extern mefi_graphics_write_frame

/* r0: number of milliseconds to wait while MEFI owns SysTick. */
ASM_FUNCTION mefi_stall
    cbz r0, .Lstalled

    ldr r1, =0xE000E010
    movs r2, #1
    lsls r2, r2, #16

.Lstall_tick:
    ldr r3, [r1]
    tst r3, r2
    beq .Lstall_tick

    subs r0, r0, #1
    bne .Lstall_tick

.Lstalled:
    bx lr
ASM_END_FUNCTION mefi_stall

/* Hold the MEFI logo, animate it outward, then hold before kernel entry. */
ASM_FUNCTION mefi_boot_animation
    push {r4, lr}

    ldr r0, =mefi_boot_logo
    bl mefi_graphics_write_frame
    movw r0, #2000
    bl mefi_stall

    ldr r0, =mefi_animation_frame_one
    bl mefi_graphics_write_frame
    movs r0, #120
    bl mefi_stall

    ldr r0, =mefi_animation_frame_two
    bl mefi_graphics_write_frame
    movs r0, #120
    bl mefi_stall

    ldr r0, =mefi_animation_frame_three
    bl mefi_graphics_write_frame
    movs r0, #120
    bl mefi_stall

    ldr r0, =mefi_boot_logo
    bl mefi_graphics_write_frame
    movw r0, #1000
    bl mefi_stall

    pop {r4, pc}
ASM_END_FUNCTION mefi_boot_animation

ASM_FUNCTION mefi_entrypoint
    bl mefi_graphics_initialize
    bl mefi_boot_animation

    ldr r0, =MEFI_KERNEL_VECTOR_BASE
    ldr r1, =mefi_system_table
    b mefi_exit_boot_services
ASM_END_FUNCTION mefi_entrypoint

ASM_FUNCTION mefi_fault_handler
    cpsid i
.Lmefi_fault:
    b .Lmefi_fault
ASM_END_FUNCTION mefi_fault_handler

.section .rodata.mefi, "a", %progbits
.balign 4

/* MEFI_BOOT_SERVICES */
.global mefi_boot_services
.type mefi_boot_services, %object
mefi_boot_services:
    .word MEFI_BOOT_SERVICES_SIGNATURE
    .word MEFI_REVISION
    .word 24
    .word 0
    .word mefi_stall
    .word mefi_exit_boot_services
.size mefi_boot_services, . - mefi_boot_services

/* MEFI_GRAPHICS_OUTPUT */
.global mefi_graphics_output
.type mefi_graphics_output, %object
mefi_graphics_output:
    .word MEFI_GRAPHICS_OUTPUT_SIGNATURE
    .word MEFI_REVISION
    .word 44
    .word 0
    .word 5
    .word 5
    .word 5
    .word mefi_graphics_framebuffer
    .word mefi_graphics_set_pixel
    .word mefi_graphics_clear
    .word mefi_graphics_write_frame
.size mefi_graphics_output, . - mefi_graphics_output

/* MEFI_SYSTEM_TABLE */
.global mefi_system_table
.type mefi_system_table, %object
mefi_system_table:
    .word MEFI_SYSTEM_TABLE_SIGNATURE
    .word MEFI_REVISION
    .word 36
    .word 0
    .word mefi_boot_services
    .word mefi_graphics_output
    .word MEFI_KERNEL_VECTOR_BASE
    .word MEFI_FIRMWARE_BASE
    .word MEFI_FIRMWARE_SIZE
.size mefi_system_table, . - mefi_system_table

mefi_animation_frame_one:
    .byte 0b00000, 0b01110, 0b01010, 0b01110, 0b00000

mefi_animation_frame_two:
    .byte 0b00000, 0b00100, 0b01110, 0b00100, 0b00000

mefi_animation_frame_three:
    .byte 0b00100, 0b01010, 0b10001, 0b01010, 0b00100

/* The original boot logo remains the final animation frame. */
mefi_boot_logo:
    .byte 0b00000, 0b01110, 0b01010, 0b01110, 0b00000
