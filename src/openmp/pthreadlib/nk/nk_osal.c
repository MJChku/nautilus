
#include "pte_types.h"
#include "pte_osal.h"
#include <nautilus/libccompat.h>
#include <nautilus/thread.h>
#include <nautilus/scheduler.h>
#include <nautilus/semaphore.h>

#define ERROR(fmt, args...) ERROR_PRINT("embpthread: " fmt, ##args)
#define DEBUG(fmt, args...)
#define INFO(fmt, args...)   INFO_PRINT("embpthread: " fmt, ##args)

#define DEBUG(fmt, args...)
#ifdef NAUT_CONFIG_OPENMP_RT_DEBUG
#undef DEBUG
#define DEBUG(fmt, args...) DEBUG_PRINT("empthread: " fmt, ##args)
#endif

#define STATE_UNLOCK(a,b) spin_unlock_irq_restore(a,b)
#define STATE_TRY_LOCK(a,b) spin_try_lock_irq_save(a,b)
#define STATE_LOCK(a) spin_lock_irq_save(a)

/* #define SEMAPHORE_LOCK_CONF uint8_t _semaphore_lock_flags */
/* #define SEMAPHORE_LOCK(s) _semaphore_lock_flags = spin_lock_irq_save(&(s)->lock) */
/* #define SEMAPHORE_TRY_LOCK(s) spin_try_lock_irq_save(&(s)->lock,&_semaphore_lock_flags) */
/* #define SEMAPHORE_UNLOCK(s) spin_unlock_irq_restore(&(s)->lock, _semaphore_lock_flags) */
/* #define SEMAPHORE_UNIRQ(s) irq_enable_restore(_semaphore_lock_flags) */

#define poffsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#define pcontainer_of(ptr, type, member) ({                      \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - offsetof(type,member) );})



/**
 * Provides a hook for the OSAL to implement any OS specific initialization.  This is guaranteed to be
 * called before any other OSAL function.
 */
pte_osResult pte_osInit(void){
  return PTE_OS_OK;
}

/**
 * Creates a mutex
 *
 * @param pHandle  Set to the handle of the newly created mutex.
 *
 * @return PTE_OS_OK - Mutex successfully created
 * @return PTE_OS_NO_RESOURCESs - Insufficient resources to create mutex
 */
pte_osResult pte_osMutexCreate(pte_osMutexHandle *pHandle){

   *pHandle = malloc(sizeof(struct pmutex));
   spinlock_init(&((*pHandle)->lock));
   DEBUG("osMutexCreate\n");
   return PTE_OS_OK;
}

/**
 * Deletes a mutex and frees any associated resources.
 *
 * @param handle Handle of mutex to delete.
 *
 * @return PTE_OS_OK - Mutex successfully deleted.
 */
pte_osResult pte_osMutexDelete(pte_osMutexHandle handle){
  spinlock_deinit(&(handle->lock));
  DEBUG("osMutexDelete\n");
  return PTE_OS_OK;
}

/**
 * Locks the mutex
 *
 * @param handle Handle of mutex to lock.
 *
 * @return PTE_OS_OK - Mutex successfully locked.
 */
pte_osResult pte_osMutexLock(pte_osMutexHandle handle){
  int ans= -1;
  handle->flags = STATE_LOCK(&(handle->lock));
  DEBUG("osMutexLock\n");
  return PTE_OS_OK;
  
}

/**
 * Locks the mutex, returning after @p timeoutMsecs if the resources is not
 * available.  Can be used for polling mutex by using @p timeoutMsecs of zero.
 *
 * @param handle Handle of mutex to lock.
 * @param timeoutMsecs Number of milliseconds to wait for resource before returning.
 *
 * @return PTE_OS_OK - Mutex successfully locked.
 * @return PTE_OS_TIMEOUT - Timeout expired before lock was obtained.
 */
pte_osResult pte_osMutexTimedLock(pte_osMutexHandle handle, unsigned int timeout){
  unsigned int start = (unsigned int) time(NULL);
  unsigned int end = start;
  int res = -1;
  while( (end-start) <timeout ){
    res=STATE_TRY_LOCK(&(handle->lock), &(handle->flags) );
    DEBUG("osMutexTimedLock\n");
    end = (unsigned int)time(NULL);
    if(res == 0){
      return PTE_OS_OK;
    }
  }
  return PTE_OS_TIMEOUT;
}

/**
 * Unlocks the mutex
 *
 * @param handle Handle of mutex to unlock
 *
 * @return PTE_OS_OK - Mutex successfully unlocked.
 */
