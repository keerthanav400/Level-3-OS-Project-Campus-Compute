#ifndef JOB_H
#define JOB_H

// Job status
typedef enum {
    JOB_NEW=0,
    JOB_READY,JOB_RUNNING,JOB_COMPLETED,JOB_FAILED
}JobStatus;
// Core Job structure 
typedef struct Job {
    char job_id[32];
    int arrival_time;
    int cpu_burst;
    int priority;          // Lower no.,higher priority
    JobStatus status;
    int completion_time;
    int turnaround_time;
    int waiting_time;
} Job;

int validate_job_input(const char* job_id, int arrival_time, int cpu_burst, int priority);
void init_job(Job* job, const char* job_id, int arrival_time, int cpu_burst, int priority);
const char* job_status_to_string(JobStatus status);

#endif
