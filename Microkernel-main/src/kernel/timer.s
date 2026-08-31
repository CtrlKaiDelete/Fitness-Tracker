/* Created by Julien Rodot on 29/8/26. Licensed under MIT */

.include "Microkernel/include/mefi/assembly.inc"

.section .text.kernel.timer, "ax", %progbits

.extern kernel_timer_dispatch
.extern kernel_watchdog_dispatch

ASM_FUNCTION kernel_timer_handler
    b kernel_timer_dispatch
ASM_END_FUNCTION kernel_timer_handler

ASM_FUNCTION kernel_watchdog_handler
    b kernel_watchdog_dispatch
ASM_END_FUNCTION kernel_watchdog_handler
