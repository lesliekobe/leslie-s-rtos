#ifndef SCHED_H
#define SCHED_H

#include "types.h"
#include "task.h"

#define SCHED_MAX_CPUS  4

/* Scheduler algorithm types */
typedef enum {
    SCHED_RR = 0,      /* Round-robin */
    SCHED_PRIORITY,    /* Priority based */
    SCHED_RR_PRIORITY, /* Priority with round-robin within same priority */
} sched_algorithm_t;

typedef struct {
    task_t    *current;
    u32       quantum;       /* time slice in ticks */
    u64       total_ticks;
} cpu_info_t;

extern cpu_info_t g_cpu_info[SCHED_MAX_CPUS];

void sched_init(sched_algorithm_t algo);
void sched_add_task(task_t *t);
void sched_remove_task(task_t *t);
void sched_tick(void);
task_t* sched_get_next(void);
void sched_switch(task_t *prev, task_t *next);
void sched_set_quantum(u32 ms);

#endif /* SCHED_H */