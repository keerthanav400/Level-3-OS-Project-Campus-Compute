# Campus-Compute:Intelligent Job Scheduling and Resource Management
This project simulates a shared university computing lab where jobs compete for CPU time, require synchronized activity logging, inter-process communication (IPC), deadlock avoidance, and worker failure detection.

## Module & File Breakdown

### Team Member 1 Modules (FR1 - FR4)
* **Job Management (`job.h`, `job.c`)**
  * Implements the core `Job` structure (tracking ID, arrival time, CPU burst, priority, and status) and provides strict input validation to catch malformed data, using `validate_job_input()`.
* **CPU Scheduling (`scheduler.h`, `scheduler.c`)**
  * Implements FCFS Round Robin with a configurable time quantum, and Non-Preemptive Priority Scheduling (using `run_fcfs`,`run_round_robin`,`run_priority`) with arrival-time tie-breaking, alongside performance metric computations.
* **Concurrent Activity Logger (`logger.h`, `logger.c`)**
  * Utilizes POSIX threads (pthread) to implement a circular message queue, protected by a mutex lock and coordinated via condition variables for thread-safe asynchronous event logging.
* **Inter-Process Communication (`ipc.h`, `ipc.c`)**
  * Employs POSIX shared memory to exchange live state payloads between cooperating processes, synchronized using POSIX named semaphores with robust error handling and cleanup routines.

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
