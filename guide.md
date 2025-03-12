# ThreadMaster Library Guide

## Overview

This project demonstrates a multi-threaded application using pthreads in C. It includes a thread master that manages worker threads, each executing a specified task. The project is structured to allow easy management and execution of tasks in a multi-threaded environment.

## Before Using

Make sure you include the necessary files in your `CMakelist.txt` or whatever compiler you use.

For example: 
```CMAKE
add_executable(${APP_NAME} ${SOURCE}/main.c ${CMAKE_SOURCE_DIR}/include/threadlib/threadlib.c)
```

It's important to understand that giving it, for example, 10 threads to work with, doesn't mean it will use all available threads at the same time. It will only use more threads if the job requests are faster than the workers available.

## All Available Functions

### thread_master_init

```c
void thread_master_init(int max_threads, void *(*__thread_worker__)(void *), void *custom_params);
```
Initializes the thread master with a specified number of threads and a worker function.

- `max_threads`: The maximum number of threads to be used.
- `__thread_worker__`: The function pointer to the worker function that each thread will execute.
- `custom_params`: Custom parameters to be passed to the worker function.

### thread_mutex_init

```c
void thread_mutex_init(pthread_mutex_t *mutex);
```
Initializes a mutex.

- `mutex`: Pointer to the mutex to be initialized.

### thread_mutex_destroy

```c
void thread_mutex_destroy(pthread_mutex_t *mutex);
```
Destroys a mutex.

- `mutex`: Pointer to the mutex to be destroyed.

### thread_master_free

```c
void thread_master_free();
```
Frees resources allocated by the thread master.

### thread_master_get_status

```c
void thread_master_get_status();
```
Retrieves the status of the thread master.

### thread_master_assign_new_job

```c
void thread_master_assign_new_job(void *(*__thread_worker__)(void *), void *custom_params);
```
Assigns a new job to the thread workers.

- `__thread_worker__`: The function pointer to the worker function that each thread will execute.
- `custom_params`: Custom parameters to be passed to the worker function.

## File Structure

- `src/main.c`: Contains the main function and the thread worker example of use.
- `include/threadlib/threadlib.h`: Header file for the thread library.
- `include/threadlib/threadlib.c`: Implementation of the thread library.

### src/main.c

This file contains the main function and the thread worker implementation. The main function initializes the thread master with a specified number of threads and assigns tasks to these threads.

```c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "threadlib/threadlib.h"

#define MAX_THREAD 2

typedef struct custom_params_t
{
    int a;
} custom_params_t;

void *thread_worker(void *params)
{
    worker_param_t *param = (worker_param_t *)params;
    // custom_params_t *custom = (custom_params_t *)param->custom_params;

    if (param->thread_id == 3)
    {
        param->status = IDLE;
        return NULL;
    }

    printf("Thread %d - WE ARE RUNNING THIS TASK\n", param->thread_id);

    param->status = IDLE;
    return NULL;
}

int main(void)
{
    custom_params_t *custom_params = (custom_params_t *)malloc(sizeof(custom_params_t));
    custom_params->a = 0;
    thread_master_init(MAX_THREAD, thread_worker, NULL);

    thread_master_free();
    printf("Custom Params: %d\n", custom_params->a);
    return 0;
}
```

### include/threadlib/threadlib.h

This header file declares the functions and structures used in the thread library. It provides the necessary declarations for initializing and managing threads, as well as handling mutexes.

```c
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
```

### include/threadlib/threadlib.c

This file contains the implementation of the thread library functions. It includes the logic for initializing the thread master, creating and managing threads, and handling mutexes.

