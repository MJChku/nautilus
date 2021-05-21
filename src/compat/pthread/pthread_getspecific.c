
/*
 * pthread_getspecific.c
 *
 */


#include "pthread.h"
#include "implement.h"


void *
pthread_getspecific (pthread_key_t key)
/*
 * ------------------------------------------------------
 * DOCPUBLIC
 *      This function returns the current value of key in the
 *      calling thread. If no value has been set for 'key' in
 *      the thread, NULL is returned.
 *
 * PARAMETERS
 *      key
 *              an instance of pthread_key_t
 *
 *
 * DESCRIPTION
 *      This function returns the current value of key in the
 *      calling thread. If no value has been set for 'key' in
 *      the thread, NULL is returned.
 *
 * RESULTS
 *              key value or NULL on failure
 *
 * ------------------------------------------------------
 */
{
//  BOGUS();

  return nk_tls_get(key);	
  /* void * ptr; */

  /* if (key == NULL) */
  /*   { */
  /*     ptr = NULL; */
  /*   } */
  /* else */
  /*   { */
  /*     ptr = pte_osTlsGetValue (key->key); */
  /*   } */

  /* return ptr; */
}