pte_osResult pte_osMutexUnlock(pte_osMutexHandle handle){
  STATE_UNLOCK(&(handle->lock), handle->flags);
  DEBUG("osMutexUnlock\n");
  return PTE_OS_OK;
}

/**
 * Creates a new thread.  The thread must be started in a suspended state - it will be
 * explicitly started when pte_osThreadStart() is called.
 *
 * @param entryPoint Entry point to the new thread.
 * @param stackSize The initial stack size, in bytes.  Note that this can be considered a minimum -
 *                  for instance if the OS requires a larger stack space than what the caller specified.
 * @param initialPriority The priority that the new thread should be initially set to.
 * @param argv Parameter to pass to the new thread.
 * @param ppte_osThreadHandle set to the handle of the new thread.
 *
 * @return PTE_OS_OK - New thread successfully created.
 * @return PTE_OS_NO_RESOURCESs - Insufficient resources to create thread
 */
pte_osResult pte_osThreadCreate(pte_osThreadEntryPoint entryPoint,
                                int stackSize,
                                int initialPriority,
                                void *argv,
                                pte_osThreadHandle* handle)
{
  //pte_osThreadEntryPoint is nk_thread_fun
  //pte_osThreadhandle will be nk_thread_id
  pte_osThreadHandle handleobj = malloc(sizeof(struct thread_with_signal));
  memset(handleobj,0,sizeof(struct thread_with_signal));
  int ret = nk_thread_create(entryPoint, argv, NULL, false,(nk_stack_size_t) stackSize, &(handleobj->tid),-1);
  if (ret != 0){
    DEBUG("create error exit\n");
    nk_thread_exit(NULL);
  }
  *handle = handleobj;
  struct nk_thread* thread = (struct nk_thread*) (*handle)->tid;
  DEBUG("osThreadCreate %08x, %ld\n",  thread, thread->tid);
 
  return PTE_OS_OK;
}
/**
 * Starts executing the specified thread.
 *
 * @param osThreadHandle handle of the thread to start.
 *
 * @return PTE_OS_OK - thread successfully started.
 */
pte_osResult pte_osThreadStart(pte_osThreadHandle handle){
  //struct nk_thread* thread = (struct nk_thread*) &(handle->tid);
  //thread->sched_state->constraints.type = APERIODIC;
  //nk_thread_run(&(handle->tid));
  nk_thread_run(handle->tid);
  DEBUG("osThreadStart %08x\n", handle);

  return PTE_OS_OK;
}

/**
 * Causes the current thread to stop executing.
 *
 * @return Never returns (thread terminated)
 */
void pte_osThreadExit(){
  DEBUG("osthread EXIT\n");
  nk_thread_exit(NULL);
}

/**
 * Waits for the specified thread to end.  If the thread has already terminated, this returns
 * immediately.
 *
 * @param threadHandle Handle fo thread to wait for.
 *
 * @return PTE_OS_OK - specified thread terminated.
 */
pte_osResult pte_osThreadWaitForEnd(pte_osThreadHandle threadHandle){
  DEBUG("pte osThread Wait For End\n");
  nk_thread_t *thread = (nk_thread_t*) &(threadHandle->tid);
  while(true){
    if(thread->status==NK_THR_EXITED){
      return PTE_OS_OK;
    }
    nk_yield();
  }  
}

/**
 * Returns the handle of the currently executing thread.
 */
pte_osThreadHandle pte_osThreadGetHandle(void){
  //note pte_osThreadHandle is a pointer of struct _thread_with_signal
  nk_thread_t* thethread = get_cur_thread();
  DEBUG("osThreadGetHandle\n");
  return (pte_osThreadHandle) pcontainer_of(thethread,struct thread_with_signal, tid);
  // return (pte_osThreadHandle)
}

/**
 * Returns the priority of the specified thread.
 */
int pte_osThreadGetPriority(pte_osThreadHandle threadHandle){
  return 0;
}

/**
 * Sets the priority of the specified thread.
 *
 * @return PTE_OS_OK - thread priority successfully set
 */
pte_osResult pte_osThreadSetPriority(pte_osThreadHandle threadHandle, int newPriority){
  return PTE_OS_OK;

}

/**
 * Frees resources associated with the specified thread.  This is called after the thread has terminated
 * and is no longer needed (e.g. after pthread_join returns).  This call will always be made
 * from a different context than that of the target thread.
 */
pte_osResult pte_osThreadDelete(pte_osThreadHandle handle){
  DEBUG("osThreadDelete\n");
  nk_thread_destroy(&(handle->tid));
}

