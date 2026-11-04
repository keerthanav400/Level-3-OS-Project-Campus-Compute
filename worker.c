/* worker.c - FR6: fork() workers, collect status with waitpid().
 * mode 0: three normal workers
 * mode 1: two normal + one SIMULATED failure (exit code 42)
 * mode 2: two normal + one SIMULATED failure (killed by raise(SIGTERM))
 * The failures are deliberately triggered by the program; they are NOT real OS faults. */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include "worker.h"

#define NW 3
#define SIM_EXIT_CODE 42

static void worker_body(int id, int fail_mode) {
    printf("  [worker %d pid=%d] started\n", id, getpid());
    fflush(stdout);
    volatile long x = 0;
    for (long i = 0; i < 20000000L * (id + 1) / 2; i++) x += i;   /* small CPU job */
    if (fail_mode == 1) {
        printf("  [worker %d pid=%d] SIMULATED FAILURE: exiting with code %d\n", id, getpid(), SIM_EXIT_CODE);
        fflush(stdout);
        _exit(SIM_EXIT_CODE);
    }
    if (fail_mode == 2) {
        printf("  [worker %d pid=%d] SIMULATED FAILURE: sending itself SIGTERM\n", id, getpid());
        fflush(stdout);
        raise(SIGTERM);
        _exit(99);   /* not reached */
    }
    printf("  [worker %d pid=%d] finished work normally\n", id, getpid());
    fflush(stdout);
    _exit(0);
}

int worker_demo_normal(int mode) {
    pid_t pids[NW];
    int expected_fail = (mode != 0);
    printf("=== Worker processes (%s) ===\n",
           mode == 0 ? "normal completion" :
           mode == 1 ? "simulated failure: non-zero exit code" : "simulated failure: terminated by signal");
    fflush(stdout);

    for (int i = 0; i < NW; i++) {
        int fm = (i == NW - 1) ? mode : 0;   /* only the last worker fails */
        pids[i] = fork();
        if (pids[i] < 0) { perror("fork"); return 1; }
        if (pids[i] == 0) worker_body(i + 1, fm);
        printf("[parent pid=%d] created worker %d -> child pid %d\n", getpid(), i + 1, pids[i]);
        fflush(stdout);
    }

    int normal = 0, failed = 0;
    for (int i = 0; i < NW; i++) {
        int st;
        pid_t p = waitpid(pids[i], &st, 0);
        if (p < 0) { perror("waitpid"); return 1; }
        if (WIFEXITED(st) && WEXITSTATUS(st) == 0) {
            printf("[parent] worker %d (pid %d): NORMAL completion, exit status 0\n", i + 1, p);
            normal++;
        } else if (WIFEXITED(st)) {
            printf("[parent] worker %d (pid %d): FAILURE DETECTED, exit status %d (simulated)\n", i + 1, p, WEXITSTATUS(st));
            failed++;
        } else if (WIFSIGNALED(st)) {
            printf("[parent] worker %d (pid %d): FAILURE DETECTED, killed by signal %d (%s) (simulated)\n",
                   i + 1, p, WTERMSIG(st), strsignal(WTERMSIG(st)));
            failed++;
        }
    }
    printf("[parent] Summary: %d normal, %d failed. All children reaped with waitpid().\n", normal, failed);
    int ok = expected_fail ? (failed == 1 && normal == NW - 1) : (failed == 0 && normal == NW);
    printf("Result: %s\n", ok ? "PASS (parent report matches what was triggered)" : "FAIL");
    return ok ? 0 : 1;
}
