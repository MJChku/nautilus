/*
 * pthread_mutex_init.c
 *
 * Description:
 * This translation unit implements mutual exclusion (mutex) primitives.
 *
 * --------------------------------------------------------------------------
 *
 *      Pthreads-embedded (PTE) - POSIX Threads Library for embedded systems
 *      Copyright(C) 2008 Jason Schmidlapp
 *
 *      Contact Email: jschmidlapp@users.sourceforge.net
 *
 *
 *      Based upon Pthreads-win32 - POSIX Threads Library for Win32
 *      Copyright(C) 1998 John E. Bossom
 *      Copyright(C) 1999,2005 Pthreads-win32 contributors
 *
 *      Contact Email: rpj@callisto.canberra.edu.au
 *
 *      The original list of contributors to the Pthreads-win32 project
 *      is contained in the file CONTRIBUTORS.ptw32 included with the
 *      source code distribution. The list can also be seen at the
 *      following World Wide Web location:
 *      http://sources.redhat.com/pthreads-win32/contributors.html
 *
 *      This library is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU Lesser General Public
 *      License as published by the Free Software Foundation; either
 *      version 2 of the License, or (at your option) any later version.
 *
 *      This library is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *      Lesser General Public License for more details.
 *
 *      You should have received a copy of the GNU Lesser General Public
 *      License along with this library in the file COPYING.LIB;
 *      if not, write to the Free Software Foundation, Inc.,
 *      59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

/* #include <stdio.h> */
/* #include <stdlib.h> */
#include <nautilus/nautilus.h>
#include "pthread.h"
#include "implement.h"
#include "debug.h"

int
pthread_mutex_init (pthread_mutex_t * mutex, const pthread_mutexattr_t * attr)
{
  NK_PROFILE_ENTRY();
  int result = 0;
  pthread_mutex_t mx;


  if (mutex == NULL)
    {
      return EINVAL;
    }


  mx = (pthread_mutex_t) calloc (1, sizeof (*mx));
  memset(mx, 0, sizeof(*mx));

  if (mx == NULL)
    {
      result = ENOMEM;
    }
  else
    {

      ERROR("initialize pthread_mutex addr %p\n", mx);
      NK_MUTEX_LOCK_INIT(mx);
/*
      mx->lock_idx = 0;
      mx->recursive_count = 0;
      // ERROR("attr addr %08x\n", attr);
      // ERROR("attr *addr %08x\n", *attr);

      //this is mx->kind assignment is new
      //mx->kind = PTHREAD_MUTEX_DEFAULT;
      // below is original assignment, it fails
      mx->kind = ((attr == NULL ) ? PTHREAD_MUTEX_DEFAULT : attr->kind);

      //DEBUG("pass mxkind\n");
      mx->ownerThread = NULL;
      mx->sem = malloc(sizeof(simple_sem_t));
      mx->sem = nk_semaphore_create(NULL, 0,0, NULL);
      //ssem_init(mx->sem,  0);
     //pte_osSemaphoreCreate(0,&mx->handle);
*/
    }

  *mutex = mx;
   NK_PROFILE_EXIT();
  return (result);
}

