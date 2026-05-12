/*
 * Binary Semaphore for RTOS
 */
#include "types.h"

typedef struct sem {
    u32 count;
    u32 max_count;
} sem_t;

static inline void sem_init(sem_t *sem, u32 initial_count, u32 max_count) {
    sem->count = initial_count;
    sem->max_count = max_count;
}

static inline status_t sem_take(sem_t *sem) {
    if (sem->count > 0) {
        sem->count--;
        return OK;
    }
    return ERR_BUSY;
}

static inline status_t sem_give(sem_t *sem) {
    if (sem->count < sem->max_count) {
        sem->count++;
        return OK;
    }
    return ERR_BUSY;
}

static inline u32 sem_count(sem_t *sem) {
    return sem->count;
}