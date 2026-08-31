ENTRY(mefi_entrypoint)

MEMORY
{
    FLASH (rx)  : ORIGIN = 0x00000000, LENGTH = 64K
    RAM   (rwx) : ORIGIN = 0x20000000, LENGTH = 128K
}

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

    __bootloader_flash_start = ORIGIN(FLASH);
    __bootloader_flash_end = ORIGIN(FLASH) + LENGTH(FLASH);

    __ram_start = ORIGIN(RAM);
    __ram_end = ORIGIN(RAM) + LENGTH(RAM);

    __heap_start = __bss_end;
    __heap_end = _stack_top;

    /DISCARD/ :
    {
        *(.comment)
        *(.note*)
    }
}

ASSERT(__data_end <= _stack_top, "Bootloader RAM overflow")
ASSERT(LOADADDR(.data) + SIZEOF(.data) <= ORIGIN(FLASH) + LENGTH(FLASH), "Bootloader exceeds 64 KiB flash region")
