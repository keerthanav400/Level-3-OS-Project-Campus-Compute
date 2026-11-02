#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scheduler.h"

static int compare_arrival(const void* a, const void* b) {
    const Job* jobA=(const Job*)a;
    const Job* jobB=(const Job*)b;
    if (jobA->arrival_time!=jobB->arrival_time) {
        return jobA->arrival_time-jobB->arrival_time;
    }
    return strcmp(jobA->job_id, jobB->job_id);
}

void calculate_metrics(Job jobs[], int n, SchedulingMetrics* metrics) {
    int total_tat=0;
    int total_wt=0;
    int max_completion=0;

    for (int i=0;i<n;i++) {
        jobs[i].turnaround_time=jobs[i].completion_time-jobs[i].arrival_time;
        jobs[i].waiting_time=jobs[i].turnaround_time-jobs[i].cpu_burst;
      
        if (jobs[i].waiting_time < 0) 
          jobs[i].waiting_time = 0; 

        total_tat+=jobs[i].turnaround_time;
        total_wt+=jobs[i].waiting_time;

        if (jobs[i].completion_time > max_completion)
            max_completion = jobs[i].completion_time;
    }

    metrics->avg_turnaround_time=(n > 0) ? ((double)total_tat / n): 0.0;
    metrics->avg_waiting_time=(n > 0)?((double)total_wt / n): 0.0;
    metrics->total_completion_time=max_completion;
}

void print_execution_summary(const Job jobs[], int n, const char* policy_name) {
    printf("\n==================================================\n");
    printf("         SCHEDULER REPORT: %s\n", policy_name);
    printf("==================================================\n");
    printf("%-10s | %-8s | %-7s | %-8s | %-11s | %-8s\n", 
           "Job ID", "Arrival", "Burst", "Priority", "Completion", "Waiting");
    printf("--------------------------------------------------\n");

    for (int i=0;i<n;i++) {
        printf("%-10s | %-8d | %-7d | %-8d | %-11d | %-8d\n", jobs[i].job_id, jobs[i].arrival_time,
               jobs[i].cpu_burst, jobs[i].priority, jobs[i].completion_time, jobs[i].waiting_time);
    }
    printf("==================================================\n");
}

void run_fcfs(Job jobs[], int n, SchedulingMetrics* metrics) {
    qsort(jobs, n, sizeof(Job), compare_arrival);

    int current_time=0;
    for (int i = 0; i < n; i++) {
        if (current_time < jobs[i].arrival_time) 
            current_time = jobs[i].arrival_time; 
      
        current_time += jobs[i].cpu_burst;
        jobs[i].completion_time=current_time;
        jobs[i].status=JOB_COMPLETED;
    }

    calculate_metrics(jobs,n,metrics);
    print_execution_summary(jobs,n,"FCFS (First-Come, First-Served)");
    printf("Averages->Waiting Time: %.2f | Turnaround Time: %.2f\n\n",metrics->avg_waiting_time, metrics->avg_turnaround_time);
}

void run_round_robin(Job jobs[], int n, int time_quantum, SchedulingMetrics* metrics) {
    if (time_quantum <= 0) {
        fprintf(stderr, "[Error] Time quantum must be greater than 0 for Round Robin.\n");
        return;
    }

    int* remaining_burst=(int*)malloc(n * sizeof(int));
    for (int i=0;i<n; i++) {
        remaining_burst[i] = jobs[i].cpu_burst;
        jobs[i].status = JOB_READY;
    }

    int current_time=0;
    int completed_count=0;

    while (completed_count < n) {
        int progress_made = 0;

        for (int i = 0; i < n; i++) {
            if (jobs[i].arrival_time <= current_time && remaining_burst[i] > 0) {
                progress_made = 1;
                if (remaining_burst[i] > time_quantum) {
                    current_time += time_quantum;
                    remaining_burst[i] -= time_quantum;
                } else {
                    current_time += remaining_burst[i];
                    remaining_burst[i] = 0;
                    jobs[i].completion_time = current_time;
                    jobs[i].status = JOB_COMPLETED;
                    completed_count++;
                }
            }
        }

        if (!progress_made) 
          current_time++;
    }

    free(remaining_burst);
    calculate_metrics(jobs, n, metrics);
    
    char policy_buf[64];
    snprintf(policy_buf, sizeof(policy_buf), "Round Robin (Quantum = %d)", time_quantum);
    print_execution_summary(jobs, n, policy_buf);
    printf("Averages -> Waiting Time: %.2f | Turnaround Time: %.2f\n\n", 
           metrics->avg_waiting_time, metrics->avg_turnaround_time);
}

void run_priority(Job jobs[], int n, SchedulingMetrics* metrics) {
    int* is_completed = (int*)calloc(n, sizeof(int));
    int current_time = 0;
    int completed_count = 0;

    while (completed_count < n) {
        int selected_idx = -1;
        int highest_priority = 999999; // Assuming lower no.=higher priority
        int earliest_arrival = 999999;

        // Find available job with highest priority (lowest integer value)
        // Tie-breaker rule:Earliest arrival time
        for (int i = 0; i < n; i++) {
            if (!is_completed[i] && jobs[i].arrival_time <= current_time) {
                if (jobs[i].priority < highest_priority) {
                    highest_priority = jobs[i].priority;
                    earliest_arrival = jobs[i].arrival_time;
                    selected_idx = i;
                } 
                else if (jobs[i].priority == highest_priority) {
                    if (jobs[i].arrival_time < earliest_arrival) {
                        earliest_arrival = jobs[i].arrival_time;
                        selected_idx = i;
                    }
                }
            }
        }

        if (selected_idx == -1) {
            current_time++;
        } else {
            current_time += jobs[selected_idx].cpu_burst;
            jobs[selected_idx].completion_time = current_time;
            jobs[selected_idx].status = JOB_COMPLETED;
            is_completed[selected_idx] = 1;
            completed_count++;
        }
    }

    free(is_completed);
    calculate_metrics(jobs, n, metrics);
    print_execution_summary(jobs, n, "Non-preemptive Priority Scheduling (Tie-breaker: Arrival Time)");
    printf("Averages -> Waiting Time: %.2f | Turnaround Time: %.2f\n\n", 
           metrics->avg_waiting_time, metrics->avg_turnaround_time);
}
