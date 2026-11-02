#ifndef LOGGER_H
#define LOGGER_H
#include <pthread.h>

#define MAX_LOG_MSG_LEN 256
#define QUEUE_SIZE 10

// Log levels for classifying events
typedef enum {
    LOG_INFO=0, LOG_WARN, LOG_ERROR, LOG_SUCCESS
} LogLevel;

// Bounded buffer queue structure for thread-safe logging
typedef struct {
    char messages[QUEUE_SIZE][MAX_LOG_MSG_LEN];
    int head; int tail;
    int count; int shutdown_requested; 
} LogQueue;

int init_logger(const char* log_filename);
void log_message(LogLevel level, const char* format, ...);
void* consumer_thread(void* arg);
void close_logger(void);

#endif 
