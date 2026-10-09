ENTRY(h2_main_menu_guest_bootstrap)

PHDRS
{
    text PT_LOAD FLAGS(5);
    rodata PT_LOAD FLAGS(4);
    data PT_LOAD FLAGS(6);
}

SECTIONS
{
    . = 0x10000000;
    .text : ALIGN(16)
    {
        *(.text .text.*)
    } :text

    . = ALIGN(0x4000);
    .rodata : ALIGN(16)
    {
        *(.rodata .rodata.*)
        *(.eh_frame .eh_frame_hdr)
    } :rodata

    . = ALIGN(0x4000);
    .data : ALIGN(16)
    {
        *(.data .data.*)
        *(.got .got.*)
    } :data

    .guest_init : ALIGN(4)
    {
        __guest_init_start = .;
        KEEP(*(.guest_init))
        __guest_init_end = .;
    } :data

    .bss (NOLOAD) : ALIGN(16)
    {
        *(.bss .bss.* COMMON)
    } :data

    /DISCARD/ :
    {
        *(.comment)
        *(.note .note.*)
    }
}
