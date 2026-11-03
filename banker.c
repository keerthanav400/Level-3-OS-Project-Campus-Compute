/* banker.c - FR5: Banker's safety algorithm. Need = Maximum - Allocation. */
#include <stdio.h>
#include <string.h>
#include "banker.h"

#define MAXP 10
#define MAXR 10

typedef struct {
    int n, m;
    int avail[MAXR];
    int alloc[MAXP][MAXR];
    int max[MAXP][MAXR];
} state_t;

static void compute_need(const state_t *s, int need[MAXP][MAXR]) {
    for (int i = 0; i < s->n; i++)
        for (int j = 0; j < s->m; j++)
            need[i][j] = s->max[i][j] - s->alloc[i][j];
}

static void print_state(const state_t *s, int need[MAXP][MAXR]) {
    printf("        Allocation   Maximum      Need\n");
    for (int i = 0; i < s->n; i++) {
        printf("  P%-3d  ", i);
        for (int j = 0; j < s->m; j++) printf("%3d", s->alloc[i][j]);
        printf("   ");
        for (int j = 0; j < s->m; j++) printf("%3d", s->max[i][j]);
        printf("   ");
        for (int j = 0; j < s->m; j++) printf("%3d", need[i][j]);
        printf("\n");
    }
    printf("  Available: ");
    for (int j = 0; j < s->m; j++) printf("%3d", s->avail[j]);
    printf("\n");
}

/* Returns 1 if safe (and fills seq), 0 if unsafe. */
static int is_safe(const state_t *s, int need[MAXP][MAXR], int seq[MAXP]) {
    int work[MAXR], finish[MAXP] = {0}, count = 0;
    memcpy(work, s->avail, sizeof(work));
    while (count < s->n) {
        int found = 0;
        for (int i = 0; i < s->n; i++) {
            if (finish[i]) continue;
            int ok = 1;
            for (int j = 0; j < s->m; j++)
                if (need[i][j] > work[j]) { ok = 0; break; }
            if (ok) {
                for (int j = 0; j < s->m; j++) work[j] += s->alloc[i][j];
                printf("  P%d can finish (Need <= Work); Work becomes:", i);
                for (int j = 0; j < s->m; j++) printf(" %d", work[j]);
                printf("\n");
                finish[i] = 1; seq[count++] = i; found = 1;
            }
        }
        if (!found) {
            printf("  No remaining process has Need <= Work. Stuck processes:");
            for (int i = 0; i < s->n; i++) if (!finish[i]) printf(" P%d", i);
            printf("\n");
            return 0;
        }
    }
    return 1;
}

static int analyse(const state_t *s, int expect_safe) {
    int need[MAXP][MAXR], seq[MAXP];
    compute_need(s, need);
    for (int i = 0; i < s->n; i++)
        for (int j = 0; j < s->m; j++)
            if (need[i][j] < 0) { printf("Error: Maximum < Allocation for P%d R%d\n", i, j); return 2; }
    print_state(s, need);
    printf("Running safety algorithm:\n");
    int safe = is_safe(s, need, seq);
    if (safe) {
        printf("RESULT: SAFE state. Safe sequence: <");
        for (int i = 0; i < s->n; i++) printf("P%d%s", seq[i], i + 1 < s->n ? ", " : "");
        printf(">\n");
    } else {
        printf("RESULT: UNSAFE state. No safe sequence exists (possible deadlock).\n");
    }
    if (expect_safe >= 0) printf("Test expectation (%s): %s\n", expect_safe ? "safe" : "unsafe",
                                 safe == expect_safe ? "PASS" : "FAIL");
    return (expect_safe >= 0 && safe != expect_safe) ? 1 : 0;
}

static void base_state(state_t *s) {  /* classic 5-process, 3-resource example */
    static const int alloc[5][3] = {{0,1,0},{2,0,0},{3,0,2},{2,1,1},{0,0,2}};
    static const int max[5][3]   = {{7,5,3},{3,2,2},{9,0,2},{2,2,2},{4,3,3}};
    memset(s, 0, sizeof(*s));
    s->n = 5; s->m = 3;
    for (int i = 0; i < 5; i++)
        for (int j = 0; j < 3; j++) { s->alloc[i][j] = alloc[i][j]; s->max[i][j] = max[i][j]; }
}

int banker_demo(int unsafe) {
    state_t s; base_state(&s);
    if (!unsafe) { s.avail[0] = 3; s.avail[1] = 3; s.avail[2] = 2; }
    else         { s.avail[0] = 0; s.avail[1] = 0; s.avail[2] = 0; }
    printf("=== Banker's Algorithm: %s test case (5 processes, 3 resource types) ===\n",
           unsafe ? "UNSAFE" : "SAFE");
    return analyse(&s, unsafe ? 0 : 1);
}

int banker_interactive(void) {
    state_t s; memset(&s, 0, sizeof(s));
    printf("Processes (1-%d): ", MAXP);
    if (scanf("%d", &s.n) != 1 || s.n < 1 || s.n > MAXP) { printf("Error: invalid process count\n"); return 1; }
    printf("Resource types (1-%d): ", MAXR);
    if (scanf("%d", &s.m) != 1 || s.m < 1 || s.m > MAXR) { printf("Error: invalid resource count\n"); return 1; }
    printf("Available (%d values): ", s.m);
    for (int j = 0; j < s.m; j++)
        if (scanf("%d", &s.avail[j]) != 1 || s.avail[j] < 0) { printf("Error: bad Available\n"); return 1; }
    for (int i = 0; i < s.n; i++) {
        printf("P%d Allocation (%d values): ", i, s.m);
        for (int j = 0; j < s.m; j++)
            if (scanf("%d", &s.alloc[i][j]) != 1 || s.alloc[i][j] < 0) { printf("Error: bad Allocation\n"); return 1; }
        printf("P%d Maximum    (%d values): ", i, s.m);
        for (int j = 0; j < s.m; j++)
            if (scanf("%d", &s.max[i][j]) != 1 || s.max[i][j] < 0) { printf("Error: bad Maximum\n"); return 1; }
    }
    return analyse(&s, -1);
}
