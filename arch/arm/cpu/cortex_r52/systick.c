/*
 * ARM Cortex-R52 System Tick (Cortex-R52 Core Clock) driver
 */
#include "types.h"
#include "systick.h"

#define SYSTICK_BASE   0x2C010000
#define SYSTICK_CTRL   (SYSTICK_BASE + 0x00)
#define SYSTICK_LOAD   (SYSTICK_BASE + 0x04)
#define SYSTICK_VAL    (SYSTICK_BASE + 0x08)
#define SYSTICK_CALIB  (SYSTICK_BASE + 0x0C)

/* Control register bits */
#define SYSTICK_CTRL_EN         (1 << 0)
#define SYSTICK_CTRL_TICKINT    (1 << 1)
#define SYSTICK_CTRL_CLKSOURCE  (1 << 2)  /* 0=external, 1=processor clock */
#define SYSTICK_CTRL_COUNTFLAG  (1 << 16)

static u32 g_ticks_per_ms = 0;

u64 g_systick_ticks = 0;

void systick_init(u32 freq_hz) {
    g_ticks_per_ms = freq_hz / 1000;
    
    volatile u32 *load = (volatile u32 *)SYSTICK_LOAD;
    volatile u32 *ctrl = (volatile u32 *)SYSTICK_CTRL;
    
    /* Disable first */
    *ctrl = 0;
    
    /* Set reload value (freq / 100 for 10ms tick = 100Hz) */
    u32 reload = freq_hz / 100;
    *load = reload;
    
    /* Clear current value */
    volatile u32 *val = (volatile u32 *)SYSTICK_VAL;
    *val = 0;
    
    /* Enable with processor clock, tick interrupt */
    *ctrl = SYSTICK_CTRL_CLKSOURCE | SYSTICK_CTRL_TICKINT | SYSTICK_CTRL_EN;
}

void systick_start(void) {
    volatile u32 *ctrl = (volatile u32 *)SYSTICK_CTRL;
    *ctrl |= SYSTICK_CTRL_EN;
}

void systick_stop(void) {
    volatile u32 *ctrl = (volatile u32 *)SYSTICK_CTRL;
    *ctrl &= ~SYSTICK_CTRL_EN;
}

u64 systick_get_ticks(void) {
    return g_systick_ticks;
}

u64 systick_ticks_to_ms(u64 ticks) {
    return ticks;
}

u64 systick_ms_to_ticks(u64 ms) {
    return ms;
}

void systick_delay(u32 ms) {
    u64 start = systick_get_ticks();
    while ((systick_get_ticks() - start) < ms) {
        yield();
    }
}

/* SysTick exception handler - called on each tick */
void systick_handler(void) {
    g_systick_ticks++;
    extern void sched_tick(void);
    sched_tick();
}