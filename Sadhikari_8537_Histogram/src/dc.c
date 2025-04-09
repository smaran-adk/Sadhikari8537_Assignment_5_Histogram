#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <signal.h>
#include "../include/histo.h"

SharedMemory* shm;
volatile sig_atomic_t should_exit = 0;
int shm_fd;

void print_histogram() {
    system("clear");
    printf("\nHistogram:\n");
    for (int i = 0; i < 20; i++) {
        printf("%c: ", 'A' + i);
        int count = shm->letter_count[i];

        for (int j = 0; j < count; j++) {
            if (j % 100 == 0 && j > 0) printf("*");
            else if (j % 10 == 0 && j > 0) printf("+");
            else printf("-");
        }
        printf("\n");
    }
}

void cleanup(int sig) {
    should_exit = 1;
}

int main() {
    signal(SIGINT, cleanup);

    shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    shm = (SharedMemory*)mmap(0, sizeof(SharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    while (!should_exit) {
        sem_wait(&shm->mutex);

        while (shm->read_index != shm->write_index) {
            char letter = shm->buffer[shm->read_index];
            shm->letter_count[letter - 'A']++;
            shm->read_index = (shm->read_index + 1) % BUFFER_SIZE;
        }

        sem_post(&shm->mutex);
        sleep(10);
        print_histogram();
    }


    sem_wait(&shm->mutex);
    while (shm->read_index != shm->write_index) {
        char letter = shm->buffer[shm->read_index];
        shm->letter_count[letter - 'A']++;
        shm->read_index = (shm->read_index + 1) % BUFFER_SIZE;
    }
    sem_post(&shm->mutex);


    print_histogram();
    printf("\nShazam !!\n");
    fflush(stdout);


    munmap(shm, sizeof(SharedMemory));
    close(shm_fd);
    return 0;
}
