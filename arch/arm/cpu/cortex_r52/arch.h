/*
 * ARM Cortex-R52 architecture support for RTOS
 */
#include "types.h"

#define CPSR_MODE_USER      0x10
#define CPSR_MODE_FIQ       0x11
#define CPSR_MODE_IRQ       0x12
#define CPSR_MODE_SVC       0x13
#define CPSR_MODE_ABORT     0x17
#define CPSR_MODE_UNDEFINED 0x1B
#define CPSR_MODE_SYSTEM    0x1F

#define CPSR_I_BIT          (1 << 7)   /* IRQ disable */
#define CPSR_F_BIT          (1 << 6)   /* FIQ disable */

/* Inline assembly helpers */
static inline u32 get_cpsr(void) {
    u32 val;
    __asm__ volatile ("mrs %0, cpsr" : "=r"(val));
    return val;
}

static inline void set_cpsr(u32 val) {
    __asm__ volatile ("msr cpsr, %0" : : "r"(val));
}

static inline u32 get_sp(void) {
    u32 val;
    __asm__ volatile ("mov %0, sp" : "=r"(val));
    return val;
}

static inline u32 get_lr(void) {
    u32 val;
    __asm__ volatile ("mov %0, lr" : "=r"(val));
    return val;
}

static inline void enable_irq(void) {
    __asm__ volatile ("cpsie i" : : : "memory");
}

static inline void disable_irq(void) {
    __asm__ volatile ("cpsid i" : : : "memory");
}

static inline void enable_fiq(void) {
    __asm__ volatile ("cpsie f" : : : "memory");
}

static inline void disable_fiq(void) {
    __asm__ volatile ("cpsid f" : : : "memory");
}

static inline void isb(void) {
    __asm__ volatile ("isb" : : : "memory");
}

static inline void dsb(void) {
    __asm__ volatile ("dsb" : : : "memory");
}

static inline void sev(void) {
    __asm__ volatile ("sev" : : : "memory");
}

static inline void wfi(void) {
    __asm__ volatile ("wfi" : : : "memory");
}

static inline void yield(void) {
    __asm__ volatile ("yield" : : : "memory");
}

/* Memory barrier */
#define mb()      dsb()
#define rmb()     dsb()
#define wmb()     dsb()

/* Cache operations */
static inline void invalidate_icache(void) {
    __asm__ volatile ("icache ivau, %0" : : "r"(0) : "memory");
}

static inline void invalidate_dcache(void) {
    __asm__ volatile ("ic ivau, %0" : : "r"(0) : "memory");
}

static inline void flush_dcache(void) {
    __asm__ volatile ("dc cvau, %0" : : "r"(0) : "memory");
}

/* MMU operations */
static inline void set_ttbr0(u32 val) {
    __asm__ volatile ("mcr p15, 0, %0, c2, c0, 0" : : "r"(val) : "memory");
}

static inline u32 get_ttbr0(void) {
    u32 val;
    __asm__ volatile ("mrc p15, 0, %0, c2, c0, 0" : "=r"(val));
    return val;
}

/* SCS stack frame - pushed on exception entry */
typedef struct exc_frame {
    u32 r0, r1, r2, r3, r4, r5, r6, r7;
    u32 r8, r9, r10, r11, r12, lr, pc, cpsr;
} exc_frame_t;

/* Task context - saved on task switch */
typedef struct context {
    u32 sp;       /* Stack pointer */
    u32 lr;       /* Link register */
    u32 pc;       /* Program counter */
    u32 cpsr;     /* Program status */
    u32 spsr;     /* Saved PSR */
    u32 r4-r11;
} context_t;

void arch_context_init(context_t *ctx, void *stack_top, void (*entry)(void *), void *arg);
void arch_context_switch(context_t *prev, context_t *next);
void arch_cpu_init(void);
void arch_cpu_reset(void);
void arch_idle(void);

void svc_handler(void);
void irq_handler(void);
void fiq_handler(void);