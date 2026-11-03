#ifndef BANKER_H
#define BANKER_H

/* FR5: Banker's safety algorithm (Need = Maximum - Allocation).
 * banker_demo(0) = built-in safe state, banker_demo(1) = built-in unsafe state.
 * banker_interactive() reads a validated state from stdin.
 * Return 0 when the result matches the expectation. */
int banker_demo(int unsafe);
int banker_interactive(void);

#endif
