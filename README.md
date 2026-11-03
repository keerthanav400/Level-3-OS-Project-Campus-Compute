# Campus-Compute:Intelligent Job Scheduling and Resource Management
This project simulates a shared university computing lab where jobs compete for CPU time, require synchronized activity logging, inter-process communication (IPC), deadlock avoidance, and worker failure detection.

## Module & File Breakdown

### Team Member 1 Modules (FR1 - FR4)
* **Job Management (`job.h`, `job.c`)**
  * `struct Job`: Defines core attributes including job ID, arrival time, CPU burst, priority, and status.
  * `validate_job_input()`: Validates input parameters to prevent malformed data (e.g., negative burst times or invalid priorities).
  * `init_job()`: Initializes a job instance and sets default tracking states.

* **CPU Scheduling (`scheduler.h`, `scheduler.c`)**
  * `run_fcfs()`: Implements First-Come, First-Served scheduling.
  * `run_round_robin()`: Implements Round Robin scheduling with a configurable time quantum.
  * `run_priority()`: Implements non-preemptive Priority scheduling with arrival time as a tie-breaker.
  * `calculate_metrics()`: Computes completion time, turnaround time, waiting time, and lab averages.

* **Concurrent Activity Logger (`logger.h`, `logger.c`)**
  * `init_logger()`: Initializes the logging subsystem and starts the background consumer thread.
  * `log_message()`: Thread-safe producer function that pushes events into a bounded circular queue.
  * `consumer_thread()`: Background POSIX thread routine that consumes logs and writes them to a file.
  * `close_logger()`: Gracefully flushes the queue, joins threads, and cleans up mutex/condition variables.

* **Inter-Process Communication (`ipc.h`, `ipc.c`)**
  * `init_shared_memory()`: Sets up POSIX shared memory (`shm_open`, `mmap`) for data exchange.
  * `init_semaphore()`: Initializes a POSIX named semaphore for process synchronization.
  * `ipc_lock()` / `ipc_unlock()`: Coordinates safe entry and exit into critical sections.
  * `cleanup_shared_memory()`: Unmaps memory, closes handles, and unlinks system objects.

---

### Team Member 2 Modules (FR5 - FR8)
* **Resource-Safety Analysis (`banker.h`, `banker.c`)**
  * Implements the Banker's safety algorithm using Available, Allocation, and Need matrices to verify safe states and safe sequences.
* **Worker Failure Detection (`worker.h`, `worker.c`)**
  * Manages child processes using `fork()` and monitors exit statuses with `waitpid()` to detect normal completions vs. simulated failures.
* **Resource Limits & Cleanup (`limits.h`, `limits.c`)**
  * Applies process resource limits using `setrlimit()` and coordinates cleanup procedures.
* **Command-Line Interface (`cli.h`, `cli.c`)**
  * Provides user commands/menus, input validation, and error reporting for independent testability.
* **Build Configuration (`Makefile`)**
  * Compiles all individual modules together into the final executable.

---

## Prerequisites & Build Instructions
1. **Operating System:** Linux/Ubuntu or WSL Ubuntu.
2. **Build Tools:** GCC and Make.
3. **Compilation Command:**
   ```bash
   make
