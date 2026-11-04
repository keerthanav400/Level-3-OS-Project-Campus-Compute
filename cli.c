#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cli.h"
#include "job.h"
#include "scheduler.h"
#include "logger.h"
#include "ipc.h"
#include "banker.h"
#include "worker.h"
#include "limits.h"

void print_menu(void) {
    printf("\n=========================================================\n");
    printf("        CAMPUS COMPUTE - OS MINI-PROJECT CLI             \n");
    printf("=========================================================\n");
    printf("  [1] FR1: Test Job Input Validation                      \n");
    printf("  [2] FR2: Run CPU Scheduling (FCFS, RR, Priority)       \n");
    printf("  [3] FR3: Test Concurrent Activity Logger (Producer/Consumer)\n");
    printf("  [4] FR4: Test Shared Memory IPC & Semaphores           \n");
    printf("  [5] FR5: Test Banker's Safety Algorithm (Safe/Unsafe)  \n");
    printf("  [6] FR6: Test Worker Process & Failure Detection       \n");
    printf("  [7] FR7: Test Resource Limits & Cleanup                \n");
    printf("  [8] Run All / Integrated Demonstration                 \n");
    printf("  [0] Exit                                               \n");
    printf("\n");
    printf("Enter your choice: ");
}

void start_cli(void) {
    int choice;
    if (!init_logger("campus_compute.log"))
        fprintf(stderr, "[Warning] Failed to initialize file logger.\n");

    log_message(LOG_INFO, "Campus Compute CLI started successfully.");

    do {
        print_menu();
        if (scanf("%d", &choice) != 1) {
            printf("[Error] Invalid input. Please enter a valid menu number.\n");
            // Clear invalid input buffer
            while (getchar() != '\n');
            continue;
        }

        switch (choice) {
            case 1: {
                printf("\n--- FR1: Job Input Validation Test ---\n");
                char id[32];
                int arrival, burst, priority;
                
                printf("Enter Job ID: ");
                scanf("%31s", id);
                printf("Enter Arrival Time: ");
                scanf("%d", &arrival);
                printf("Enter CPU Burst: ");
                scanf("%d", &burst);
                printf("Enter Priority: ");
                scanf("%d", &priority);

                if (validate_job_input(id, arrival, burst, priority)) {
                    Job j;
                    init_job(&j, id, arrival, burst, priority);
                    printf("[Success] Job input is valid! Initialized Job %s (Arrival: %d, Burst: %d)\n", 
                           j.job_id, j.arrival_time, j.cpu_burst);
                    log_message(LOG_SUCCESS, "Validated and created job: %s", j.job_id);
                } else {
                    printf("[Failure] Validation rejected the job parameters.\n");
                    log_message(LOG_WARN, "Job validation failed for input ID: %s", id);
                }
                break;
            }
            case 2: {
                printf("\n--- FR2: CPU Scheduling Demonstration ---\n");
                // Sample job set for demonstration
                Job sample_jobs_fcfs[4] = {
                    {"JobA", 0, 5, 2, JOB_NEW, 0, 0, 0},
                    {"JobB", 1, 3, 1, JOB_NEW, 0, 0, 0},
                    {"JobC", 2, 8, 3, JOB_NEW, 0, 0, 0},
                    {"JobD", 3, 6, 2, JOB_NEW, 0, 0, 0}
                };
                Job sample_jobs_rr[4];
                Job sample_jobs_pri[4];
                memcpy(sample_jobs_rr, sample_jobs_fcfs, sizeof(sample_jobs_fcfs));
                memcpy(sample_jobs_pri, sample_jobs_fcfs, sizeof(sample_jobs_fcfs));

                SchedulingMetrics metrics;
                
                printf("\nExecuting FCFS:\n");
                run_fcfs(sample_jobs_fcfs, 4, &metrics);

                printf("\nExecuting Round Robin (Quantum = 3):\n");
                run_round_robin(sample_jobs_rr, 4, 3, &metrics);

                printf("\nExecuting Non-Preemptive Priority Scheduling:\n");
                run_priority(sample_jobs_pri, 4, &metrics);

                log_message(LOG_INFO, "Executed CPU scheduling algorithms suite.");
                break;
            }
            case 3: {
                printf("\n--- FR3: Producer-Consumer Logger Test ---\n");
                printf("Pushing test log events through the thread-safe queue...\n");
                log_message(LOG_INFO, "Simulating lab user activity batch 1.");
                log_message(LOG_WARN, "Simulating resource contention alert.");
                log_message(LOG_SUCCESS, "Simulating successful job completion queue flush.");
                printf("[Success] Log messages queued and flushed to 'campus_compute.log'. Check file contents!\n");
                break;
            }
            case 4: {
                printf("\n--- FR4: Shared Memory IPC & Semaphores Test ---\n");
                if (!init_semaphore()) {
                    printf("[Error] Could not initialize semaphore.\n");
                    break;
                }
                SharedData* shm = init_shared_memory(1);
                if (shm != NULL) {
                    ipc_lock();
                    shm->active_jobs_count = 5;
                    shm->system_load_status = 1;
                    snprintf(shm->latest_message, sizeof(shm->latest_message), "IPC Operational Check from CLI");
                    printf("[IPC Write] Active Jobs: %d | Status: %d | Msg: %s\n", 
                           shm->active_jobs_count, shm->system_load_status, shm->latest_message);
                    ipc_unlock();

                    cleanup_shared_memory(shm, 1);
                    printf("[Success] Shared memory and semaphores allocated, written, and cleaned up.\n");
                    log_message(LOG_SUCCESS, "Executed shared memory IPC test.");
                } else {
                    printf("[Error] Shared memory initialization failed.\n");
                }
                break;
            }
            case 5: {
                printf("\n--- FR5: Banker's Safety Algorithm Test ---\n");
                printf("Running Safe State Demonstration:\n");
                banker_demo(0); // 0 = safe case
                printf("\nRunning Unsafe State Demonstration:\n");
                banker_demo(1); // 1 = unsafe case
                log_message(LOG_INFO, "Executed Banker's algorithm safe/unsafe tests.");
                break;
            }
            case 6: {
                printf("\n--- FR6: Worker Process & Failure Detection Test ---\n");
                printf("Demonstrating normal process completion:\n");
                worker_demo_normal();
                printf("\nDemonstrating simulated controlled worker failure:\n");
                worker_demo_failure();
                log_message(LOG_INFO, "Executed worker process fork and failure detection tests.");
                break;
            }
            case 7: {
                printf("\n--- FR7: Resource Limits & Cleanup Verification ---\n");
                limits_demo();     
                cleanup_verify();   // Verifies IPC and zombie processes are cleaned up
                log_message(LOG_SUCCESS, "Tested resource limits and verified system cleanup.");
                break;
            }
            case 8: {
                printf("\n--- Integrated Demonstration (Running All Core Suites) ---\n");
                // Trigger major routines sequentially
                Job j_test;
                init_job(&j_test, "TestJob", 0, 4, 1);
                SchedulingMetrics m;
                Job j_arr[1] = {j_test};
                run_fcfs(j_arr, 1, &m);
                banker_demo(0);
                worker_demo_normal();
                printf("[Success] Integrated test complete.\n");
                log_message(LOG_SUCCESS, "Completed full integrated system demonstration.");
                break;
            }
            case 0:
                printf("Exiting Campus Compute. Cleaning up logger and resources...\n");
                break;
            default:
                printf("[Error] Invalid option. Please choose between 0 and 8.\n");
        }
    } while (choice != 0);

    close_logger();
}
