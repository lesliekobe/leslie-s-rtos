/*
 * ARM Cortex-R52 Linker Script
 * For RTOS running on cortex-r52
 */

ENTRY(_start)

/* Memory layout - customize for your board */
MEMORY {
    /* ROM - where we load */
    ROM (rx) : ORIGIN = 0x00000000, LENGTH = 64M
    
    /* RAM */
    RAM (rwx) : ORIGIN = 0x80000000, LENGTH = 64M
    
    /* Device memory */
    DEVICE (rwx) : ORIGIN = 0x2C000000, LENGTH = 64M
}

/* Stack sizes */
_stack_size = 4096;
_irq_stack_size = 1024;
_fiq_stack_size = 1024;
_svc_stack_size = 1024;

SECTIONS {
    /* Vector table must be at address 0 */
    .vectors 0x00000000 : {
        *(.vectors)
        *(.vector_table)
    } > ROM

    .text : {
        *(.text*)
        *(.rodata*)
        *(.constdata*)
        . = ALIGN(4);
    } > ROM

    .data : {
        *(.data*)
        . = ALIGN(4);
    } > RAM AT > ROM

    .bss : {
        *(.bss*)
        . = ALIGN(4);
    } > RAM

    /* Stack areas */
    .stack (NOLOAD) : {
        . = ALIGN(8);
        _estack = .;
        . = . + _stack_size;
        
        . = ALIGN(8);
        _irq_stack = .;
        . = . + _irq_stack_size;
        
        . = ALIGN(8);
        _fiq_stack = .;
        . = . + _fiq_stack_size;
        
        . = ALIGN(8);
        _svc_stack = .;
        . = . + _svc_stack_size;
    } > RAM

    /* Heap - simple bump allocator for now */
    _heap_start = .;
    _heap_size = 16M;
    . = . + _heap_size;
    _heap_end = .;
}