/**
 * Frees resources associated with the specified thread and then causes the thread to exit.
 * This is called after the thread has terminated and is no longer needed (e.g. after
 * pthread_join returns).  This call will always be made from the context of the target thread.
 */
pte_osResult pte_osThreadExitAndDelete(pte_osThreadHandle handle){
  //handle->signal = NK_THREAD_CANCEL;
  //nk_thread_cancel(&(handle->tid));
  DEBUG("osThreadExitDelete\n");
  nk_thread_destroy(&(handle->tid));
}

/**
 * Cancels the specified thread.  This should cause pte_osSemaphoreCancellablePend() and for pte_osThreadCheckCancel()
 * to return @p PTE_OS_INTERRUPTED.
 *
 * @param threadHandle handle to the thread to cancel.
 *
 * @return Thread successfully canceled.
 */
pte_osResult pte_osThreadCancel(pte_osThreadHandle handle){
  DEBUG("osThreadCancel \n");
   handle->signal = NK_THREAD_CANCEL;
   // nk_thread_cancel(&(handle->tid));  
}

/**
 * Check if pte_osThreadCancel() has been called on the specified thread.
 *
 * @param threadHandle handle of thread to check the state of.
 *
 * @return PTE_OS_OK - Thread has not been cancelled
 * @return PTE_OS_INTERRUPTED - Thread has been cancelled.
 */
pte_osResult pte_osThreadCheckCancel(pte_osThreadHandle handle){
    nk_thread_t * thethread = (nk_thread_t*)&(handle->tid);
    DEBUG("osThreadCheckCancel\n");
    if (thethread->status ==NK_THR_EXITED){
      return PTE_OS_OK;
    }else{
      return PTE_OS_INTERRUPTED;
    }
}

/**
 * Causes the current thread to sleep for the specified number of milliseconds.
 */
void pte_osThreadSleep(unsigned int msecs){
  //DEBUG("os thread start to sleep\n");
  //nk_thread_exit(NULL);
  unsigned int start = (unsigned int) time(NULL);
  unsigned int end = start;
  int res = -1;
  while( (end-start) <msecs ){
    nk_yield();
    end = (unsigned int)time(NULL);
    //DEBUG("os Thread Sleep end %d time %d \n", end-start, msecs);
  }
}

/**
 * Returns the maximum allowable priority
 */
int pte_osThreadGetMaxPriority(){
  return 3;

}

/**
 * Returns the minimum allowable priority
 */
int pte_osThreadGetMinPriority(){
  return 0;
}

/**
 * Returns the priority that should be used if the caller to pthread_create doesn't
 * explicitly set one.
 */
int pte_osThreadGetDefaultPriority(){
  return 0;
}

//@}


/** @name Semaphores */
//@{

/**
 * Creates a semaphore
 *
 * @param initialValue Initial value of the semaphore
 * @param pHandle  Set to the handle of the newly created semaphore.
 *
 * @return PTE_OS_OK - Semaphore successfully created
 * @return PTE_OS_NO_RESOURCESs - Insufficient resources to create semaphore
 */
pte_osResult pte_osSemaphoreCreate(int initialValue, pte_osSemaphoreHandle *pHandle){
  //pte_osSemaphoreHandle is nk_semaphore
  *pHandle = malloc(sizeof(struct psemaphore));
  spinlock_init(&((*pHandle)->lock));
  (*pHandle)->count = 0;
  //*pHandle->sem = nk_semaphore_create(NULL,initialValue,0,NULL);   
  DEBUG("osSemaphoreCreate\n");
  return PTE_OS_OK;
}

/**
 * Deletes a semaphore and frees any associated resources.
 *
 * @param handle Handle of semaphore to delete.
 *
 * @return PTE_OS_OK - Semaphore successfully deleted.
 */
pte_osResult pte_osSemaphoreDelete(pte_osSemaphoreHandle handle){
  //nk_semaphore_release(handle);
  spinlock_deinit(&(handle->lock));
  free(handle);
  DEBUG("osSemaphoreDelete\n");
  return PTE_OS_OK;
}

/**
 * Posts to the semaphore
 *
 * @param handle Semaphore to release
 * @param count  Amount to increment the semaphore by.
 *
 * @return PTE_OS_OK - semaphore successfully released.
 */
pte_osResult pte_osSemaphorePost(pte_osSemaphoreHandle handle, int count){
    DEBUG("osSemaphorePost\n");
    handle->flags = STATE_LOCK(&(handle->lock));
    handle->count += count;
    STATE_UNLOCK(&(handle->lock), handle->flags);
    return PTE_OS_OK;
}

