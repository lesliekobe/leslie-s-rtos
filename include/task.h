#ifndef TASK_H
#define TASK_H

#include "types.h"
#include "list.h"

#define TASK_NAME_LEN   32
#define TASK_PRIORITY_MAX  255
#define TASK_STACK_SIZE    2048

typedef enum {
    TASK_STATE_READY = 0,
    TASK_STATE_RUNNING,
    TASK_STATE_BLOCKED,
    TASK_STATE_SUSPENDED,
    TASK_STATE_DELETED,
} task_state_t;

/* Task control block */
typedef struct task {
    list_node_t node;
    u32         task_id;
    char        name[TASK_NAME_LEN];
    task_state_t state;
    u8          priority;
    u32         stack_size;
    void        *stack_base;
    u32         time_slice;
    u64         cpu_cycles;
    void        (*entry)(void *arg);
    void        *arg;
    u32         flags;
    u32         cpu_affinity;   /* Which CPU(s) can run this */
    u32         last_tick;
} task_t;

void     task_init(void);
task_t*  task_create(const char *name, void (*entry)(void*), void *arg, u8 priority, u32 stack_size);
void     task_delete(task_t *t);
void     task_set_priority(task_t *t, u8 prio);
void     task_resume(task_t *t);
void     task_suspend(task_t *t);
void     task_yield(void);
task_t*  task_current(void);
u32      task_next_id(void);
list_t*  task_list_get(void);

#endif /* TASK_H */