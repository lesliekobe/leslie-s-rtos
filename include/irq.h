#ifndef IRQ_H
#define IRQ_H

#include "types.h"

/* ARM Cortex-R52 supports 480 interrupts (GICv2) */
#define IRQ_MAX      480
#define IRQ_SGI_MAX  16   /* Software Generated Interrupts */
#define IRQ_PPI_MAX  16   /* Private Peripheral Interrupts */

typedef enum {
    IRQ_TYPE_SGI = 0,   /* Software Generated */
    IRQ_TYPE_PPI,       /* Private Peripheral */
    IRQ_TYPE_SPI,       /* Shared Peripheral */
    IRQ_TYPE_LPI,       /* Locality-specific Peripheral (GICv4) */
} irq_type_t;

typedef enum {
    IRQ_LEVEL_LOW = 0,
    IRQ_LEVEL_HIGH,
    IRQ_EDGE_RISING,
    IRQ_EDGE_FALLING,
} irq_trigger_t;

/* IRQ handler return */
typedef enum {
    IRQ_NONE = 0,
    IRQ_HANDLED,
    IRQ_WAKE,
} irq_ret_t;

typedef irq_ret_t (*irq_handler_t)(u32 irq_num, void *ctx);

typedef struct {
    irq_handler_t handler;
    void          *ctx;
    irq_trigger_t trigger;
    u32           flags;
} irq_desc_t;

void   irq_init(void);
status_t irq_register(u32 irq_num, irq_handler_t handler, void *ctx, irq_trigger_t trigger);
status_t irq_unregister(u32 irq_num);
status_t irq_enable(u32 irq_num);
status_t irq_disable(u32 irq_num);
void   irq_global_enable(void);
void   irq_global_disable(void);
u32    irq_get_active(void);
void   irq_end_of_interrupt(u32 irq_num);

#endif /* IRQ_H */