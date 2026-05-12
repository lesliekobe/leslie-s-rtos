/*
 * Heartbeat Task
 * Sends periodic heartbeat messages and monitors system liveness
 */
#include "types.h"
#include "task.h"
#include "systick.h"
#include "semphr.h"
#include <stdio.h>

#define HEARTBEAT_INTERVAL_MS  1000

typedef struct {
    u32 sequence;
    u64 last_sent_time;
    u64 last_ack_time;
    u32 missed_acks;
    u32 errors;
} heartbeat_data_t;

static heartbeat_data_t g_hb = {
    .sequence = 0,
    .last_sent_time = 0,
    .last_ack_time = 0,
    .missed_acks = 0,
    .errors = 0,
};

void heartbeat_task(void *arg) {
    (void)arg;
    printf("[HEARTBEAT] Heartbeat task started\n");
    
    while (1) {
        systick_delay(HEARTBEAT_INTERVAL_MS);
        
        g_hb.sequence++;
        g_hb.last_sent_time = systick_get_ticks();
        
        task_t *current = task_current();
        u32 current_pid = current ? current->task_id : 0;
        
        printf("[HEARTBEAT] #%u sent at tick %llu | task_id=%d\n",
               g_hb.sequence, g_hb.last_sent_time, current_pid);
        
        /* Simulate ACK reception (in real system, would wait for it) */
        if ((g_hb.sequence % 50) == 0) {
            /* Simulate occasional missed ACK */
            g_hb.missed_acks++;
            printf("[HEARTBEAT] Missed ACK #%u\n", g_hb.sequence);
        }
        
        g_hb.last_ack_time = systick_get_ticks();
        
        /* Health check every 100 heartbeats */
        if ((g_hb.sequence % 100) == 0) {
            printf("[HEARTBEAT] Health report: seq=%u missed=%u errors=%u\n",
                   g_hb.sequence, g_hb.missed_acks, g_hb.errors);
            
            if (g_hb.missed_acks > 5) {
                printf("[HEARTBEAT] WARNING: Multiple missed ACKs detected!\n");
            }
        }
    }
}