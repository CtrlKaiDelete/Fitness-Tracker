/* Created by Julien Rodot on 29/8/26. Licensed under MIT */

.include "Microkernel/include/mefi/assembly.inc"

.section .text.kernel.interrupt, "ax", %progbits
.extern kernel_irq_dispatch

ASM_FUNCTION kernel_irq_handler
    mrs r0, ipsr
    subs r0, r0, #16
    b kernel_irq_dispatch
ASM_END_FUNCTION kernel_irq_handler
