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