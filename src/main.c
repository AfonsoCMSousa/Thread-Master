#include <stdio.h>
#include <stdlib.h>

#include "threadlib/threadlib.h"

#define MAX_THREAD 2

typedef struct custom_params_t
{
    int a;
} custom_params_t;

void *thread_worker(void *params)
{
    worker_param_t *param = (worker_param_t *)params;
    custom_params_t *custom = (custom_params_t *)param->custom_params;

    if (param->thread_id == 3)
    {
        param->status = IDLE;
        return NULL;
    }

    sleep_ms(100);

    custom->a += 10;

    printf("Thread %d - WE ARE RUNNING THIS TASK\n", param->thread_id);

    param->status = IDLE;
    return NULL;
}

int main(void)
{
    custom_params_t *custom_params = (custom_params_t *)malloc(sizeof(custom_params_t));
    custom_params->a = 0;
    thread_master_init(MAX_THREAD, NULL, custom_params);

    sleep_ms(1000);

    thread_master_assign_new_job(thread_worker, custom_params);
    thread_master_assign_new_job(thread_worker, custom_params);
    thread_master_assign_new_job(thread_worker, custom_params);

    thread_master_free();
    printf("Custom Params: %d\n", custom_params->a);

    free(custom_params);
    return 0;
}