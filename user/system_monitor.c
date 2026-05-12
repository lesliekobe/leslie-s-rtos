/*
 * System Monitor Task
 * Monitors system health: CPU usage, memory, task states
 */
#include "types.h"
#include "task.h"
#include "sched.h"
#include "systick.h"
#include "list.h"
#include <stdio.h>

#define MONITOR_INTERVAL_MS  1000
#define MEMORY_THRESHOLD     80  /* % */

typedef struct {
    u32 idle_count;
    u32 busy_count;
    u32 total_ticks;
    u32 mem_used;
    u32 peak_mem;
} system_stats_t;

static system_stats_t g_stats = {0};

void system_monitor_task(void *arg) {
    (void)arg;
    printf("[MONITOR] System monitor task started\n");
    
    u64 start_ticks = systick_get_ticks();
    u32 last_cpu_idle = 0;
    
    while (1) {
        systick_delay(MONITOR_INTERVAL_MS);
        
        u64 current_ticks = systick_get_ticks();
        u64 elapsed = current_ticks - start_ticks;
        
        /* Calculate CPU usage */
        task_t *idle = task_get_idle();
        u32 cpu_usage = 0;
        if (idle && elapsed > 0) {
            cpu_usage = (u32)((idle->cpu_cycles * 100) / elapsed);
        }
        
        /* Count tasks by state */
        list_t *tasks = task_list_get();
        u32 ready = 0, running = 0, blocked = 0;
        
        list_foreach(tasks, task_t, node, t) {
            switch (t->state) {
                case TASK_STATE_READY:   ready++; break;
                case TASK_STATE_RUNNING: running++; break;
                case TASK_STATE_BLOCKED: blocked++; break;
                default: break;
            }
        }
        
        printf("[MONITOR] === System Stats ===\n");
        printf("[MONITOR] CPU: %d%% used\n", cpu_usage);
        printf("[MONITOR] Memory: %d KB used\n", g_stats.mem_used / 1024);
        printf("[MONITOR] Tasks: READY=%d RUNNING=%d BLOCKED=%d\n",
               ready, running, blocked);
        printf("[MONITOR] Uptime: %llu ms\n", systick_ticks_to_ms(elapsed));
        printf("[MONITOR] ======================\n");
        
        /* Check for anomalies */
        if (cpu_usage > 95) {
            printf("[MONITOR] WARNING: High CPU usage!\n");
        }
        if (g_stats.mem_used > g_stats.peak_mem) {
            g_stats.peak_mem = g_stats.mem_used;
        }
    }
}