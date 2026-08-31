.syntax unified
.cpu cortex-m4
.thumb

.extern main
.extern __bss_start
.extern __bss_end
.extern _stack_top

.section .isr_vector, "a", %progbits
.align 8
.global g_pfnVectors
g_pfnVectors:
    .word _stack_top
    .word Reset_Handler
    .word Default_Handler
    .word Default_Handler
    .word Default_Handler
    .word Default_Handler
    .word Default_Handler
    .word 0
    .word 0
    .word 0
    .word 0
    .word Default_Handler
    .word Default_Handler
    .word 0
    .word Default_Handler
    .word Default_Handler
    .word Default_Handler
    .word Default_Handler
    .word Default_Handler
    .word Default_Handler
    .word Default_Handler
    .word Default_Handler
    .word Default_Handler
    .word Default_Handler
    .word Default_Handler
    .word Default_Handler
    .word Default_Handler
.size g_pfnVectors, . - g_pfnVectors

.section .text.Reset_Handler, "ax", %progbits
.global Reset_Handler
.type Reset_Handler, %function
.thumb_func
Reset_Handler:
    ldr r0, =__bss_start
    ldr r1, =__bss_end
    movs r2, #0
1:
    cmp r0, r1
    bcs 2f
    str r2, [r0], #4
    b 1b
2:
    bl main
3:
    b 3b

.section .text.Default_Handler, "ax", %progbits
.global Default_Handler
.type Default_Handler, %function
.thumb_func
Default_Handler:
4:
    b 4b
