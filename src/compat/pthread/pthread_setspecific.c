/*
 * pthread_setspecific.c
 */


#include "pthread.h"
#include "implement.h"



int
pthread_setspecific (pthread_key_t key, const void *value)
/*
 * ------------------------------------------------------
 * DOCPUBLIC
 *      This function sets the value of the thread specific
 *      key in the calling thread.
 *
 * PARAMETERS
 *      key
 *              an instance of pthread_key_t
 *      value
 *              the value to set key to
 *
 *
 * DESCRIPTION
 *      This function sets the value of the thread specific
 *      key in the calling thread.
 *
 * RESULTS
 *              0               successfully set value
 *              EAGAIN          could not set value
 *              ENOENT          SERIOUS!!
 *
 * ------------------------------------------------------
 */
{
//  BOGUS();
  return nk_tls_set(key, value);
  
  /* pthread_t self; */
  /* int result = 0; */

  /* if (key != pte_selfThreadKey) */
  /*   { */
  /*     /\* */
  /*      * Using pthread_self will implicitly create */
  /*      * an instance of pthread_t for the current */
  /*      * thread if one wasn't explicitly created */
  /*      *\/ */
  /*     self = pthread_self (); */
  /*     if (self.p == NULL) */
  /*       { */
  /*         return ENOENT; */
  /*       } */
  /*   } */
  /* else */
  /*   { */
  /*     /\* */
  /*      * Resolve catch-22 of registering thread with selfThread */
  /*      * key */
  /*      *\/ */
  /*     pte_thread_t * sp = (pte_thread_t *) pthread_getspecific (pte_selfThreadKey); */

  /*     if (sp == NULL) */
  /*       { */
  /*         if (value == NULL) */
  /*           { */
  /*             return ENOENT; */
  /*           } */

  /*         self = *((pthread_t *) value); */
  /*       } */
  /*     else */
  /*       { */
  /*         self = sp->ptHandle; */
  /*       } */
  /*   } */

  /* result = 0; */

  /* if (key != NULL) */
  /*   { */
  /*     if (self.p != NULL && key->destructor != NULL && value != NULL) */
  /*       { */
  /*         /\* */
  /*          * Only require associations if we have to */
  /*          * call user destroy routine. */
  /*          * Don't need to locate an existing association */
  /*          * when setting data to NULL since the */
  /*          * data is stored with the operating system; not */
  /*          * on the association; setting assoc to NULL short */
  /*          * circuits the search. */
  /*          *\/ */
  /*         ThreadKeyAssoc *assoc; */

  /*         if (pthread_mutex_lock(&(key->keyLock)) == 0) */
  /*           { */
  /*             pte_thread_t * sp = (pte_thread_t *) self.p; */

  /*             (void) pthread_mutex_lock(&(sp->threadLock)); */

  /*             assoc = (ThreadKeyAssoc *) sp->keys; */
  /*             /\* */
  /*              * Locate existing association */
  /*              *\/ */
  /*             while (assoc != NULL) */
  /*               { */
  /*                 if (assoc->key == key) */
  /*                   { */
  /*                     /\* */
  /*                      * Association already exists */
  /*                      *\/ */
  /*                     break; */
  /*                   } */
  /*                 assoc = assoc->nextKey; */
  /*               } */

  /*             /\* */
  /*              * create an association if not found */
  /*              *\/ */
  /*             if (assoc == NULL) */
  /*               { */
  /*                 result = pte_tkAssocCreate (sp, key); */
  /*               } */

  /*             (void) pthread_mutex_unlock(&(sp->threadLock)); */
  /*           } */
  /*         (void) pthread_mutex_unlock(&(key->keyLock)); */
  /*       } */

  /*     if (result == 0) */
  /*       { */
  /*         if (pte_osTlsSetValue (key->key, (void *) value) != PTE_OS_OK) */
  /*           { */
  /*             result = EAGAIN; */
  /*           } */

  /*       } */
  /*   } */

  /* return (result); */
}				/* pthread_setspecific */