/**
 * Acquire a semaphore, returning after @p timeoutMsecs if the semaphore is not
 * available.  Can be used for polling a semaphore by using @p timeoutMsecs of zero.
 *
 * @param handle Handle of semaphore to acquire.
 * @param pTimeout Pointer to the number of milliseconds to wait to acquire the semaphore
 *                 before returning.  If set to NULL, wait forever.
 *
 * @return PTE_OS_OK - Semaphore successfully acquired.
 * @return PTE_OS_TIMEOUT - Timeout expired before semaphore was obtained.
 */
pte_osResult pte_osSemaphorePend(pte_osSemaphoreHandle handle, unsigned int *pTimeout){
   DEBUG("osSemaphorePend\n");
   if(pTimeout == NULL){
     while(1){
       DEBUG("osSemaphorePend NULL time\n");
       handle->flags = STATE_LOCK(&(handle->lock));
       if(handle->count > 0){
         handle->count--;
	 STATE_UNLOCK(&(handle->lock), handle->flags);
	 return PTE_OS_OK;
       }else{
	 STATE_UNLOCK(&(handle->lock), handle->flags);
	 nk_yield();
       }

     }
       // nk_yield();
       //nk_thread_exit(NULL);
       //nk_yield();
       //nk_thread_exit(NULL);
   }else{
  
    DEBUG("release semaphore\n");
    unsigned int start = (unsigned int) time(NULL);
    unsigned int end = start;
    int res = -1;
    while( (end-start) < *pTimeout ){
      DEBUG("osSemaphorePend %d \n", end-start);
      handle->flags = STATE_LOCK(&(handle->lock));
      if(handle->count > 0){
         handle->count--;
	 STATE_UNLOCK(&(handle->lock), handle->flags);
	 return PTE_OS_OK;
       }else{
	 STATE_UNLOCK(&(handle->lock), handle->flags);
	 nk_yield();
       }
       end = (unsigned int)time(NULL);
    }
    return PTE_OS_TIMEOUT;
  }
}

/**
 * Acquire a semaphore, returning after @p timeoutMsecs if the semaphore is not
 * available.  Can be used for polling a semaphore by using @p timeoutMsecs of zero.
 * Call must return immediately if pte_osThreadCancel() is called on the thread waiting for
 * the semaphore.
 *
 * @param handle Handle of semaphore to acquire.
 * @param pTimeout Pointer to the number of milliseconds to wait to acquire the semaphore
 *                 before returning.  If set to NULL, wait forever.
 *
 * @return PTE_OS_OK - Semaphore successfully acquired.
 * @return PTE_OS_TIMEOUT - Timeout expired before semaphore was obtained.
 */
pte_osResult pte_osSemaphoreCancellablePend(pte_osSemaphoreHandle handle, unsigned int *pTimeout){
     nk_thread_t* thethread= get_cur_thread();
     pte_osThreadHandle oshandle = pcontainer_of(thethread, struct thread_with_signal, tid);
     DEBUG("osSemaphorecancelablepend\n");
     int res = -1;
   if(pTimeout == NULL){
     while(true){
       if(oshandle->signal == NK_THREAD_CANCEL){
	 nk_thread_exit(NULL);
         return PTE_OS_INTERRUPTED;
       }
       handle->flags = STATE_LOCK(&(handle->lock));
       if(handle->count > 0){
         handle->count--;
	 STATE_UNLOCK(&(handle->lock), handle->flags);
	 return PTE_OS_OK;
       }else{
	 STATE_UNLOCK(&(handle->lock), handle->flags);
	 nk_yield();
       }
     }
   }else{
     DEBUG("release semaphore\n");
     unsigned int start = (unsigned int) time(NULL);
     unsigned int end = start;
     while( (end-start) < *pTimeout ){
       if(oshandle->signal == NK_THREAD_CANCEL){
         return PTE_OS_INTERRUPTED;
       }
       handle->flags = STATE_LOCK(&(handle->lock));
       if(handle->count > 0){
         handle->count--;
	 STATE_UNLOCK(&(handle->lock), handle->flags);
	 return PTE_OS_OK;
       }else{
	 STATE_UNLOCK(&(handle->lock), handle->flags);
	 nk_yield();
       }
        end = (unsigned int)time(NULL);
     }
     return PTE_OS_TIMEOUT;
   }
}


/** @name Thread Local Storage */
//@{
/**
 * Sets the thread specific value for the specified key for the
 * currently executing thread.
 *
 * @param index The TLS key for the value.
 * @param value The value to save
 */
pte_osResult pte_osTlsSetValue(unsigned int key, void * value){
   DEBUG("pte_osTlsSetvalue\n");
   nk_tls_set(key, value);
   return PTE_OS_OK;
}

