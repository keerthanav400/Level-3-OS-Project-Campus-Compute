/* limits.c - FR7: demonstrate setrlimit(). Each test runs in a forked child so
 * the limit affects only that child, never the parent or the system. */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include "limits.h"

static void bye(int c) { fflush(stdout); _exit(c); }   /* flush before exiting child */

static void show(const char *name, int res) {
    struct rlimit rl; getrlimit(res, &rl);
    printf("  %s: soft=%lu hard=%lu\n", name, (unsigned long)rl.rlim_cur, (unsigned long)rl.rlim_max);
}

static void test_nofile(void) {
    printf("[Test 1] RLIMIT_NOFILE (max open file descriptors)\n");
    show("before", RLIMIT_NOFILE);
    struct rlimit rl = {16, 16};
    if (setrlimit(RLIMIT_NOFILE, &rl) < 0) { perror("setrlimit"); bye(1); }
    show("after ", RLIMIT_NOFILE);
    int fds[64], n = 0;
    for (; n < 64; n++) {
        fds[n] = open("/dev/null", O_RDONLY);
        if (fds[n] < 0) {
            printf("  open() #%d FAILED: %s (errno=%d)\n", n + 1, strerror(errno), errno);
            break;
        }
    }
    printf("  Observed effect: only %d extra files could be opened (limit 16 includes stdin/out/err)\n", n);
    for (int i = 0; i < n; i++) close(fds[i]);
    bye(n < 64 ? 0 : 1);
}

static void test_as(void) {
    printf("[Test 2] RLIMIT_AS (max virtual memory)\n");
    show("before", RLIMIT_AS);
    struct rlimit rl = {64UL * 1024 * 1024, 64UL * 1024 * 1024};   /* 64 MB */
    if (setrlimit(RLIMIT_AS, &rl) < 0) { perror("setrlimit"); bye(1); }
    show("after ", RLIMIT_AS);
    void *small = malloc(1024 * 1024);
    void *big   = malloc(256UL * 1024 * 1024);
    printf("  malloc(1 MB)   -> %s\n", small ? "succeeded" : "FAILED");
    printf("  malloc(256 MB) -> %s\n", big ? "succeeded (unexpected)" : "FAILED (ENOMEM) as expected");
    free(small); free(big);
    bye(big == NULL ? 0 : 1);
}

static void test_cpu(void) {
    printf("[Test 3] RLIMIT_CPU (max CPU seconds): busy loop with 1 s limit\n");
    struct rlimit rl = {1, 2};
    if (setrlimit(RLIMIT_CPU, &rl) < 0) { perror("setrlimit"); bye(1); }
    show("after ", RLIMIT_CPU);
    fflush(stdout);
    volatile unsigned long x = 0;
    for (;;) x++;     /* kernel sends SIGXCPU after 1 s of CPU time */
}

static int run(void (*fn)(void), int expect_signal) {
    fflush(stdout);
    pid_t p = fork();
    if (p < 0) { perror("fork"); return 1; }
    if (p == 0) { fn(); bye(1); }
    int st; waitpid(p, &st, 0);
    if (WIFSIGNALED(st)) {
        printf("  Parent: child killed by signal %d (%s)\n", WTERMSIG(st), strsignal(WTERMSIG(st)));
        return expect_signal && WTERMSIG(st) == SIGXCPU ? 0 : 1;
    }
    printf("  Parent: child exit status %d\n", WEXITSTATUS(st));
    return (!expect_signal && WEXITSTATUS(st) == 0) ? 0 : 1;
}

int limits_demo(void) {
    printf("=== Resource limits with setrlimit() (applied in child processes only) ===\n");
    int bad = 0;
    bad += run(test_nofile, 0);
    bad += run(test_as, 0);
    bad += run(test_cpu, 1);
    printf("Result: %s\n", bad == 0 ? "PASS" : "FAIL");
    return bad ? 1 : 0;
}

/* ---- cleanup verification (S20) ---- */
int cleanup_verify(void) {
    int bad = 0;
    const char *paths[] = {"/dev/shm/campuscompute_shm",
                           "/dev/shm/sem.campuscompute_req",
                           "/dev/shm/sem.campuscompute_resp"};
    printf("=== Cleanup verification ===\n");
    for (int i = 0; i < 3; i++) {
        if (access(paths[i], F_OK) == 0) { printf("  LEFTOVER: %s\n", paths[i]); bad++; }
        else printf("  not present (released): %s\n", paths[i]);
    }
    errno = 0;
    pid_t p = waitpid(-1, NULL, WNOHANG);
    if (p == -1 && errno == ECHILD) printf("  No child processes remain (no zombies, none running)\n");
    else { printf("  Unreaped child exists (pid %d)\n", p); bad++; }
    printf("Result: %s\n", bad ? "FAIL" : "PASS - all program-created resources released");
    return bad ? 1 : 0;
}

