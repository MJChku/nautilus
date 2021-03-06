/*
 * pthread_cond_destroy.c
 *
 */

/* #include <stdlib.h> */
#include <nautilus/nautilus.h>
#include "pthread.h"
#include "implement.h"
#include "pthread_cond_common.h"

int
pthread_cond_destroy (pthread_cond_t * c)
{
    DEBUG("Destroying condvar (%p)\n", (void*)c);

    NK_LOCK(&c->lock);
    if (c->nwaiters != 0) {
        NK_UNLOCK(&c->lock);
        return EINVAL;
    }

    ssem_destroy(c->sem);
    NK_UNLOCK(&c->lock);
    memset(c, 0, sizeof(pthread_cond_t));
    return 0;

}
