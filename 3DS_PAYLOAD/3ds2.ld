ENTRY(_Reset)
SECTIONS
{
 . = 0x080C3EE0;
 .text : {
 bootloader.o (INTERRUPT_VECTOR)
 *(.text)
 }
 .data : { *(.data) }
 .bss : { *(.bss COMMON) }
 . = ALIGN(8);
 heap_low = .;
 . = . + 0x1000; /* 64kB of stack memory */
 heap_top = .;
 . = . + 0x1000; /* 4kB of stack memory */
 stack_top = .;
}
