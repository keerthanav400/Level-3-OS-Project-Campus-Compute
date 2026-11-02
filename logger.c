#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include "logger.h"

// Global logger control variables
static LogQueue log_queue;
static pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond_non_empty = PTHREAD_COND_INITIALIZER;
static pthread_cond_t cond_non_full = PTHREAD_COND_INITIALIZER;
static pthread_t logger_thread_id;
static FILE* log_file_ptr = NULL;
static int logger_initialized = 0;

static const char* level_to_string(LogLevel level) {
    switch (level) {
        case LOG_INFO:    return "INFO";
        case LOG_WARN:    return "WARN";
        case LOG_ERROR:   return "ERROR";
        case LOG_SUCCESS: return "SUCCESS";
        default:          return "UNKNOWN";
    }
}

int init_logger(const char* log_filename) {
    if (logger_initialized) return 1;

    // Open file for writing logs
    log_file_ptr = fopen(log_filename, "w");
    if (log_file_ptr == NULL) {
        perror("[Error] Failed to open log file");
        return 0;
    }

    // Initialize queue state
    log_queue.head = 0;
    log_queue.tail = 0;
    log_queue.count = 0;
    log_queue.shutdown_requested = 0;

    // Spawn the background consumer worker thread
    if (pthread_create(&logger_thread_id, NULL, consumer_thread, NULL) != 0) {
        perror("[Error] Failed to create logger consumer thread");
        fclose(log_file_ptr);
        return 0;
    }

    logger_initialized = 1;
    return 1;
}

void log_message(LogLevel level, const char* format, ...) {
    if (!logger_initialized) return;

    // Construct formatted message string with timestamp
    char formatted_msg[MAX_LOG_MSG_LEN];
    char time_buffer[64];
    
    time_t raw_time = time(NULL);
    struct tm* time_info = localtime(&raw_time);
    strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S", time_info);

    va_list args;
    va_start(args, format);
    char user_msg[192];
    vsnprintf(user_msg, sizeof(user_msg), format, args);
    va_end(args);

    snprintf(formatted_msg, sizeof(formatted_msg), "[%s] [%s] %s", 
             time_buffer, level_to_string(level), user_msg);

    // Producer logic: Acquire mutex lock and push to bounded circular queue
    pthread_mutex_lock(&queue_mutex);

    // Wait if the bounded queue is completely full
    while (log_queue.count >= QUEUE_SIZE && !log_queue.shutdown_requested) {
        pthread_cond_wait(&cond_non_full, &queue_mutex);
    }

    if (log_queue.shutdown_requested) {
        pthread_mutex_unlock(&queue_mutex);
        return;
    }

    // Insert message into queue tail
    strncpy(log_queue.messages[log_queue.tail], formatted_msg, MAX_LOG_MSG_LEN - 1);
    log_queue.messages[log_queue.tail][MAX_LOG_MSG_LEN - 1] = '\0';
    log_queue.tail = (log_queue.tail + 1) % QUEUE_SIZE;
    log_queue.count++;

    // Signal consumer thread that new data is available
    pthread_cond_signal(&cond_non_empty);
    pthread_mutex_unlock(&queue_mutex);
}

void* consumer_thread(void* arg) {
    (void)arg;

    while (1) {
        pthread_mutex_lock(&queue_mutex);

        // Wait while queue is empty, unless shutdown has been requested
        while (log_queue.count == 0 && !log_queue.shutdown_requested) {
            pthread_cond_wait(&cond_non_empty, &queue_mutex);
        }

        // If shutdown is flagged and queue is completely drained, exit loop
        if (log_queue.count == 0 && log_queue.shutdown_requested) {
            pthread_mutex_unlock(&queue_mutex);
            break;
        }

        // Pop message from queue head
        char current_msg[MAX_LOG_MSG_LEN];
        strncpy(current_msg, log_queue.messages[log_queue.head], MAX_LOG_MSG_LEN);
        log_queue.head = (log_queue.head + 1) % QUEUE_SIZE;
        log_queue.count--;

        // Signal producer that space has opened up in queue
        pthread_cond_signal(&cond_non_full);
        pthread_mutex_unlock(&queue_mutex);

        // Write message out to file and stdout safely
        if (log_file_ptr != NULL) {
            fprintf(log_file_ptr, "%s\n", current_msg);
            fflush(log_file_ptr); // Immediate persistent write
        }
    }

    return NULL;
}

void close_logger(void) {
    if (!logger_initialized) return;

    // Set shutdown flag and wake up consumer thread if it's sleeping
    pthread_mutex_lock(&queue_mutex);
    log_queue.shutdown_requested = 1;
    pthread_cond_broadcast(&cond_non_empty);
    pthread_cond_broadcast(&cond_non_full);
    pthread_mutex_unlock(&queue_mutex);

    // Wait for consumer background thread to finish cleanly
    pthread_join(logger_thread_id, NULL);

    // Close log file output stream
    if (log_file_ptr != NULL) {
        fclose(log_file_ptr);
        log_file_ptr = NULL;
    }

    // Clean up mutex and condition variables
    pthread_mutex_destroy(&queue_mutex);
    pthread_cond_destroy(&cond_non_empty);
    pthread_cond_destroy(&cond_non_full);

    logger_initialized = 0;
}
