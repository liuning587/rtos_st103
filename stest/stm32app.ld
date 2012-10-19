MEMORY
{
    FLASH (rx) : ORIGIN = 0x08001000, LENGTH = 0x0007f000
    SRAM (rwx) : ORIGIN = 0x20000000, LENGTH = 0x00010000
}

SECTIONS
{
    .text :
    {
        _text = .;
        KEEP(*(.isr_vector))
        *(.text*)
        *(.rodata*)
       
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
    the_sidata = _etext;
    .data : AT(the_sidata)
    {
        the_data = .;
        *(vtable)
        *(.data*)
        the_edata = .;
    } > SRAM
    
    
    .bss :
    {
        the_bss = .;
        *(.bss*)
        the_ebss = .;
    } > SRAM
    	 
   	 heap_low = .; /* for _sbrk */
     . = . + 0x200; /* 1kB of heap memory */
     heap_top = .; /* for _sbrk */
}
