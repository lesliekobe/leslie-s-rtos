#ifndef SYSTICK_H
#define SYSTICK_H

#include "types.h"

void systick_init(u32 freq_hz);
void systick_start(void);
void systick_stop(void);
u64  systick_get_ticks(void);
u64  systick_ticks_to_ms(u64 ticks);
u64  systick_ms_to_ticks(u64 ms);
void systick_delay(u32 ms);

#endif /* SYSTICK_H */