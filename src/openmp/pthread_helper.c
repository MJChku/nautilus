#include <nautilus/nautilus.h>
#include <nautilus/libccompat.h>
#include <nautilus/thread.h>
#include <nautilus/errno.h>
#include <nautilus/random.h>
#include <nautilus/env.h>
//kmp
#include <nautilus/spinlock.h>
#define __PTHREAD_COND_SHARED_MASK 1
#define PTHREAD_MUTEX_PSHARED_BIT 128


#define ERROR(fmt, args...) ERROR_PRINT("pthread: " fmt, ##args)
#define DEBUG(fmt, args...) DEBUG_PRINT("pthread: " fmt, ##args)
#define INFO(fmt, args...)   INFO_PRINT("pthread: " fmt, ##args)

/* int pthread_mutexattr_init(pthread_mutexattr_t *attr){ */

/*   DEBUG("pthread_mutexattr_init\n"); */
/*   /\* if (sizeof (struct pthread_mutexattr) != sizeof (pthread_mutexattr_t)) *\/ */
/*   /\*      memset (attr, '\0', sizeof (*attr)); *\/ */
/*   /\* We use bit 31 to signal whether the mutex is going to be */
/*      process-shared or not.  By default it is zero, i.e., the mutex is */
/*      not process-shared.  *\/ */
/*   ((struct pthread_mutexattr *) attr)->mutexkind = PTHREAD_MUTEX_NORMAL; */
/* DEBUG("FIN"); */
/*   return 0;  */
/* } */
/* int pthread_cond_init(pthread_cond_t *cond, */
/*               const pthread_condattr_t * cond_attr){ */
/*   DEBUG("pthread_cond_init"); */
/*   struct pthread_condattr *icond_attr = (struct pthread_condattr_t *) cond_attr; */
/*   memset(cond, 0, sizeof(pthread_cond_t)); */
/*   if (icond_attr != NULL && (icond_attr->value & 1) != 0) */
/*      cond->__data.__wrefs |= __PTHREAD_COND_SHARED_MASK; */
/*   DEBUG("FIN"); */
/*   return 0; */
/* } */


/* static const struct pthread_mutexattr default_mutexattr = */
/*   { */
/*     /\* Default is a normal mutex, not shared between processes.  *\/ */
/*     .mutexkind = PTHREAD_MUTEX_NORMAL */
/*   }; */

/* int pthread_mutex_init(pthread_mutex_t * mutex, */
/*         const pthread_mutexattr_t * mutexattr){ */

/*   DEBUG("pthread_mutex_init\n"); */
/*   mutex = malloc(sizeof(pthread_mutex_t)); */
/*   /\* DEBUG("pthread_mutex_init\n"); *\/ */
/*   /\* const struct pthread_mutexattr *imutexattr; *\/ */
  
/*   /\* imutexattr = ((const struct pthread_mutexattr *) mutexattr ?: &default_mutexattr); *\/ */

/*   /\* /\\* Clear the whole variable.  *\\/ *\/ */
/*   /\* memset (mutex, '\0', __SIZEOF_PTHREAD_MUTEX_T); *\/ */
/*   /\* /\\* Copy the values from the attribute.  *\\/ *\/ */
/*   /\* int mutex_kind = imutexattr->mutexkind & ~PTHREAD_MUTEXATTR_FLAG_BITS; *\/ */

/*   /\* mutex_kind |= PTHREAD_MUTEX_PSHARED_BIT; *\/ */
/*   /\* mutex->__data.__kind = mutex_kind; *\/ */

/*   /\* //atomic_store_relaxed (&(mutex->__data.__kind), mutex_kind); *\/ */
/*   /\* DEBUG("FIN"); *\/ */
/*   return 0; */
   
/* } */
/* int pthread_condattr_init(pthread_condattr_t *attr){ */

/*   DEBUG("pthread_condattr_init\n"); */
/*   struct pthread_condattr *iattr = (struct pthread_condattr *) attr; */
/*   /\* Default is not pshared and CLOCK_REALTIME.  *\/ */
/*   // iattr-> value = CLOCK_REALTIME << 1; */
/*   return 0; */
/* } */

/* //#define pthread_key_t nk_tls_key_t */
/* int pthread_key_create(pthread_key_t *key, void (*destructor)(void*)) */
/* { */
/*   DEBUG("pthread_key_create\n"); */
/*   DEBUG("FIN"); */
/*   return nk_tls_key_create(key, destructor); */
/* } */

/* int pthread_attr_init (pthread_attr_t *__attr){ */
/*   DEBUG(" pthread_attr_init\n"); */
/*   DEBUG("FIN"); */
/*   return 0; */
/* } */


/* int pthread_setspecific (pthread_key_t key, const void *value) */
/* { */
/*   nk_tls_set(key, value);  */
/*   return 0; */
/* } */

/* void* pthread_getspecific (pthread_key_t key) */
/* { */
/*   return  nk_tls_get(key);  */
/*   //return 0; */
/* } */


/* pthread_t pthread_self(void){ */
/*     return get_cur_thread(); */
/* } */

int pthread_atfork (void (*prepare) (void), void (*parent) (void),
                  void (*child) (void))
{
  DEBUG("register pthread atfork\n");
  return 0;
}

int sigaction(){
  DEBUG("sigaction\n");
  return 0;
}

int sigemptyset(){
  DEBUG("sigemptyset\n");
  return 0;
}

int atexit(){
  DEBUG("atexit\n");
  // nk_thread_exit();
  return 0;
}


/* int pthread_cond_wait(void){ */
/*   nk_yield(); */
/*   return 0; */
/* } */

/* int pthread_mutex_lock(pthread_mutex_t *mutex){ */
/*   mutex->lock_flags = spin_lock_irq_save(&mutex->spin_lock); */
/*   return 0; */
/* } */

/* //int pthread_mutex_trylock(pthread_mutex_t *mutex); */

/* int pthread_mutex_unlock(pthread_mutex_t *mutex){ */
/*   spin_unlock_irq_restore(&mutex->spin_lock, mutex->lock_flags); */
/*   return 0; */
/* } */

/* int pthread_attr_setstacksize(){ */
/*   return 0; */
/* } */

/*  int pthread_create(pthread_t *thread, const pthread_attr_t *attr, */
/* 		    void *(*start_routine) (void *), void *arg){ */

/*    nk_thread_fun_t fun; */
/*    uint8_t is_detached = false; */
/*    nk_stack_size_t stack_size = 0x4000; */
/*    nk_thread_id_t * tid = malloc(sizeof(nk_thread_id_t)); */
/*    nk_thread_create(fun, NULL, NULL,false,stack_size, tid, 0); */
/*    *thread = tid; */
/*    DEBUG("pthread_create_ %d \n", tid); */
/*    return 0; */
/*  } */

/* int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate){ */
/*   return 0;    */
/* } */

/* int pthread_attr_destroy(){ */
/*   return 0; */
/* } */
