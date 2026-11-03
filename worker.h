#ifndef WORKER_H
#define WORKER_H

/* FR6: fork() workers collected with waitpid().
 * mode 0 = all normal, 1 = one SIMULATED failure (exit code 42),
 * mode 2 = one SIMULATED failure (killed by signal). Returns 0 if the parent's report is accurate. */
int worker_demo(int mode);

#endif
