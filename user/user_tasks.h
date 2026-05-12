/*
 * User task definitions
 * Include all user-space tasks here
 */
#ifndef USER_TASKS_H
#define USER_TASKS_H

#include "task.h"

/* Task priorities */
#define PRIORITY_SYSTEM_MONITOR  50
#define PRIORITY_TEMP_SENSOR     60
#define PRIORITY_HEARTBEAT       40
#define PRIORITY_CONSOLE         30

/* Task stack sizes */
#define STACK_SIZE_SMALL   1024
#define STACK_SIZE_MEDIUM  2048
#define STACK_SIZE_LARGE   4096

/* Task declarations */
extern void system_monitor_task(void *arg);
extern void temperature_sensor_task(void *arg);
extern void heartbeat_task(void *arg);
extern void uart_console_task(void *arg);

/* Task creation helper */
static inline task_t* create_user_task(const char *name, void (*entry)(void*),
                                       void *arg, u8 priority, u32 stack_size) {
    task_t *t = task_create(name, entry, arg, priority, stack_size);
    if (t) {
        printf("[USER] Created task: %s (prio=%d, stack=%d)\n",
               name, priority, stack_size);
    }
    return t;
}

/* Initialize all user tasks */
static inline void user_tasks_init(void) {
    printf("[USER] Initializing user tasks...\n");
    
    task_t *t;
    
    t = create_user_task("sys_monitor", system_monitor_task, NULL,
                         PRIORITY_SYSTEM_MONITOR, STACK_SIZE_MEDIUM);
    (void)t;
    
    t = create_user_task("temp_sensor", temperature_sensor_task, NULL,
                         PRIORITY_TEMP_SENSOR, STACK_SIZE_MEDIUM);
    (void)t;
    
    t = create_user_task("heartbeat", heartbeat_task, NULL,
                         PRIORITY_HEARTBEAT, STACK_SIZE_SMALL);
    (void)t;
    
    t = create_user_task("console", uart_console_task, NULL,
                         PRIORITY_CONSOLE, STACK_SIZE_LARGE);
    (void)t;
    
    printf("[USER] All user tasks initialized\n");
}

#endif /* USER_TASKS_H */