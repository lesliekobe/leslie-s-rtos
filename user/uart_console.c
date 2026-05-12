/*
 * UART Console Task
 * Handles command input and output
 */
#include "types.h"
#include "task.h"
#include "systick.h"
#include <stdio.h>
#include <string.h>

#define CMD_BUF_SIZE   64

typedef enum {
    CMD_OK,
    CMD_NOT_FOUND,
    CMD_TOO_LONG,
    CMD_INVALID_ARGS,
} cmd_status_t;

typedef cmd_status_t (*cmd_handler_t)(int argc, char **argv);

typedef struct {
    const char *name;
    cmd_handler_t handler;
    const char *help;
} command_t;

/* Command handlers */
static cmd_status_t cmd_help(int argc, char **argv);
static cmd_status_t cmd_info(int argc, char **argv);
static cmd_status_t cmd_tasks(int argc, char **argv);
static cmd_status_t cmd_time(int argc, char **argv);

static command_t g_commands[] = {
    {"help",  cmd_help,  "Show this help"},
    {"info",  cmd_info,  "Show system info"},
    {"tasks", cmd_tasks, "List all tasks"},
    {"time",  cmd_time, "Show uptime"},
};

#define NUM_COMMANDS (sizeof(g_commands) / sizeof(command_t))

static cmd_status_t cmd_help(int argc, char **argv) {
    (void)argc; (void)argv;
    printf("Available commands:\n");
    for (u32 i = 0; i < NUM_COMMANDS; i++) {
        printf("  %-10s - %s\n", g_commands[i].name, g_commands[i].help);
    }
    return CMD_OK;
}

static cmd_status_t cmd_info(int argc, char **argv) {
    (void)argc; (void)argv;
    printf("Leslie's RTOS v0.1\n");
    printf("Target: ARM Cortex-R52\n");
    printf("Scheduler: Priority + Round-Robin\n");
    printf("Build date: %s %s\n", __DATE__, __TIME__);
    return CMD_OK;
}

static cmd_status_t cmd_tasks(int argc, char **argv) {
    (void)argc; (void)argv;
    list_t *tasks = task_list_get();
    printf("Task list:\n");
    printf("%-4s %-16s %-8s %-4s %s\n", "ID", "Name", "State", "Prio", "CPU cycles");
    printf("-------------------------------------------------\n");
    
    list_foreach(tasks, task_t, node, t) {
        const char *state_str = "UNKNOWN";
        switch (t->state) {
            case TASK_STATE_READY:    state_str = "READY"; break;
            case TASK_STATE_RUNNING:  state_str = "RUNNING"; break;
            case TASK_STATE_BLOCKED:  state_str = "BLOCKED"; break;
            case TASK_STATE_SUSPENDED: state_str = "SUSPENDED"; break;
            case TASK_STATE_DELETED:  state_str = "DELETED"; break;
        }
        printf("%-4d %-16s %-8s %-4d %llu\n",
               t->task_id, t->name, state_str, t->priority, t->cpu_cycles);
    }
    return CMD_OK;
}

static cmd_status_t cmd_time(int argc, char **argv) {
    (void)argc; (void)argv;
    u64 ticks = systick_get_ticks();
    u64 ms = systick_ticks_to_ms(ticks);
    printf("Uptime: %llu ms (%llu.%llu seconds)\n",
           ms, ms / 1000, (ms % 1000));
    return CMD_OK;
}

static cmd_status_t parse_command(char *input) {
    char *argv[8];
    int argc = 0;
    
    /* Simple tokenization */
    char *token = strtok(input, " \r\n");
    while (token && argc < 8) {
        argv[argc++] = token;
        token = strtok(NULL, " \r\n");
    }
    
    if (argc == 0) return CMD_OK;
    
    for (u32 i = 0; i < NUM_COMMANDS; i++) {
        if (strcmp(argv[0], g_commands[i].name) == 0) {
            return g_commands[i].handler(argc, argv);
        }
    }
    
    printf("Unknown command: %s\n", argv[0]);
    return CMD_NOT_FOUND;
}

void uart_console_task(void *arg) {
    (void)arg;
    char cmd_buf[CMD_BUF_SIZE];
    int buf_pos = 0;
    
    printf("[CONSOLE] UART console task started\n");
    printf("[CONSOLE] Type 'help' for available commands\n");
    
    while (1) {
        /* Simulate receiving a character */
        systick_delay(100);
        
        /* Echo back simulated input for demo */
        if (buf_pos == 0) {
            printf("rtos> ");
        }
        
        /* Simulate some auto-commands for demo */
        if ((systick_get_ticks() % 100) == 0 && buf_pos == 0) {
            const char *demo_cmd = "tasks";
            printf("%s\n", demo_cmd);
            parse_command((char*)demo_cmd);
        }
        
        task_yield();
    }
}