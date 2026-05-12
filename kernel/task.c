/*
 * Task management - create, delete, schedule tasks
 */
#include "types.h"
#include "list.h"
#include "task.h"
#include "sched.h"
#include <string.h>
#include <stdio.h>

#define TASK_HASH_SIZE   64

static task_t *g_task_hash[TASK_HASH_SIZE];
static list_t  g_all_tasks;
static u32     g_next_task_id = 1;
static task_t *g_idle_task = NULL;

static task_t g_task_pool[32];
static u32    g_task_alloc_next = 0;

static u32 task_hash(u32 id) { return id % TASK_HASH_SIZE; }

void task_init(void) {
    for (u32 i = 0; i < TASK_HASH_SIZE; i++)
        g_task_hash[i] = NULL;
    list_init(&g_all_tasks);
    g_next_task_id = 1;
    g_task_alloc_next = 0;
    printf("[TASK] Task subsystem initialized\n");
}

static task_t* task_alloc(void) {
    if (g_task_alloc_next >= 32) {
        printf("[TASK] ERROR: Task pool exhausted\n");
        return NULL;
    }
    task_t *t = &g_task_pool[g_task_alloc_next++];
    memset(t, 0, sizeof(task_t));
    return t;
}

task_t* task_create(const char *name, void (*entry)(void*), void *arg, u8 priority, u32 stack_size) {
    if (!entry) {
        printf("[TASK] ERROR: NULL entry point\n");
        return NULL;
    }
    
    task_t *t = task_alloc();
    if (!t) return NULL;
    
    t->task_id = g_next_task_id++;
    strncpy(t->name, name, TASK_NAME_LEN - 1);
    t->name[TASK_NAME_LEN - 1] = '\0';
    t->state = TASK_STATE_READY;
    t->priority = priority;
    t->stack_size = stack_size;
    t->stack_base = malloc(stack_size);
    if (!t->stack_base) {
        printf("[TASK] ERROR: Failed to allocate stack for %s\n", name);
        return NULL;
    }
    
    t->time_slice = 10;
    t->cpu_cycles = 0;
    t->entry = entry;
    t->arg = arg;
    t->flags = 0;
    t->cpu_affinity = 0x1;  /* CPU0 only by default */
    
    /* Initialize context */
    extern void arch_context_init(void *ctx, void *stack_top, void (*entry)(void *), void *arg);
    arch_context_init(t->stack_base + stack_size, t->stack_base + stack_size, entry, arg);
    
    /* Add to hash and list */
    u32 idx = task_hash(t->task_id);
    t->node.next = (list_node_t*)g_task_hash[idx];
    g_task_hash[idx] = t;
    
    list_add_tail(&g_all_tasks, &t->node);
    sched_add_task(t);
    
    printf("[TASK] Created task '%s' (ID=%d, prio=%d, stack=%d)\n",
           name, t->task_id, priority, stack_size);
    
    return t;
}

void task_delete(task_t *t) {
    if (!t) return;
    
    printf("[TASK] Deleting task '%s' (ID=%d)\n", t->name, t->task_id);
    
    t->state = TASK_STATE_DELETED;
    sched_remove_task(t);
    
    u32 idx = task_hash(t->task_id);
    task_t **p = &g_task_hash[idx];
    while (*p) {
        if (*p == t) {
            *p = (task_t*)(*p)->node.next;
            break;
        }
        p = (task_t**)&(*p)->node.next;
    }
    
    list_remove(&t->node);
    
    if (t->stack_base)
        free(t->stack_base);
}

void task_set_priority(task_t *t, u8 prio) {
    if (!t) return;
    t->priority = prio;
}

void task_resume(task_t *t) {
    if (!t) return;
    if (t->state == TASK_STATE_SUSPENDED || t->state == TASK_STATE_BLOCKED) {
        t->state = TASK_STATE_READY;
        sched_add_task(t);
    }
}

void task_suspend(task_t *t) {
    if (!t) return;
    t->state = TASK_STATE_SUSPENDED;
    sched_remove_task(t);
}

void task_yield(void) {
    extern void sched_yield(void);
    sched_yield();
}

task_t* task_current(void) {
    extern task_t* sched_get_current_task(void);
    return sched_get_current_task();
}

u32 task_next_id(void) { return g_next_task_id; }

list_t* task_list_get(void) { return &g_all_tasks; }

task_t* task_find_by_id(u32 id) {
    u32 idx = task_hash(id);
    task_t *t = g_task_hash[idx];
    while (t) {
        if (t->task_id == id) return t;
        t = (task_t*)t->node.next;
    }
    return NULL;
}

task_t* task_find_by_name(const char *name) {
    list_foreach(&g_all_tasks, task_t, node, t) {
        if (strncmp(t->name, name, TASK_NAME_LEN) == 0)
            return t;
    }
    return NULL;
}

task_t* task_get_idle(void) { return g_idle_task; }

void task_set_idle(task_t *t) { g_idle_task = t; }