/**
 * Retrieves the thread specific value for the specified key for
 * the currently executing thread.  If a value has not been set
 * for this key, NULL should be returned (i.e. TLS values default
 * to NULL).
 *
 * @param index The TLS key for the value.
 *
 * @return The value associated with @p key for the current thread.
 */
void * pte_osTlsGetValue(unsigned int key){
   DEBUG("osTlsGetValue\n");
   return nk_tls_get(key);
}

/**
 * Initializes the OS TLS support.  This is called by the PTE library
 * prior to performing ANY TLS operation.
 */
void pte_osTlsInit(void){
  //skip
}

/**
 * Allocates a new TLS key.
 *
 * @param pKey On success will be set to the newly allocated key.
 *
 * @return PTE_OS_OK - TLS key successfully allocated.
 * @return PTE_OS_NO_RESOURCESs - Insufficient resources to allocate key (e.g.
 *                         maximum number of keys reached).
 */
pte_osResult pte_osTlsAlloc(unsigned int *pKey){
  DEBUG("osTlsAlloc\n");
  nk_tls_key_create(pKey, NULL);
  return PTE_OS_OK;
}

/**
 * Frees the specified TLS key.
 *
 * @param index TLS key to free
 *
 * @return PTE_OS_OK - TLS key was successfully freed.
 */
pte_osResult pte_osTlsFree(unsigned int key){
  DEBUG("osTlsFree\n");
  nk_tls_key_delete(key);
  return PTE_OS_OK;
}
//@}

/** @name Atomic operations */
//@{

/**
 * Sets the target to the specified value as an atomic operation.
 *
 * \code
 * origVal = *ptarg
 * *ptarg = val
 * return origVal
 * \endcode
 *
 * @param pTarg Pointer to the value to be exchanged.
 * @param val Value to be exchanged
 *
 * @return original value of destination
 */
int pte_osAtomicExchange(int *pTarg, int val){

  int origin = *pTarg;
  __atomic_exchange_n(pTarg,val,__ATOMIC_SEQ_CST);
  DEBUG("AtomicEXCHANGE, ORIG %d , NOW %d val %d ret %d\n", origin, *pTarg, val);
  return origin;
    //return *((int*) xchg64((void**)(&pTarg),(void*)(&val)));
 
  // return 0;
  // return origin;
  //???
  // return (int)(ret);
}

/**
 * Performs an atomic compare-and-exchange oepration on the specified
 * value.  That is:
 *
 * \code
 * origVal = *pdest
 * if (*pdest == comp)
 *   then *pdest = exchange
 * return origVal
 * \endcode
 *
 * @param pdest Pointer to the destination value.
 * @param exchange Exchange value (value to set destination to if destination == comparand)
 * @param comp The value to compare to destination.
 *
 * @return Original value of destination
 */
int pte_osAtomicCompareExchange(int *pdest, int exchange, int comp){
    DEBUG("AtomicCompareEXCHANGE");
  return __sync_val_compare_and_swap(pdest,comp,exchange);
  // return atomic_cmpswap(pdest, comp, exchange);
} 

/**
 * Adds the value to target as an atomic operation
 *
 * \code
 * origVal = *pdest
 * *pAddend += value
 * return origVal
 * \endcode
 *
 * @param pdest Pointer to the variable to be updated.
 * @param value Value to be added to the variable.
 *
 * @return Original value of destination
 */
int  pte_osAtomicExchangeAdd(int volatile* pdest, int value){
    DEBUG("AtomicEXCHANGEAdd\n");
  return __sync_fetch_and_add(pdest, value);
  //return atomic_add(pdest, value);
}

/**
 * Decrements the destination.
 *
 * \code
 * origVal = *pdest
 * *pdest++
 * return origVal
 * \endcode
 *
 * @param pdest Destination value to decrement
 *
 * @return Original destination value
 */
int pte_osAtomicDecrement(int *pdest){
    DEBUG("AtomicDecrement\n" );
  return __sync_fetch_and_sub(pdest,1);
    //return atomic_dec(pdest);
}

/**
 * Increments the destination value
 *
 * \code
 * origVal = *pdest;
 * *pdest++;
 * return origVal;
 */
int pte_osAtomicIncrement(int *pdest){
    DEBUG("AtomicIncrement\n");
  return __sync_fetch_and_add(pdest,1);
  //return atomic_inc(pdest);
}
//@}

int ftime(struct timeb *tb){
  tb->time = 0;
  tb->millitm = 0;
  tb->timezone = 0;
  tb->dstflag=0;
  return 0;
}

