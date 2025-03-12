#ifndef THREADLIB_H
#define THREADLIB_H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#define sleep_ms(x) Sleep(x)
#else
#define sleep_ms(x) usleep(x * 1000)
#include <unistd.h>
#endif

typedef struct worker_param_t
{
    int thread_id;
    enum status
    {
        IDLE,
        BUSY
    } status;
    pthread_mutex_t *mutex;
    void *custom_params; // User-defined parameters
} worker_param_t;

typedef struct thread
{
    pthread_t thread;
    worker_param_t worker_param;
} thread;

typedef struct thread_master_param_t
{
    int max_threads;
    pthread_mutex_t mutex;
    void *(*__thread_worker__)(void *); // task to be done by the worker
    void *custom_params;                // User-defined parameters
} thread_master_param_t;

/**
 * @brief Initializes the thread master with a specified number of threads and a worker function.
 *
 * @param max_threads The maximum possible number of threads to be used.
 * @param thread_worker The function pointer to the worker function that each thread will execute.
 * @param custom_params Custom parameters to be passed to the worker function.
 */
void thread_master_init(int max_threads, void *(*__thread_worker__)(void *), void *custom_params);

/**
 * @brief Initializes a mutex.
 *
 * @param mutex Pointer to the mutex to be initialized.
 */
void thread_mutex_init(pthread_mutex_t *mutex);

/**
 * @brief Destroys a mutex.
 *
 * @param mutex Pointer to the mutex to be destroyed.
 */
void thread_mutex_destroy(pthread_mutex_t *mutex);

/**
 * @brief Frees resources allocated by the thread master.
 */
void thread_master_free();

/**
 * @brief Retrieves the status of the thread master.
 */
void thread_master_get_status();

/**
 * @brief Assigns a new job to the thread workers.
 *
 * @param thread_worker The function pointer to the worker function that each thread will execute.
 * @param custom_params Custom parameters to be passed to the worker function.
 */
void thread_master_assign_new_job(void *(*__thread_worker__)(void *), void *custom_params);

/**
 * Example of usage:
 * 
 * void *worker_function(void *param) {
 *     worker_param_t *worker_param = (worker_param_t *)param;
 *     printf("Thread %d is working\n", worker_param->thread_id);
 *     sleep_ms(1000); // Simulate work
 *     return NULL;
 * }
 * 
 * int main() {
 *     thread_master_init(4, worker_function, NULL);
 *     // Do some work
 *     thread_master_free();
 *     return 0;
 * }
 */

#endif // !THREADLIB_H