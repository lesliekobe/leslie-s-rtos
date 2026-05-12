/*
 * Temperature Sensor Task
 * Simulates temperature monitoring and warning system
 */
#include "types.h"
#include "task.h"
#include "systick.h"
#include <stdio.h>
#include <stdlib.h>

#define TEMP_SAMPLE_INTERVAL_MS  500
#define TEMP_WARNING_HIGH         75  /* Celsius */
#define TEMP_WARNING_LOW          10  /* Celsius */
#define TEMP_CRITICAL_HIGH        90  /* Celsius */

typedef enum {
    TEMP_STATUS_NORMAL,
    TEMP_STATUS_WARNING,
    TEMP_STATUS_CRITICAL,
    TEMP_STATUS_LOW,
} temp_status_t;

typedef struct {
    float current_temp;
    float min_temp;
    float max_temp;
    float avg_temp;
    u32   sample_count;
    temp_status_t status;
} temp_data_t;

static temp_data_t g_temp = {
    .current_temp = 25.0f,
    .min_temp = 25.0f,
    .max_temp = 25.0f,
    .avg_temp = 25.0f,
    .sample_count = 0,
    .status = TEMP_STATUS_NORMAL,
};

/* Simulate temperature fluctuations */
static float temp_simulate(float base) {
    float noise = (float)((rand() % 100) - 50) / 10.0f;  /* +/- 5 degrees */
    return base + noise;
}

void temperature_sensor_task(void *arg) {
    (void)arg;
    srand((u32)systick_get_ticks());  /* Seed random */
    
    printf("[TEMP] Temperature sensor task started\n");
    
    float base_temp = 35.0f;  /* Normal operating temp */
    
    while (1) {
        systick_delay(TEMP_SAMPLE_INTERVAL_MS);
        
        /* Simulate temperature reading */
        g_temp.current_temp = temp_simulate(base_temp);
        
        /* Track min/max */
        if (g_temp.current_temp < g_temp.min_temp)
            g_temp.min_temp = g_temp.current_temp;
        if (g_temp.current_temp > g_temp.max_temp)
            g_temp.max_temp = g_temp.current_temp;
        
        /* Calculate running average */
        g_temp.sample_count++;
        g_temp.avg_temp = g_temp.avg_temp * 0.95f + g_temp.current_temp * 0.05f;
        
        /* Determine status */
        temp_status_t prev_status = g_temp.status;
        if (g_temp.current_temp >= TEMP_CRITICAL_HIGH) {
            g_temp.status = TEMP_STATUS_CRITICAL;
        } else if (g_temp.current_temp >= TEMP_WARNING_HIGH) {
            g_temp.status = TEMP_STATUS_WARNING;
        } else if (g_temp.current_temp <= TEMP_WARNING_LOW) {
            g_temp.status = TEMP_STATUS_LOW;
        } else {
            g_temp.status = TEMP_STATUS_NORMAL;
        }
        
        /* Log when status changes */
        if (g_temp.status != prev_status) {
            printf("[TEMP] Status changed: %d -> %d\n", prev_status, g_temp.status);
        }
        
        /* Periodic report every 20 samples */
        if ((g_temp.sample_count % 20) == 0) {
            printf("[TEMP] Reading: %.1f C | Min: %.1f | Max: %.1f | Avg: %.1f | Status: %d\n",
                   g_temp.current_temp, g_temp.min_temp, g_temp.max_temp, g_temp.avg_temp, g_temp.status);
        }
        
        /* Simulate thermal load variations */
        if ((g_temp.sample_count % 100) == 0) {
            base_temp = 30.0f + (float)((rand() % 80) - 20);
        }
    }
}