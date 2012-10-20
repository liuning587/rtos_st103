
/* Entry Point */
ENTRY(resetRoutine)

cstack_top = 0x20010000;    /* end of 64K RAM */

MEMORY
{
    FLASH (rx) : ORIGIN = 0x08001000, LENGTH = 0x0007f000
    SRAM (rwx) : ORIGIN = 0x20000000, LENGTH = 0x00010000
}
    
SECTIONS
{
    .text :
    {   
        . = ALIGN(4);
        _text = .;
        KEEP(*(.isr_vector))
        *(.text*)
        *(.rodata*)
        __shell_cmd_start = .;
        KEEP(*(.shell_cmd)) 
        __shell_cmd_end = .;
       . = ALIGN(4);
    } > FLASH

    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)

    } > FLASH

    .ARM.exidx :
    {
        __exidx_start = .;
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
        __exidx_end = .;
    } > FLASH


    . = ALIGN(4);
     _etext = .;
    /* This is used by the startup in order to initialize the .data secion */
    _sidata = _etext;
    .data : AT(_sidata)
    {
        . = ALIGN(4);
        _data = .;
        *(vtable)
        . = ALIGN(4);
        *(.data*)
        . = ALIGN(4);
        _edata = .;
        . = ALIGN(4);
    } > SRAM
    
    
    .bss :
    {
        _bss = .;
        . = ALIGN(4);
        *(.bss)
        . = ALIGN(4);
        *(.bss*)
        . = ALIGN(4);
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } > SRAM
    
	 heap_low = .; /* for _sbrk */
     . = . + 0x2000; /* 4kB of heap memory */
     heap_top = .; /* for _sbrk */


}
