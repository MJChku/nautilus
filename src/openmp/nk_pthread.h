/* Copyright (C) 2002-2018 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

#ifndef _PTHREAD_H_
#define _PTHREAD_H_	

#include <nautilus/thread.h>
#include <nautilus/spinlock.h>
//mjc
struct __empty{
  int a;
};

#define pthread_t nk_thread_id_t
#define pthread_condattr_t unsigned
#define pthread_cond_t unsigned
#define pthread_mutex_t spinlock_t
#define pthread_mutexattr_t unsigned
#define pthread_attr_t unsigned






extern int pthread_mutex_init (pthread_mutex_t *__mutex,
			       const pthread_mutexattr_t *__mutexattr); // __THROW __nonnull ((1));

//extern int pthread_mutex_destroy (pthread_mutex_t *__mutex); //__THROW __nonnull ((1));
//extern int pthread_mutex_trylock (pthread_mutex_t *__mutex); //__THROWNL __nonnull ((1));
//extern int pthread_mutex_lock (pthread_mutex_t *__mutex); //__THROWNL __nonnull ((1));

//extern int pthread_mutex_unlock (pthread_mutex_t *__mutex); //__THROWNL __nonnull ((1));

extern int pthread_mutexattr_init (pthread_mutexattr_t *__attr); // __THROW __nonnull ((1));
//extern int pthread_mutexattr_destroy (pthread_mutexattr_t *__attr); // __THROW __nonnull ((1));

extern int pthread_attr_init (pthread_attr_t *__attr); //__THROW __nonnull ((1));



/* Detach state.  */
/* enum */
/* { */
/*   PTHREAD_CREATE_JOINABLE, */
/* #define PTHREAD_CREATE_JOINABLE	PTHREAD_CREATE_JOINABLE */
/*   PTHREAD_CREATE_DETACHED */
/* #define PTHREAD_CREATE_DETACHED	PTHREAD_CREATE_DETACHED */
/* }; */


/* /\* Mutex types.  *\/ */
/* enum */
/* { */
/*   PTHREAD_MUTEX_TIMED_NP, */
/*   PTHREAD_MUTEX_RECURSIVE_NP, */
/*   PTHREAD_MUTEX_ERRORCHECK_NP, */
/*   PTHREAD_MUTEX_ADAPTIVE_NP, */
/* #if defined __USE_UNIX98 || defined __USE_XOPEN2K8 */
/*   PTHREAD_MUTEX_NORMAL = PTHREAD_MUTEX_TIMED_NP, */
/*   PTHREAD_MUTEX_RECURSIVE = PTHREAD_MUTEX_RECURSIVE_NP, */
/*   PTHREAD_MUTEX_ERRORCHECK = PTHREAD_MUTEX_ERRORCHECK_NP, */
/*   PTHREAD_MUTEX_DEFAULT = PTHREAD_MUTEX_NORMAL */
/* #endif */
/* } */

//extern int pthread_create (pthread_t *__restrict __newthread,
//			   const pthread_attr_t *__restrict __attr,
//			   void *(*__start_routine) (void *),
//			   void *__restrict __arg); // __THROWNL __nonnull ((1, 3));

//extern void pthread_exit (void *__retval); //__attribute__ ((__noreturn__));
//extern int pthread_join (pthread_t __th, void **__thread_return);
//extern int pthread_detach (pthread_t __th); // __THROW;

//extern pthread_t pthread_self (void); //__THROW __attribute__ ((__const__));
//extern int pthread_equal (pthread_t __thread1, pthread_t __thread2); //__THROW __attribute__ ((__const__));


//extern int pthread_attr_destroy (pthread_attr_t *__attr);  //__THROW __nonnull ((1));

/* extern int pthread_attr_getdetachstate (const pthread_attr_t *__attr, */
/* 					int *__detachstate) */
/*      __THROW __nonnull ((1, 2)); */
/* extern int pthread_attr_setdetachstate (pthread_attr_t *__attr, */
/* 					int __detachstate) */
/*      __THROW __nonnull ((1)); */

/* extern int pthread_attr_getguardsize (const pthread_attr_t *__attr, */
/* 				      size_t *__guardsize) */
/*      __THROW __nonnull ((1, 2)); */
/* extern int pthread_attr_setguardsize (pthread_attr_t *__attr, */
/* 				      size_t __guardsize) */
/*      __THROW __nonnull ((1)); */

/* extern int pthread_attr_getschedparam (const pthread_attr_t *__restrict __attr, */
/* 				       struct sched_param *__restrict __param) */
/*      __THROW __nonnull ((1, 2)); */
/* extern int pthread_attr_setschedparam (pthread_attr_t *__restrict __attr, */
/* 				       const struct sched_param *__restrict */
/* 				       __param) __THROW __nonnull ((1, 2)); */
/* extern int pthread_attr_getschedpolicy (const pthread_attr_t *__restrict */
/* 					__attr, int *__restrict __policy) */
/*      __THROW __nonnull ((1, 2)); */
/* extern int pthread_attr_setschedpolicy (pthread_attr_t *__attr, int __policy) */
/*      __THROW __nonnull ((1)); */

/* extern int pthread_attr_getinheritsched (const pthread_attr_t *__restrict */
/* 					 __attr, int *__restrict __inherit) */
/*      __THROW __nonnull ((1, 2)); */
/* extern int pthread_attr_setinheritsched (pthread_attr_t *__attr, */
/* 					 int __inherit) */
/*      __THROW __nonnull ((1)); */
/* extern int pthread_attr_getscope (const pthread_attr_t *__restrict __attr, */
/* 				  int *__restrict __scope) */
/*      __THROW __nonnull ((1, 2)); */
/* extern int pthread_attr_setscope (pthread_attr_t *__attr, int __scope) */
/*      __THROW __nonnull ((1)); */
/* extern int pthread_attr_getstackaddr (const pthread_attr_t *__restrict */
/* 				      __attr, void **__restrict __stackaddr) */
/*      __THROW __nonnull ((1, 2)) __attribute_deprecated__; */
/* extern int pthread_attr_setstackaddr (pthread_attr_t *__attr, */
/* 				      void *__stackaddr) */
/*      __THROW __nonnull ((1)) __attribute_deprecated__; */
/* extern int pthread_attr_getstacksize (const pthread_attr_t *__restrict */
/* 				      __attr, size_t *__restrict __stacksize) */
/*      __THROW __nonnull ((1, 2)); */
/* extern int pthread_attr_setstacksize (pthread_attr_t *__attr, */
/* 				      size_t __stacksize) */
/*      __THROW __nonnull ((1)); */


#endif	/* pthread.h */
