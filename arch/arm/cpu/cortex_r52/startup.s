/*
 * ARM Cortex-R52 startup code
 */
.syntax unified
.cpu cortex-r52

.section .vectors, "ax"
.global _start
.global _vector_table

/* Vector table */
_vector_table:
    b _start          /* Reset */
    b .L_undefined    /* Undefined instruction */
    b .L_svc          /* SVC call */
    b .L_prefetch_abort /* Prefetch abort */
    b .L_data_abort   /* Data abort */
    b .L_reserved     /* Reserved */
    b .L_irq          /* IRQ interrupt */
    b .L_fiq          /* FIQ interrupt */

.text
.align 3

_start:
    /* Set up stack pointers for each mode */
    cpsr_mode SYS_MODE
    
    /* Initialize stacks for different modes */
    ldr sp, =_estack
    
    cpsr_mode IRQ_MODE
    ldr sp, =_irq_stack
    
    cpsr_mode FIQ_MODE
    ldr sp, =_fiq_stack
    
    cpsr_mode SVC_MODE
    ldr sp, =_svc_stack
    
    /* Switch to system mode (full access) */
    cpsr_mode SYS_MODE
    ldr sp, =_estack
    
    /* Disable caches and MMU for now */
    mrc p15, 0, r0, c1, c0, 0
    bic r0, r0, #0x1
    bic r0, r0, #0x4
    bic r0, r0, #0x1000
    mcr p15, 0, r0, c1, c0, 0
    
    /* Invalidate caches */
    mov r0, #0
    mcr p15, 0, r0, c7, c5, 0    /* Invalidate icache */
    mcr p15, 0, r0, c7, c6, 0    /* Invalidate dcache */
    
    /* Invalidate TLB */
    mcr p15, 0, r0, c8, c7, 0
    
    /* Call kernel main */
    bl kernel_main

.L_undefined:
    b .L_undefined

.L_svc:
    b .L_svc

.L_prefetch_abort:
    b .L_prefetch_abort

.L_data_abort:
    b .L_data_abort

.L_reserved:
    b .L_reserved

.L_irq:
    /* Save context */
    sub lr, lr, #4
    stmfd sp!, {r0-r12, lr}
    mrs r0, spsr
    stmfd sp!, {r0, lr}
    
    /* Call IRQ handler */
    bl irq_handler
    
    /* Restore context */
    ldmfd sp!, {r0, lr}
    msr spsr, r0
    ldmfd sp!, {r0-r12, pc}^

.L_fiq:
    b .L_fiq

/* Macro to switch to a different CPU mode */
.macro cpsr_mode mode
    msr cpsr_c, #\mode | 0xDF
.endm

/* Fill unused interrupt vectors with default handlers */
.rept 8
    b .
.endr