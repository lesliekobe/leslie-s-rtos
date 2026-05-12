/*
 * Priority-based Round-Robin Scheduler
 */
#include "types.h"
#include "list.h"
#include "task.h"
#include "sched.h"

cpu_info_t g_cpu_info[SCHED_MAX_CPUS] = {{0}};

#define PRIORITY_LEVELS  256
static list_t g_run_queues[PRIORITY_LEVELS];
static sched_algorithm_t g_sched_algo = SCHED_RR_PRIORITY;
static u32 g_quantum_ms = 10;

static task_t* pick_highest_runnable(void) {
    for (u32 p = 0; p < PRIORITY_LEVELS; p++) {
        if (!list_empty(&g_run_queues[p])) {
            list_node_t *n = list_pop(&g_run_queues[p]);
            return (task_t*)((char*)n - offsetof(task_t, node));
        }
    }
    return NULL;
}

void sched_init(sched_algorithm_t algo) {
    g_sched_algo = algo;
    
    for (u32 i = 0; i < PRIORITY_LEVELS; i++)
        list_init(&g_run_queues[i]);
    
    g_cpu_info[0].current = NULL;
    g_cpu_info[0].quantum = g_quantum_ms;
    g_cpu_info[0].total_ticks = 0;
    
    printf("[SCHED] Scheduler initialized (%s)\n",
           algo == SCHED_RR ? "Round-Robin" :
           algo == SCHED_PRIORITY ? "Priority" : "Priority+RR");
}

void sched_add_task(task_t *t) {
    if (!t) return;
    list_add_tail(&g_run_queues[t->priority], &t->node);
    printf("[SCHED] Task '%s' added to run queue (prio=%d)\n", t->name, t->priority);
}

void sched_remove_task(task_t *t) {
    if (!t) return;
    list_remove(&t->node);
}

void sched_tick(void) {
    g_cpu_info[0].total_ticks++;
    
    task_t *cur = g_cpu_info[0].current;
    if (cur) {
        cur->cpu_cycles++;
        cur->last_tick = (u32)g_cpu_info[0].total_ticks;
    }
}

task_t* sched_get_next(void) {
    return pick_highest_runnable();
}

task_t* sched_get_current_task(void) {
    return g_cpu_info[0].current;
}

void sched_set_quantum(u32 ms) {
    g_quantum_ms = ms;
    g_cpu_info[0].quantum = ms;
}

void sched_switch(task_t *prev, task_t *next) {
    if (prev == next) return;
    
    g_cpu_info[0].current = next;
    
    if (prev) prev->state = TASK_STATE_READY;
    if (next) {
        next->state = TASK_STATE_RUNNING;
        printf("[SCHED] Switch: %s -> %s\n",
               prev ? prev->name : "nil",
               next->name);
    }
}

void sched_yield(void) {
    task_t *cur = g_cpu_info[0].current;
    
    if (cur && cur->state == TASK_STATE_RUNNING) {
        /* Re-add to run queue for round-robin */
        list_add_tail(&g_run_queues[cur->priority], &cur->node);
    }
    
    task_t *next = pick_highest_runnable();
    if (next && next != cur) {
        sched_switch(cur, next);
    }
}

void sched_start(task_t *first) {
    if (!first) return;
    
    g_cpu_info[0].current = first;
    first->state = TASK_STATE_RUNNING;
    
    printf("[SCHED] Starting scheduler with task '%s'\n", first->name);
    
    /* Jump to first task */
    extern void arch_start_first(task_t *first);
    arch_start_first(first);
}