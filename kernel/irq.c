/*
 * IRQ management
 */
#include "types.h"
#include "irq.h"

static irq_desc_t g_irq_descs[IRQ_MAX];

void irq_init(void) {
    for (u32 i = 0; i < IRQ_MAX; i++) {
        g_irq_descs[i].handler = NULL;
        g_irq_descs[i].ctx = NULL;
        g_irq_descs[i].trigger = IRQ_LEVEL_HIGH;
        g_irq_descs[i].flags = 0;
    }
    printf("[IRQ] Interrupt subsystem initialized\n");
}

status_t irq_register(u32 irq_num, irq_handler_t handler, void *ctx, irq_trigger_t trigger) {
    if (irq_num >= IRQ_MAX) {
        printf("[IRQ] ERROR: IRQ %d out of range\n", irq_num);
        return ERR_INVALID;
    }
    if (!handler) {
        printf("[IRQ] ERROR: NULL handler for IRQ %d\n", irq_num);
        return ERR_INVALID;
    }
    
    g_irq_descs[irq_num].handler = handler;
    g_irq_descs[irq_num].ctx = ctx;
    g_irq_descs[irq_num].trigger = trigger;
    
    printf("[IRQ] Registered handler for IRQ %d\n", irq_num);
    return OK;
}

status_t irq_unregister(u32 irq_num) {
    if (irq_num >= IRQ_MAX) return ERR_INVALID;
    g_irq_descs[irq_num].handler = NULL;
    g_irq_descs[irq_num].ctx = NULL;
    return OK;
}

status_t irq_enable(u32 irq_num) {
    if (irq_num >= IRQ_MAX) return ERR_INVALID;
    extern void gic_enable_irq(u32 irq_num);
    gic_enable_irq(irq_num);
    return OK;
}

status_t irq_disable(u32 irq_num) {
    if (irq_num >= IRQ_MAX) return ERR_INVALID;
    extern void gic_disable_irq(u32 irq_num);
    gic_disable_irq(irq_num);
    return OK;
}

void irq_global_enable(void) {
    extern void enable_irq(void);
    enable_irq();
}

void irq_global_disable(void) {
    extern void disable_irq(void);
    disable_irq();
}

irq_ret_t irq_dispatch(u32 irq_num) {
    if (irq_num < IRQ_MAX && g_irq_descs[irq_num].handler) {
        return g_irq_descs[irq_num].handler(irq_num, g_irq_descs[irq_num].ctx);
    }
    return IRQ_NONE;
}