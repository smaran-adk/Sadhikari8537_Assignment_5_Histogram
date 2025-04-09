#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <time.h>
#include <signal.h>
#include <sys/wait.h>
#include "../include/histo.h"

SharedMemory* shm;
pid_t dc_pid;
int shm_fd;
volatile sig_atomic_t running = 1;

void cleanup(int sig) {
    running = 0;
    if (dc_pid > 0) {
        kill(dc_pid, SIGINT);
        waitpid(dc_pid, NULL, 0);
    }
    munmap(shm, sizeof(SharedMemory));
    close(shm_fd);
    exit(0);
}

int main() {
    pid_t pid;

    signal(SIGINT, cleanup);

    shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    shm = (SharedMemory*)mmap(0, sizeof(SharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    srand(time(NULL));

    pid = fork();
    if (pid == 0) {
        execlp("./bin/dc", "dc", NULL);
        perror("execlp failed");
        exit(1);
    }
    dc_pid = pid;
    shm->dc_pid = dc_pid;

    while (running) {
        sem_wait(&shm->mutex);
        char letter = 'A' + (rand() % 20);
        shm->buffer[shm->write_index] = letter;
        shm->write_index = (shm->write_index + 1) % BUFFER_SIZE;
        sem_post(&shm->mutex);
        usleep(50000);
    }

    return 0;
}