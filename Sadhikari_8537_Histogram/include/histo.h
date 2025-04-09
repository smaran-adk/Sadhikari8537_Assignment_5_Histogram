#ifndef HISTO_H
#define HISTO_H

#include <semaphore.h>

#define BUFFER_SIZE 256
#define SHM_NAME "/histo_shm"

typedef struct {
    char buffer[BUFFER_SIZE];
    int write_index;
    int read_index;
    int letter_count[20];
    sem_t mutex;
    pid_t dc_pid;
} SharedMemory;

#endif
