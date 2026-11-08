#ifndef WORKER_H
#define WORKER_H

/* FR6: fork() workers collected with waitpid().
 * mode 0 = all normal, 1 = one SIMULATED failure (exit code 42),
 * mode 2 = one SIMULATED failure (killed by signal). Returns 0 if the parent's report is accurate. */
int worker_demo(int mode);
int worker_demo_normal(void);   /* mode 0 */
int worker_demo_failure(void);  /* mode 1 */

#endif
