/*
 * pthread_setcanceltype.c
 */

#include "pthread.h"
#include "implement.h"


int
pthread_setcanceltype (int type, int *oldtype)
/*
 * ------------------------------------------------------
 * DOCPUBLIC
 *      This function atomically sets the calling thread's
 *      cancelability type to 'type' and returns the previous
 *      cancelability type at the location referenced by
 *      'oldtype'
 *
 * PARAMETERS
 *      type,
 *      oldtype
 *              PTHREAD_CANCEL_DEFERRED
 *                      only deferred cancelation is allowed,
 *
 *              PTHREAD_CANCEL_ASYNCHRONOUS
 *                      Asynchronous cancellation is allowed
 *
 *
 * DESCRIPTION
 *      This function atomically sets the calling thread's
 *      cancelability type to 'type' and returns the previous
 *      cancelability type at the location referenced by
 *      'oldtype'
 *
 *      NOTES:
 *      1)      Use with caution; most code is not safe for use
 *              with asynchronous cancelability.
 *
 * COMPATIBILITY ADDITIONS
 *      If 'oldtype' is NULL then the previous type is not returned
 *      but the function still succeeds. (Solaris)
 *
 * RESULTS
 *              0               successfully set cancelability type,
 *              EINVAL          'type' is invalid
 *              EPERM           Async cancellation is not supported.
 *
 * ------------------------------------------------------
 */
{
    //BOGUS();
  int result = 0;
/*   pthread_t self = pthread_self (); */
/*   pte_thread_t * sp = (pte_thread_t *) self.p; */

#ifndef PTE_SUPPORT_ASYNC_CANCEL
  if (type == PTHREAD_CANCEL_ASYNCHRONOUS)
    {
      /* Async cancellation is not supported at this time.  See notes in
       * pthread_cancel.
       */
      return EPERM;
    }
#endif /* PTE_SUPPORT_ASYNC_CANCEL */

  if ( (type != PTHREAD_CANCEL_DEFERRED
          && type != PTHREAD_CANCEL_ASYNCHRONOUS))
    {
      return EINVAL;
    }

  /*
   * Lock for async-cancel safety.
   */
  /* (void) pthread_mutex_lock (&sp->cancelLock); *\/ */

/*   if (oldtype != NULL) */
/*     { */
/*       *oldtype = sp->cancelType; */
/*     } */

/*   sp->cancelType = type; */

/*   /\* */
/*    * Check if there is a pending asynchronous cancel */
/*    *\/ */

/*   if (sp->cancelState == PTHREAD_CANCEL_ENABLE */
/*       && (type == PTHREAD_CANCEL_ASYNCHRONOUS) */
/*       && (pte_osThreadCheckCancel(sp->threadId) == PTE_OS_INTERRUPTED) ) */
/*     { */
/*       sp->state = PThreadStateCanceling; */
/*       sp->cancelState = PTHREAD_CANCEL_DISABLE; */
/*       (void) pthread_mutex_unlock (&sp->cancelLock); */
/*       pte_throw (PTE_EPS_CANCEL); */

/*       /\* Never reached *\/ */
/*     } */

/*   (void) pthread_mutex_unlock (&sp->cancelLock); */

  return (result);

}				/* pthread_setcanceltype*/
