#ifndef SCHEDULER_H
#define SCHEDULER_H
#include "job.h"

typedef struct {
    double avg_turnaround_time;
    double avg_waiting_time;
    int total_completion_time;
} SchedulingMetrics;

void run_fcfs(Job jobs[], int n, SchedulingMetrics* metrics);
void run_round_robin(Job jobs[], int n, int time_quantum, SchedulingMetrics* metrics);
void run_priority(Job jobs[], int n, SchedulingMetrics* metrics);
void calculate_metrics(Job jobs[], int n, SchedulingMetrics* metrics);
void print_execution_summary(const Job jobs[], int n, const char* policy_name);

#endif 
