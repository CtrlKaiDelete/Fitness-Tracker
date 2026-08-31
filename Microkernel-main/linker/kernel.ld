ENTRY(kernel_entrypoint)

MEMORY
{
    FLASH (rx)  : ORIGIN = 0x00010000, LENGTH = 444K
    RAM   (rwx) : ORIGIN = 0x20000000, LENGTH = 128K
}

__page_pool_start = ORIGIN(RAM) + LENGTH(RAM) - 40K;
__page_pool_end = ORIGIN(RAM) + LENGTH(RAM) - 8K;
__kernel_stack_bottom = __page_pool_end;
_stack_top = ORIGIN(RAM) + LENGTH(RAM);

SECTIONS
{
    .isr_vector ORIGIN(FLASH) :
    {
        KEEP(*(.isr_vector))
    } > FLASH

    .text :
    {
        . = ALIGN(4);

        *(.text*)
        *(.rodata*)

        . = ALIGN(4);
    } > FLASH

    .ARM.extab :
    {
        *(.ARM.extab*)
    } > FLASH

    .ARM.exidx :
    {
        __exidx_start = .;

        *(.ARM.exidx*)

        __exidx_end = .;
    } > FLASH

    .preinit_array :
    {
        . = ALIGN(4);

        __preinit_array_start = .;

        KEEP(*(.preinit_array*))

        __preinit_array_end = .;
    } > FLASH

    .init_array :
    {
        . = ALIGN(4);

        __init_array_start = .;

        KEEP(*(SORT_BY_INIT_PRIORITY(.init_array.*)))
        KEEP(*(.init_array*))

        __init_array_end = .;
    } > FLASH

    .fini_array :
    {
        . = ALIGN(4);

        __fini_array_start = .;

        KEEP(*(SORT_BY_INIT_PRIORITY(.fini_array.*)))
        KEEP(*(.fini_array*))

        __fini_array_end = .;
    } > FLASH

    .data :
    {
        . = ALIGN(4);

        __data_start = .;

        *(.data*)

        . = ALIGN(4);

        __data_end = .;
    } > RAM AT > FLASH

    __data_load = LOADADDR(.data);

    .bss (NOLOAD) :
    {
        . = ALIGN(4);

        __bss_start = .;

        *(.bss*)
        *(COMMON)

        . = ALIGN(4);

        __bss_end = .;
    } > RAM

    __kernel_flash_start = ORIGIN(FLASH);
    __kernel_flash_end = ORIGIN(FLASH) + LENGTH(FLASH);

    __kernel_image_start = ORIGIN(FLASH);
    __kernel_image_end = LOADADDR(.data) + SIZEOF(.data);

    __ram_start = ORIGIN(RAM);
    __ram_end = ORIGIN(RAM) + LENGTH(RAM);

    __heap_start = __bss_end;
    __heap_end = __page_pool_start;

    PROVIDE(end = __heap_start);
    PROVIDE(_end = __heap_start);

    /DISCARD/ :
    {
        *(.comment)
        *(.note*)
    }
}

ASSERT(
    ADDR(.isr_vector) == 0x00010000,
    "Kernel vector table is NOT at 0x00010000"
)

ASSERT(
    __bss_end <= __heap_end,
    "Kernel data overlaps the heap or page pool"
)

ASSERT(
    (__page_pool_start & 1023) == 0,
    "Page pool is not aligned to a 1 KiB page"
)

ASSERT(
    (__page_pool_end - __page_pool_start) == 32K,
    "Page pool must contain exactly 32 pages"
)

ASSERT(
    __kernel_image_end <= ORIGIN(FLASH) + LENGTH(FLASH),
    "Kernel exceeds available flash"
)
