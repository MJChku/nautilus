/* pte_types.h  */
#ifndef PTE_TYPES_H
#define PTE_TYPES_H
#include <nautilus/list.h>
#include <nautilus/semaphore.h>
#include <nautilus/spinlock.h>
#include <nautilus/libccompat.h>
#include <nautilus/waitqueue.h>
#include <nautilus/ticketlock.h>

#define TICKET_LOCK 0

struct ticketmutex{
 union  nk_ticket_lock lock;
}; //not used 

struct pmutex{
  spinlock_t lock;
  uint8_t  flags;
};

struct ticket_semaphore{
  int sleepcount;
  union nk_ticket_lock lock;
  int count;
  nk_wait_queue_t  *wait_queue;
  uint8_t flags;
};

struct psemaphore{
  int sleepcount;
  spinlock_t lock;
  int count;
  nk_wait_queue_t  *wait_queue;
  uint8_t flags;
};

struct thread_with_signal{
  nk_thread_id_t tid;
  uint8_t priority;
  uint8_t signal;
  // nk_wait_queue_t *in_queue;
};

//typedef  struct pmutex* pte_osMutexHandle;
#if TICKET_LOCK
typedef  union nk_ticket_lock* pte_osMutexHandle;
#else
typedef  struct pmutex* pte_osMutexHandle;
#endif

typedef  nk_thread_fun_t pte_osThreadEntryPoint;
//typedef  struct thread_with_signal* pte_osThreadHandle;
typedef nk_thread_id_t pte_osThreadHandle;

typedef struct nk_semaphore* pte_osSemaphoreHandle;
/*
#if TICKET_LOCK
typedef struct ticket_semaphore* pte_osSemaphoreHandle;
#else
typedef struct psemaphore* pte_osSemaphoreHandle;
#endif
*/
typedef enum _pthread_signal{
  NK_THREAD_NORM,
  NK_THREAD_CANCEL
}pthread_signal;

  
typedef nk_thread_fun_t pte_osThreadEntryPoint;
typedef int pid_t;
typedef unsigned int mode_t;

struct timeb
{
  time_t time;
  unsigned short millitm;
  short timezone;
  short dstflag;
};

#endif /* PTE_TYPES_H */
