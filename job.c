#include <stdio.h>
#include <string.h>
#include "job.h"

int validate_job_input(const char* job_id, int arrival_time, int cpu_burst, int priority)  {
    if (job_id==NULL || strlen(job_id)==0 || strlen(job_id)>=32) 
    {
        fprintf(stderr, "[Error] Job ID is empty, too long, or invalid.\n");
        return 0;
    }
    if (arrival_time<0) {
        fprintf(stderr, "[Error] Arrival time for job %s cannot be negative (%d).\n", job_id, arrival_time);
        return 0;
    }
    if (cpu_burst<=0) {
        fprintf(stderr, "[Error] CPU burst for job %s must be greater than 0 (got %d).\n", job_id, cpu_burst);
        return 0;
    }
    if (priority < 0) {
        fprintf(stderr, "[Error] Priority for job %s cannot be negative (%d).\n", job_id, priority);
        return 0;
    }
    return 1;
}

void init_job(Job* job, const char* job_id, int arrival_time, int cpu_burst, int priority) {
    if (job==NULL) 
      return;

    strncpy(job->job_id,job_id,31);
    job->job_id[31]='\0';
    job->arrival_time=arrival_time;
    job->cpu_burst=cpu_burst;
    job->priority=priority;
    job->status=JOB_NEW;
    job->completion_time=0;
    job->turnaround_time=0;
    job->waiting_time=0;
}

const char* job_status_to_string(JobStatus status) {
    switch (status) {
        case JOB_NEW:       
          return "NEW";
        case JOB_READY:     
          return "READY";
        case JOB_RUNNING:   
          return "RUNNING";
        case JOB_COMPLETED: 
          return "COMPLETED";
        case JOB_FAILED:    
          return "FAILED";
        default:            
          return "UNKNOWN";
    }
}
