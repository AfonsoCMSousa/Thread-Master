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
        fprintf(stderr, "Thread Master - Error: Failed to initialize mutex.\n");
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
        fprintf(stderr, "Thread Master - Error: Failed to destroy mutex.\n");
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
        fprintf(stderr, "Thread Master - Error: Failed to allocate memory for thread pool.\n");
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
        fprintf(stderr, "Thread Master - Error: Failed to create thread master.\n");
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
            // check if all threads are IDLE
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