```c
#include "threadlib/threadlib.h"

int __max_threads;
unsigned char isThreadMasterRunning = 0;
thread *listT = NULL;
thread_master_param_t *param = NULL;
pthread_t thread_master;

static pthread_mutex_t mutex;

void __thread_mutex_init__(pthread_mutex_t *mutex)
{
    if (pthread_mutex_init(mutex, NULL) != 0)
    {
        fprintf(stderr, "Error: Failed to initialize mutex.\n");
        exit(EXIT_FAILURE);
    }
}

void thread_mutex_init(pthread_mutex_t *mutex)
{
    __thread_mutex_init__(mutex);
}

void __thread_mutex_destroy__(pthread_mutex_t *mutex)
{
    if (pthread_mutex_destroy(mutex) != 0)
    {
        fprintf(stderr, "Error: Failed to destroy mutex.\n");
        exit(EXIT_FAILURE);
    }
}

void thread_mutex_destroy(pthread_mutex_t *mutex)
{
    __thread_mutex_destroy__(mutex);
}

void *__thread_master_init__(void *params)
{
    thread_master_param_t *param = (thread_master_param_t *)params;

    listT = (thread *)malloc(param->max_threads * sizeof(thread));
    if (listT == NULL)
    {
        fprintf(stderr, "Tread Master - Error: Failed to allocate memory for thread pool.\n");
        return NULL;
    }

    thread_mutex_init(&mutex);

    __max_threads = param->max_threads;

    for (int i = 0; i < param->max_threads; i++)
    {
        listT[i].worker_param.thread_id = i + 1;
        listT[i].worker_param.mutex = &mutex;
        listT[i].worker_param.status = IDLE;
        listT[i].worker_param.custom_params = param->custom_params;
    }

    printf("Thread Master - Success: Ready.\n");

    thread_master_assign_new_job(param->__thread_worker__, param->custom_params);

    return NULL;
}

void thread_master_init(int max_threads, void *(*__thread_worker__)(void *), void *custom_params)
{
    param = (thread_master_param_t *)malloc(sizeof(thread_master_param_t));

    param->max_threads = max_threads;
    param->__thread_worker__ = __thread_worker__;
    param->mutex = mutex;
    param->custom_params = custom_params;

    static pthread_mutex_t mutex;
    __thread_mutex_init__(&mutex);

    param->mutex = mutex;

    if (pthread_create(&thread_master, NULL, __thread_master_init__, (void *)param) != 0)
    {
        fprintf(stderr, "Error: Failed to create thread master.\n");
        exit(EXIT_FAILURE);
    }
    isThreadMasterRunning = 1;

    pthread_detach(thread_master);
    printf("Thread Master - Created\n");
}

void thread_master_assign_new_job(void *(*__thread_worker__)(void *), void *custom_params)
{
    for (int i = 0; i < __max_threads; i++)
    {
        if (listT[i].worker_param.status == IDLE)
        {
            listT[i].worker_param.status = BUSY;
            listT[i].worker_param.custom_params = custom_params;
            pthread_create(&(listT[i].thread), NULL, __thread_worker__, &(listT[i].worker_param));
            break;
        }

        if (i == __max_threads - 1)
        {
            sleep_ms(10);
            i = 0;
        }
    }
}

void thread_master_get_status()
{
    printf("\n\nSTATUS\n");
    do
    {
#ifdef DEBUG
        printf("Thread Master - Waiting for status\n");
        printf("Thread Master - Max Threads: %d\n", __max_threads);
        printf("Param: %d\n", param->max_threads);
#endif // DEBUG
        sleep(1);
    } while (__max_threads == 0);

    pthread_mutex_lock(&param->mutex);
    for (int i = 0; i < __max_threads; i++)
    {
        switch (listT[i].worker_param.status)
        {
        case IDLE:
            printf("Thread %d - IDLE\n", listT[i].worker_param.thread_id);
            break;
        case BUSY:
            printf("Thread %d - BUSY\n", listT[i].worker_param.thread_id);
            isThreadMasterRunning = 1;
            break;
        default:
            printf("Thread %d - UNKNOWN\n", listT[i].worker_param.thread_id);
            break;
        }
    }
    pthread_mutex_unlock(&param->mutex);
    printf("END STATUS\n");
}

void thread_master_free()
{
    while (isThreadMasterRunning)
    {
#ifdef DEBUG
        printf("Thread Master - Waiting for workers to finish\n");
#endif // DEBUG

        for (int i = 0; i < __max_threads; i++)
        {
            if (listT[i].worker_param.status == BUSY)
            {
                break;
            }
            if (i == __max_threads - 1)
            {
                isThreadMasterRunning = 0;
            }
        }
        sleep_ms(100);
    }

    __thread_mutex_destroy__(&param->mutex);
    free(param);
    free(listT);
    printf("Thread Master - Free and Stopped\n");
}
```

### Usage Example

This example demonstrates how to use the thread library to create and manage threads.

```c
void *worker_function(void *param) {
    worker_param_t *worker_param = (worker_param_t *)param;
    printf("Thread %d is working\n", worker_param->thread_id);
    sleep_ms(1000); // Simulate work
    
    // Be sure to include this, otherwise the master won't know which threads are available
    param->status = IDLE;
    return NULL;
}

int main() {
    thread_master_init(4, worker_function, NULL);
    // Do some work
    thread_master_free();
    return 0;
}
```

Of course, you can create your custom parameters and use them as you please.

```c
/* Don't forget the includes */

typedef struct custom_params_t
{
    int my_custom_variable;
} custom_params_t;

void *worker_function(void *param) {
    worker_param_t *worker_param = (worker_param_t *)param;
    custom_params_t *my_custom_params = (worker_param_t *)param->custom_params;

    printf("Thread %d is working\n", worker_param->thread_id);
    sleep_ms(1000); // Simulate work

    printf("%d\n", my_custom_params->my_custom_variable); // stdout: 42

    // Be sure to include this, otherwise the master won't know which threads are available
    param->status = IDLE;
    return NULL;
}

int main() {
    custom_params_t my_params = (custom_params_t *)malloc(sizeof(custom_params_t)); // It's recommended to use the heap
    my_params->my_custom_variable = 42;

    thread_master_init(4, worker_function, my_params);
    
    // Do some work

    printf("%d\n", my_params->my_custom_variable); 
    // Of course, the value may not have changed by here
    // it depends on how fast the thread work is

    thread_master_free();
    free(my_params); // Dont forget to free your custom parameters
    return 0;
}
```

You can assign new tasks to the other threads if one is busy.

```c
void *worker_function(void *param) {
    worker_param_t *worker_param = (worker_param_t *)param;

    printf("Thread %d is working\n", worker_param->thread_id);
    sleep_ms(1000); // Simulate work

    // Be sure to include this, otherwise the master won't know which threads are available
    param->status = IDLE;
    return NULL;
}

void *new_task(void *param) {
    worker_param_t *worker_param = (worker_param_t *)param;

    printf("Thread %d is working on another task\n", worker_param->thread_id);
    sleep_ms(2000); // Simulate work

    param->status = IDLE;
    return NULL;
}

int main() {
    custom_params_t my_params = (custom_params_t *)malloc(sizeof(custom_params_t)); // It's recommended to use the heap
    my_params->my_custom_variable = 42;

    thread_master_init(4, worker_function, my_params);
    
    // Do some work

    thread_master_assign_new_job(new_task, NULL); // Instead of NULL you can include other custom parameters

    printf("%d\n", my_params->my_custom_variable); 
    // Of course, the value may not have changed by here
    // it depends on how fast the thread work is

    thread_master_free();
    free(my_params); // Dont forget to free your custom parameters
    return 0;
}
```

### A big thanks to:
- Professor **Marco Ferreira** - <marco.ferreira@ipleiria.pt>
- Professor **Patrício Domingues**  - <patricio.domingues@ipleiria.pt>
