#include "ssem.h"

#include <nautilus/list.h>
#include <nautilus/scheduler.h>

#define SLEEP_COUNT  0x10 
#define SPIN_TIME_NS 5000

void ssem_init(simple_sem_t *s, int initial_count){
  NK_LOCK_INIT(&s->lock);
  s->count = initial_count;
  s->sleepcount = 0;
  s->wait_queue = nk_wait_queue_create(NULL);
}

void ssem_destroy(simple_sem_t *s){
  
   NK_LOCK(&s->lock);
   nk_wait_queue_wake_all(s->wait_queue);
   nk_wait_queue_destroy(s->wait_queue);
   NK_UNLOCK(&s->lock);
   free(s);
   return;

}
void ssem_post(simple_sem_t *s, int count){
    
    NK_LOCK(&s->lock);
    s->count+=count;
    DEBUG("ssem_post:  count: %d \n", s->count);
    if(s->sleepcount > 0){
      DEBUG("ssem_post: sleep count: %d \n", s->sleepcount);
      if(count > 1){
	 nk_wait_queue_wake_all(s->wait_queue); 
         s->sleepcount = 0;	 
      }else{

      int a = count;
      if (a > s->sleepcount){
        a = s->sleepcount;
      }
      s->sleepcount -= a;
      while(a--){
        nk_wait_queue_wake_one(s->wait_queue);
      }
      }
    }
    NK_UNLOCK(&s->lock);
}

int ssem_timedwait( simple_sem_t *s, uint64_t timeout_ns){

   //int ori =  __sync_fetch_and_sub(&(s->count), 1);
   uint64_t start = nk_sched_get_realtime(), now;

   do{
      if (s == NULL){
        ERROR("ssem_timedwait NULL sem\n");
        return 0;
      }
      NK_LOCK(&s->lock);
      if(s->count > 0){
        s->count--;
        break;
      }
      
      NK_UNLOCK(&s->lock);
      nk_yield();
      //__asm__("pause");
      
      now = nk_sched_get_realtime();
      
      if( now-start > timeout_ns){
         //NK_UNLOCK(&s->lock);
	 return 1;
      }

   }while(1);

    NK_UNLOCK(&s->lock);
    return 0;
}

inline void force_sleep(simple_sem_t *sem){
       // we need to gracefully put ourselves to sleep
        nk_thread_t *t = get_cur_thread();
        // disable preemption early since interrupts may remain on given our locking model
        preempt_disable();
        // onto the semaphore's wait queue we go
        t->status = NK_THR_WAITING;
        nk_wait_queue_enqueue(sem->wait_queue,t);
        // and go to sleep - this will also release the lock
        // and reenable preemption
	nk_sched_sleep(&sem->lock);

	return;


   //nk_wait_queue_sleep_extended(t, 0, 0);
} 


void ssem_wait( simple_sem_t *s){

  int busy_wait = 0;
  while(1){	
    busy_wait = ssem_timedwait(s, SPIN_TIME_NS);
    if (busy_wait == 0){
       return;
    }else{
	   NK_LOCK(&s->lock);
	   //check condition again before sleep;
	   if(s->count > 0){
             s->count--;
	     NK_UNLOCK(&s->lock);
             return;
           }
           s->sleepcount++;
           DEBUG("ssem wait in queue sem %p\n", s);
	   force_sleep(s);
           DEBUG("ssem wait wakeup in queue sem %p\n", s);
    }
  }


}
