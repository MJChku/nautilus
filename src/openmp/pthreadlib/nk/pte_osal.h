#ifndef _PTE_OSAL_H_
#define _PTE_OSAL_H_
#include "pte_types.h"
#include "pte_generic_osal.h"
#include <nautilus/nautilus.h>

#define calloc(a,b) malloc(a*b)


/* struct pmutex{ */
/*   spinlock_t lock; */
/*   uint8_t  flags;    */
/* }; */
/* struct _thread_with_signal{ */
/*   nk_thread_t tid; */
/*   uint8_t signal; */
/* }; */

/* typedef pte_osMutexHandle struct pmutex* */
/* typedef pte_osThreadEntryPoint  nk_thread_fun */
/* typedef pte_osThreadhandle  _thread_with_singal* */
/* typedef pte_osSemaphoreHandle nk_semaphore* */

/* typedef enum _pthread_signal{ */
/*   0; */
/*   NK_THREAD_CANCEL; */
/* }pthread_signal; */

/* struct timespec */
/* { */
/*   time_t  tv_sec; */
/*   long    tv_nsec; */
/* }; */

/* struct timeb */
/* { */
/*   time_t time; */
/*   unsigned short millitm; */
/*   short timezone; */
/*   short dstflag; */
/* }; */

#ifndef EPERM
#define EPERM           1
#endif // EPERM

#ifndef ESRCH
#define ESRCH           3
#endif // ESRCH

#ifndef EINTR
#define EINTR           4
#endif // EINTR

#ifndef EIO
#define EIO             5
#endif // EIO

#ifndef EAGAIN
#define EAGAIN          11
#endif // EAGAIN

#ifndef ENOMEM
#define ENOMEM          12
#endif // ENOMEM

#ifndef EBUSY
#define EBUSY           16
#endif // EBUSY

#ifndef EINVAL
#define EINVAL          22
#endif // EINVAL

#ifndef ENOSPC
#define ENOSPC          28
#endif // ENOSPC

#ifndef EDEADLK
#define EDEADLK         35
#endif /* EDEADLK */

#ifndef ENOSYS
#define ENOSYS          38
#endif /* ENOSYS */

#ifndef ENOTSUP
#define ENOTSUP         95
#endif /* ENOTSUP */

#ifndef ETIMEDOUT
#define ETIMEDOUT       116
#endif // ETIMEDOUT

#ifndef ENOENT
#define ENOENT 7
#endif

#ifndef ERANGE
#define ERANGE 9
#endif 


#endif
