#ifndef IPC_H
#define IPC_H

#include <stddef.h>

#define SHM_NAME "/campus_compute_shm"
#define SEM_NAME "/campus_compute_sem"
#define SHM_SIZE 1024

// Structure defining the payload shared between processes via IPC
typedef struct {
    int active_jobs_count;
    int system_load_status; // e.g., 0 for Normal, 1 for Heavy, 2 for Critical
    char latest_message[256];
} SharedData;

SharedData* init_shared_memory(int create_new);
int init_semaphore(void);
void ipc_lock(void);
void ipc_unlock(void);
void cleanup_shared_memory(SharedData* shm_ptr, int creator);

#endif 
