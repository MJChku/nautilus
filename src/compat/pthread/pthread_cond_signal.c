/*
 * pthread_cond_signal.c
 *
 */
#include "pthread.h"
#include "implement.h"
#include "pthread_cond_common.h"
	
	
int
pthread_cond_signal (pthread_cond_t * c)
{
    	
    DEBUG("Condvar signaling on (%p)\n", (void*)c);
    if (c == NULL)
    {
      return EINVAL;
    }

    NK_PROFILE_ENTRY();
    NK_LOCK(&c->lock);

    // do we have anyone to signal?
    if (c->main_seq > c->wakeup_seq) {

        ++c->wakeup_seq;

        DEBUG("Condvar signaling on (%p)\n", (void*)c);
	ssem_post(c->sem,1);

    } 

    NK_UNLOCK(&c->lock);
    NK_PROFILE_EXIT();
    return 0;

}				/* pthread_cond_signal */

int
pthread_cond_broadcast (pthread_cond_t * c)
{
    if (c == NULL)
    {
      return EINVAL;
    }
 
    NK_PROFILE_ENTRY();

    NK_LOCK(&c->lock);

    // do we have anyone to wakeup?
    if (c->main_seq > c->wakeup_seq) {

        c->woken_seq = c->main_seq;
        c->wakeup_seq = c->main_seq;
        ++c->bcast_seq;

        NK_UNLOCK(&c->lock);

        DEBUG_PRINT("Condvar broadcasting on (%p) (core=%u)\n", (void*)c, my_cpu_id());
        ssem_post(c->sem, c->nwaiters);
        NK_PROFILE_EXIT();
	//nk_wait_queue_wake_all(c->wait_queue);
        return 0;

    }

    NK_UNLOCK(&c->lock);
    NK_PROFILE_EXIT();
    return 0;
	
} /* pthread_cond_broadcast */
