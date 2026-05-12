/*
 * RTOS Kernel - Main entry point
 */
#include "types.h"
#include "task.h"
#include "sched.h"
#include "irq.h"
#include "systick.h"
#include "user/user_tasks.h"

extern void arch_cpu_init(void);
extern void arch_idle(void);

void idle_task(void *arg) {
    (void)arg;
    printf("[IDLE] Idle task started\n");
    
    int count = 0;
    while (1) {
        count++;
        if ((count % 100) == 0) {
            printf("[IDLE] Running... count=%d\n", count);
        }
        
        /* Yield to other tasks */
        task_yield();
    }
}

void kernel_main(void) {
    printf("\n");
    printf("========================================\n");
    printf("   Leslie's RTOS for ARM Cortex-R52\n");
    printf("========================================\n\n");
    
    /* Initialize architecture */
    arch_cpu_init();
    printf("[KERNEL] ARM Cortex-R52 initialized\n");
    
    /* Initialize subsystems */
    task_init();
    sched_init(SCHED_RR_PRIORITY);
    irq_init();
    systick_init(100000000);  /* 100 MHz */
    
    printf("[KERNEL] All subsystems initialized\n\n");
    
    /* Create idle task */
    task_t *idle = task_create("idle", idle_task, NULL, 255, 2048);
    if (!idle) {
        printf("[KERNEL] FATAL: Failed to create idle task\n");
        while (1);
    }
    
    task_set_idle(idle);
    
    printf("[KERNEL] Idle task created (ID=%d)\n\n", idle->task_id);
    
    /* User tasks */
    extern void user_tasks_init(void);
    user_tasks_init();
    printf("[KERNEL] User tasks initialized\n\n");
    
    /* Start system timer */
    systick_start();
    printf("[KERNEL] System timer started\n");
    
    /* Start scheduler */
    printf("[KERNEL] Starting scheduler...\n");
    sched_start(idle);
    
    /* Should never reach here */
    while (1);
}