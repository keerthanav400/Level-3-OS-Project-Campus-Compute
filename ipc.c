#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <semaphore.h>
#include "ipc.h"

static sem_t* global_sem = NULL;
static int shm_fd = -1;

SharedData* init_shared_memory(int create_new) {
    if (create_new) {
        // Unlink any leftover shared memory segment from previous abrupt terminations
        shm_unlink(SHM_NAME);
        
        shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
        if (shm_fd == -1) {
            perror("[Error] shm_open failed");
            return NULL;
        }

        // Set the size of the shared memory object
        if (ftruncate(shm_fd, sizeof(SharedData)) == -1) {
            perror("[Error] ftruncate failed");
            close(shm_fd);
            return NULL;
        }
    } else {
        // Open existing shared memory segment
        shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
        if (shm_fd == -1) {
            perror("[Error] shm_open (existing) failed");
            return NULL;
        }
    }

    // Map shared memory into process address space
    void* ptr = mmap(NULL, sizeof(SharedData), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED) {
        perror("[Error] mmap failed");
        close(shm_fd);
        return NULL;
    }

    SharedData* shm_data = (SharedData*)ptr;

    // If creator, initialize default fields
    if (create_new) {
        shm_data->active_jobs_count = 0;
        shm_data->system_load_status = 0;
        snprintf(shm_data->latest_message, sizeof(shm_data->latest_message), "Initialized Lab IPC");
    }

    return shm_data;
}

int init_semaphore(void) {
    // Unlink stale named semaphore just in case
    sem_unlink(SEM_NAME);

    // Create a named semaphore with initial value of 1 (binary mutex behavior)
    global_sem = sem_open(SEM_NAME, O_CREAT | O_RDWR, 0666, 1);
    if (global_sem == SEM_FAILED) {
        perror("[Error] sem_open failed");
        return 0;
    }

    return 1;
}

void ipc_lock(void) {
    if (global_sem != NULL) {
        sem_wait(global_sem);
    }
}

void ipc_unlock(void) {
    if (global_sem != NULL) {
        sem_post(global_sem);
    }
}

void cleanup_shared_memory(SharedData* shm_ptr, int creator) {
    if (shm_ptr != NULL && shm_ptr != MAP_FAILED) {
        munmap(shm_ptr, sizeof(SharedData));
    }

    if (shm_fd != -1) {
        close(shm_fd);
        shm_fd = -1;
    }

    if (global_sem != NULL && global_sem != SEM_FAILED) {
        sem_close(global_sem);
        global_sem = NULL;
    }

    // Only the process designated as creator unlinks system-wide objects
    if (creator) {
        shm_unlink(SHM_NAME);
        sem_unlink(SEM_NAME);
    }
}
