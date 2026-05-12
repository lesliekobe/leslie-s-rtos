/*
 * ARM Cortex-R52 interrupt controller (GICv2) support
 */
#include "types.h"
#include "irq.h"
#include "../arch.h"

#define GIC_BASE        0x2C000000  /* ARM Cortex-R52 GIC base */
#define GICC_OFFSET     0x1000      /* CPU interface */
#define GICD_OFFSET     0x0000      /* Distributor */

#define GICD_CTLR       (GIC_BASE + GICD_OFFSET + 0x000)
#define GICD_TYPER      (GIC_BASE + GICD_OFFSET + 0x004)
#define GICD_IGROUPR    (GIC_BASE + GICD_OFFSET + 0x080)
#define GICD_ISENABLER  (GIC_BASE + GICD_OFFSET + 0x100)
#define GICD_ICENABLER  (GIC_BASE + GICD_OFFSET + 0x180)
#define GICD_IPRIORITYR (GIC_BASE + GICD_OFFSET + 0x400)
#define GICD_ITARGETSR  (GIC_BASE + GICD_OFFSET + 0x800)
#define GICD_ICFGR      (GIC_BASE + GICD_OFFSET + 0xC00)

#define GICC_CTLR       (GIC_BASE + GICC_OFFSET + 0x000)
#define GICC_PMR        (GIC_BASE + GICC_OFFSET + 0x004)
#define GICC_BPR        (GIC_BASE + GICC_OFFSET + 0x008)
#define GICC_IAR        (GIC_BASE + GICC_OFFSET + 0x00C)
#define GICC_EOIR       (GIC_BASE + GICC_OFFSET + 0x010)

/* GICD_CTLR bits */
#define GICD_ENABLE     (1 << 0)
#define GICD_DISABLE    (0 << 0)

/* GICC_CTLR bits */
#define GICC_ENABLE     (1 << 0)
#define GICC_DISABLE    (0 << 0)

static irq_desc_t g_irq_descs[IRQ_MAX];

void gic_init(void) {
    volatile u32 *dist = (volatile u32 *)GICD_CTLR;
    volatile u32 *cpu = (volatile u32 *)GICC_CTLR;
    
    /* Disable distributor and CPU interface during init */
    *dist = GICD_DISABLE;
    *cpu = GICC_DISABLE;
    
    /* Clear any pending interrupts */
    for (u32 i = 0; i < (IRQ_MAX / 32); i++) {
        *(volatile u32 *)(GICD_ICENABLER + i * 4) = 0xFFFFFFFF;
    }
    
    /* Set all interrupts to group 0 (FIQ) */
    for (u32 i = 0; i < (IRQ_MAX / 32); i++) {
        *(volatile u32 *)(GICD_IGROUPR + i * 4) = 0x00000000;
    }
    
    /* Set priority mask (allow all priorities) */
    *(volatile u32 *)GICC_PMR = 0xFF;
    
    /* Binary point - split priority field */
    *(volatile u32 *)GICC_BPR = 3;
    
    /* Enable distributor and CPU interface */
    *dist = GICD_ENABLE;
    *cpu = GICC_ENABLE;
    
    /* Enable IRQ and FIQ */
    enable_irq();
    enable_fiq();
}

static void gic_enable_irq(u32 irq_num) {
    if (irq_num >= IRQ_MAX) return;
    u32 reg = irq_num / 32;
    u32 bit = irq_num % 32;
    volatile u32 *enable = (volatile u32 *)(GICD_ISENABLER + reg * 4);
    *enable = (1 << bit);
}

static void gic_disable_irq(u32 irq_num) {
    if (irq_num >= IRQ_MAX) return;
    u32 reg = irq_num / 32;
    u32 bit = irq_num % 32;
    volatile u32 *disable = (volatile u32 *)(GICD_ICENABLER + reg * 4);
    *disable = (1 << bit);
}

void arch_cpu_init(void) {
    gic_init();
}

void arch_idle(void) {
    while (1) {
        wfi();
        yield();
    }
}

u32 gic_get_active_irq(void) {
    volatile u32 *iar = (volatile u32 *)GICC_IAR;
    return *iar & 0x3FF;
}

void gic_end_irq(u32 irq_num) {
    volatile u32 *eoir = (volatile u32 *)GICC_EOIR;
    *eoir = irq_num;
}

void irq_handler(void) {
    u32 irq_num = gic_get_active_irq();
    
    if (irq_num < IRQ_MAX && g_irq_descs[irq_num].handler) {
        irq_ret_t ret = g_irq_descs[irq_num].handler(irq_num, g_irq_descs[irq_num].ctx);
        (void)ret;
    }
    
    gic_end_irq(irq_num);
}