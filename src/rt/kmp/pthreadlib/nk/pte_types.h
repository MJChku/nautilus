/* pte_types.h  */

#ifndef PTE_TYPES_H
#define PTE_TYPES_H
#include <nautilus/list.h>
#include <nautilus/semaphore.h>
#include <nautilus/thread.h>
#include <nautilus/spinlock.h>
#include <nautilus/libccompat.h>
struct pmutex{
  spinlock_t lock;
  uint8_t  flags;
};

struct psemaphore{
  spinlock_t lock;
  int count;
  uint8_t flags;
};
struct thread_with_signal{
  nk_thread_id_t tid;
  uint8_t signal;
};

typedef  struct pmutex* pte_osMutexHandle;
typedef  nk_thread_fun_t pte_osThreadEntryPoint;
typedef  struct thread_with_signal* pte_osThreadHandle;
typedef struct psemaphore* pte_osSemaphoreHandle;

typedef enum _pthread_signal{
  NK_THREAD_NORM,
  NK_THREAD_CANCEL
}pthread_signal;

  
typedef nk_thread_fun_t pte_osThreadEntryPoint;
typedef int pid_t;
typedef unsigned int mode_t;
/* struct timespec */
/* { */
/*   time_t  tv_sec; */
/*   long    tv_nsec; */
/* }; */

struct timeb
{
  time_t time;
  unsigned short millitm;
  short timezone;
  short dstflag;
};

#endif /* PTE_TYPES_H */
