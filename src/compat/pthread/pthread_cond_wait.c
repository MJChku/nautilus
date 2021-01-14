/*
 * pthread_cond_wait.c
 *
 */

#include "pthread.h"
#include "implement.h"
#include "pthread_cond_common.h"

static int no_check(void *s){
  return 0;
}

struct op {
     nk_timer_t *timer;
};

static int check_timer(void *s)
{
    struct op *o = (struct op *)s;
    return __sync_fetch_and_or(&o->timer->state,0)==NK_TIMER_SIGNALLED;
}

int
pthread_cond_timedwait (pthread_cond_t * c,
                    pthread_mutex_t * l, const struct timespec *abstime)
{
   
    NK_PROFILE_ENTRY();
    int result = 0;
    if (c == NULL || abstime == NULL){
      return EINVAL;
		      
    }

    uint64_t start = nk_sched_get_realtime();
    uint64_t now = start;
    uint64_t timeout_ns = abstime->tv_sec*1000000000ULL+abstime->tv_nsec;
    DEBUG("Condvar wait on (%p) mutex=%p\n", (void*)c, (void*)l);

    NK_LOCK(&c->lock);

    /* now we can unlock the mutex and go to sleep */
    pthread_mutex_unlock(l);

    ++c->nwaiters;
    ++c->main_seq;

    unsigned long long val;
    unsigned long long seq;
    unsigned bc = *(volatile unsigned*)&(c->bcast_seq);
    val = seq = c->wakeup_seq;

    do {
      if((now-start) > timeout_ns){
	result = ETIMEDOUT; //timeout
	//waited long enough
	goto bcout;
	
       }
        
        NK_UNLOCK(&c->lock); 
       
	ssem_timedwait(c->sem, timeout_ns-(now-start));
 
        NK_LOCK(&c->lock);
	

        if (bc != *(volatile unsigned*)&(c->bcast_seq)) {
            goto bcout;
        }

        val = *(volatile unsigned long long*)&(c->wakeup_seq);
        now = nk_sched_get_realtime();
	
    } while (val == seq || val == *(volatile unsigned long long*)&(c->woken_seq));

    ++c->woken_seq;

bcout:

    --c->nwaiters;

    NK_UNLOCK(&c->lock);

    /* reacquire lock */
    pthread_mutex_lock(l);

    NK_PROFILE_EXIT();

    return result;
}

int
pthread_cond_wait (pthread_cond_t * c, pthread_mutex_t * l)
{
    NK_PROFILE_ENTRY();
    if (c == NULL){
      return EINVAL;
		      
    }
    DEBUG("Condvar wait on (%p) mutex=%p\n", (void*)c, (void*)l);

    NK_LOCK(&c->lock);

    /* now we can unlock the mutex and go to sleep */
    pthread_mutex_unlock(l);

    ++c->nwaiters;
    ++c->main_seq;

    unsigned long long val;
    unsigned long long seq;
    unsigned bc = *(volatile unsigned*)&(c->bcast_seq);
    val = seq = c->wakeup_seq;

    do {

        NK_UNLOCK(&c->lock);
	
    DEBUG("Condvar before ssem wait on (%p) mutex=%p\n", (void*)c, (void*)l);
	ssem_wait(c->sem);
        
    DEBUG("Condvar after ssem wait on (%p) mutex=%p\n", (void*)c, (void*)l);
	NK_LOCK(&c->lock);

        if (bc != *(volatile unsigned*)&(c->bcast_seq)) {
            goto bcout;
        }

        val = *(volatile unsigned long long*)&(c->wakeup_seq);
   } while (val == seq || val == *(volatile unsigned long long*)&(c->woken_seq));

    ++c->woken_seq;

bcout:

    --c->nwaiters;

    NK_UNLOCK(&c->lock);

    /* reacquire lock */
    pthread_mutex_lock(l);

    NK_PROFILE_EXIT();

    return 0;

}				/* pthread_cond_wait */

