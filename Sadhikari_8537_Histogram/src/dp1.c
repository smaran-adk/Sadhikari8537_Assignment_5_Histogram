#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <time.h>
#include <signal.h>
#include "../include/histo.h"

SharedMemory* shm;
pid_t dp2_pid;
int shm_fd;
volatile sig_atomic_t running = 1;

void cleanup(int sig) {
    running = 0;
    if (dp2_pid > 0) {
        kill(dp2_pid, SIGINT);
        waitpid(dp2_pid, NULL, 0);
    }
    munmap(shm, sizeof(SharedMemory));
    shm_unlink(SHM_NAME);
    close(shm_fd);
    exit(0);
}

int main() {
    pid_t pid;

    signal(SIGINT, cleanup);

    shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, sizeof(SharedMemory));
    shm = (SharedMemory*)mmap(0, sizeof(SharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    shm->write_index = 0;
    shm->read_index = 0;
    for (int i = 0; i < 20; i++) shm->letter_count[i] = 0;
    sem_init(&shm->mutex, 1, 1);
    srand(time(NULL));

    pid = fork();
    if (pid == 0) {
        execlp("./bin/dp2", "dp2", NULL);
        perror("execlp failed");
        exit(1);
    }
    dp2_pid = pid;
    shm->dp1_pid = getpid();
    shm->dp2_pid = dp2_pid;

    while (running) {
        sem_wait(&shm->mutex);
        for (int i = 0; i < 20; i++) {
            char letter = 'A' + (rand() % 20);
            shm->buffer[shm->write_index] = letter;
            shm->write_index = (shm->write_index + 1) % BUFFER_SIZE;
        }
        sem_post(&shm->mutex);
        sleep(2);
    }

    return 0;
}