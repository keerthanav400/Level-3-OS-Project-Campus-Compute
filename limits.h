#ifndef LIMITS_H
#define LIMITS_H

/* FR7: setrlimit() demonstrations (run in child processes) and cleanup verification. */
int limits_demo(void);
/* Checks that program-created IPC objects are gone and no child processes remain. */
int cleanup_verify(void);

#endif
