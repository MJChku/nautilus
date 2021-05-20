/*
 * pthread_cond_init.c
 */

/* #include <stdlib.h> */
#include <nautilus/nautilus.h>
#include "pthread.h"
#include "implement.h"
#include "pthread_cond_common.h"


static uint64_t count = 0;
int
pthread_cond_init (pthread_cond_t * c, const pthread_condattr_t * attr)
{

    NK_PROFILE_ENTRY();
    int result = 0;

    if ((attr != NULL && *attr != NULL) &&
      ((*attr)->pshared == PTHREAD_PROCESS_SHARED))
    {
      /*
       * Creating condition variable that can be shared between
       * processes.
       */
      result = ENOSYS;
    }

    char buf[NK_WAIT_QUEUE_NAME_LEN];

    memset(c, 0, sizeof(pthread_cond_t));

    snprintf(buf,NK_WAIT_QUEUE_NAME_LEN,"condvar%lu-wait",__sync_fetch_and_add(&count,1));
    //c->sem = malloc(sizeof(simple_sem_t)); 
    //ssem_init(c->sem, 0);
    c->wait_queue = nk_wait_queue_create(buf);

    result = NK_MUTEX_LOCK_INIT(&c->lock);
   

    DEBUG("Condvar init %p mutex %p success? %d \n", c , &c->lock, result);
    NK_PROFILE_EXIT();
    return result;

}				/* pthread_cond_init */
