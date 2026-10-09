SECTIONS {
    . = 0x10000000;
    .text : { *(.text .text.*) }
    .rodata : { *(.rodata .rodata.*) }
    . = ALIGN(0x4000);
    .data : { *(.data .data.*) }
    .bss : { *(.bss .bss.* COMMON) }
}
