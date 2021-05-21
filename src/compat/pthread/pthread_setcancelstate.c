/*
 * pthread_setcancelstate.c
 *
 */

#include "pthread.h"
#include "implement.h"


int
pthread_setcancelstate (int state, int *oldstate)
/*
 * ------------------------------------------------------
 * DOCPUBLIC
 *      This function atomically sets the calling thread's
 *      cancelability state to 'state' and returns the previous
 *      cancelability state at the location referenced by
 *      'oldstate'
 *
 * PARAMETERS
 *      state,
 *      oldstate
 *              PTHREAD_CANCEL_ENABLE
 *                      cancellation is enabled,
 *
 *              PTHREAD_CANCEL_DISABLE
 *                      cancellation is disabled
 *
 *
 * DESCRIPTION
 *      This function atomically sets the calling thread's
 *      cancelability state to 'state' and returns the previous
 *      cancelability state at the location referenced by
 *      'oldstate'.
 *
 *      NOTES:
 *      1)      Use to disable cancellation around 'atomic' code that
 *              includes cancellation points
 *
 * COMPATIBILITY ADDITIONS
 *      If 'oldstate' is NULL then the previous state is not returned
 *      but the function still succeeds. (Solaris)
 *
 * RESULTS
 *              0               successfully set cancelability type,
 *              EINVAL          'state' is invalid
 *
 * ------------------------------------------------------
 */
{
 // BOGUS();
  return 0;
  
  /* int result = 0; */
  /* pthread_t self = pthread_self (); */
  /* pte_thread_t * sp = (pte_thread_t *) self.p; */

  /* if (sp == NULL */
  /*     || (state != PTHREAD_CANCEL_ENABLE && state != PTHREAD_CANCEL_DISABLE)) */
  /*   { */
  /*     return EINVAL; */
  /*   } */

  /* /\* */
  /*  * Lock for async-cancel safety. */
  /*  *\/ */
  /* (void) pthread_mutex_lock (&sp->cancelLock); */

  /* if (oldstate != NULL) */
  /*   { */
  /*     *oldstate = sp->cancelState; */
  /*   } */

  /* sp->cancelState = state; */

  /* /\* */
  /*  * Check if there is a pending asynchronous cancel */
  /*  *\/ */
  /* if (state == PTHREAD_CANCEL_ENABLE */
  /*     && (sp->cancelType == PTHREAD_CANCEL_ASYNCHRONOUS) */
  /*     && (pte_osThreadCheckCancel(sp->threadId) == PTE_OS_INTERRUPTED) ) */
  /*   { */
  /*     sp->state = PThreadStateCanceling; */
  /*     sp->cancelState = PTHREAD_CANCEL_DISABLE; */
  /*     (void) pthread_mutex_unlock (&sp->cancelLock); */
  /*     pte_throw (PTE_EPS_CANCEL); */

  /*     /\* Never reached *\/ */
  /*   } */

  /* (void) pthread_mutex_unlock (&sp->cancelLock); */

  /* return (result); */

}				/* pthread_setcancelstate */
