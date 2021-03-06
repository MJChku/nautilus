/*
 * benchtest1.c
 *
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
 *
 * --------------------------------------------------------------------------
 *
 * Measure time taken to complete an elementary operation.
 *
 * - Mutex
 *   Single thread iteration over lock/unlock for each mutex type.
 */

#include "test.h"
//#include "implement.h"

#ifdef __GNUC__
//#include <stdlib.h>
#endif

#include "benchtest.h"

#define PTW32_MUTEX_TYPES
#define ITERATIONS      100000L

static pthread_mutex_t mx;
static pthread_mutexattr_t ma;
static struct _timeb currSysTimeStart;
static struct _timeb currSysTimeStop;
static long durationMilliSecs;
static long overHeadMilliSecs = 0;
static int one = 1;
static int zero = 0;

#define GetDurationMilliSecs(_TStart, _TStop) ((_TStop.time*1000+_TStop.millitm) \
                                               - (_TStart.time*1000+_TStart.millitm))

/*
 * Dummy use of j, otherwise the loop may be removed by the optimiser
 * when doing the overhead timing with an empty loop.
 */
#define TESTSTART \
  { int i, j = 0, k = 0; _ftime(&currSysTimeStart); for (i = 0; i < ITERATIONS; i++) { j++;

#define TESTSTOP \
  }; _ftime(&currSysTimeStop); if (j + k == i) j++; }


static void
runTest (char * testNameString, int mType)
{
#ifdef PTW32_MUTEX_TYPES
  assert(pthread_mutexattr_settype(&ma, mType) == 0);
#endif
  assert(pthread_mutex_init(&mx, &ma) == 0);

  TESTSTART
  assert(pthread_mutex_lock(&mx) == zero);
  assert(pthread_mutex_unlock(&mx) == zero);
  TESTSTOP

  assert(pthread_mutex_destroy(&mx) == 0);

  durationMilliSecs = GetDurationMilliSecs(currSysTimeStart, currSysTimeStop) - overHeadMilliSecs;

  printf( "%-45s %15ld %15.3f\n",
          testNameString,
          durationMilliSecs,
          (float) durationMilliSecs * 1E3 / ITERATIONS);
}


int pthread_test_bench1()
{
  int i = 0;
  pte_osMutexHandle cs;

  overHeadMilliSecs = 0;
  one = 1;
  zero = 0;

  pthread_mutexattr_init(&ma);

  printf( "=============================================================================\n");
  printf( "\nLock plus unlock on an unlocked mutex.\n%ld iterations\n\n",
          ITERATIONS);
  printf( "%-45s %15s %15s\n",
          "Test",
          "Total(msec)",
          "average(usec)");

  /*
   * Time the loop overhead so we can subtract it from the actual test times.
   */

  TESTSTART
  assert(1 == one);
  assert(1 == one);
  TESTSTOP

  durationMilliSecs = GetDurationMilliSecs(currSysTimeStart, currSysTimeStop) - overHeadMilliSecs;
  overHeadMilliSecs = durationMilliSecs;


  TESTSTART
  assert((dummy_call(&i), 1) == one);
  assert((dummy_call(&i), 1) == one);
  TESTSTOP

  durationMilliSecs = GetDurationMilliSecs(currSysTimeStart, currSysTimeStop) - overHeadMilliSecs;

  printf( "%-45s %15ld %15.3f\n",
          "Dummy call x 2",
          durationMilliSecs,
          (float) durationMilliSecs * 1E3 / ITERATIONS);


  TESTSTART
  assert((interlocked_inc_with_conditionals(&i), 1) == one);
  assert((interlocked_dec_with_conditionals(&i), 1) == one);
  TESTSTOP

  durationMilliSecs = GetDurationMilliSecs(currSysTimeStart, currSysTimeStop) - overHeadMilliSecs;

  printf( "%-45s %15ld %15.3f\n",
          "Dummy call -> Interlocked with cond x 2",
          durationMilliSecs,
          (float) durationMilliSecs * 1E3 / ITERATIONS);

  TESTSTART
  assert((PTE_ATOMIC_INCREMENT(&i), 1) == one);
  assert((PTE_ATOMIC_INCREMENT(&i), 1) == one);
  TESTSTOP

  durationMilliSecs = GetDurationMilliSecs(currSysTimeStart, currSysTimeStop) - overHeadMilliSecs;

  printf( "%-45s %15ld %15.3f\n",
          "InterlockedOp x 2",
          durationMilliSecs,
          (float) durationMilliSecs * 1E3 / ITERATIONS);


  pte_osMutexCreate(&cs);

  TESTSTART
  {

    pte_osMutexLock(cs);
    pte_osMutexUnlock(cs);
  }
  TESTSTOP

  pte_osMutexDelete(cs);


  durationMilliSecs = GetDurationMilliSecs(currSysTimeStart, currSysTimeStop) - overHeadMilliSecs;

  printf( "%-45s %15ld %15.3f\n",
          "Simple Critical Section",
          durationMilliSecs,
          (float) durationMilliSecs * 1E3 / ITERATIONS);



  printf( ".............................................................................\n");

  /*
   * Now we can start the actual tests
   */
#ifdef PTW32_MUTEX_TYPES
  runTest("PTHREAD_MUTEX_DEFAULT", PTHREAD_MUTEX_DEFAULT);

  runTest("PTHREAD_MUTEX_NORMAL", PTHREAD_MUTEX_NORMAL);

  runTest("PTHREAD_MUTEX_ERRORCHECK", PTHREAD_MUTEX_ERRORCHECK);

  runTest("PTHREAD_MUTEX_RECURSIVE", PTHREAD_MUTEX_RECURSIVE);
#else
  runTest("Non-blocking lock", 0);
#endif

  printf( "=============================================================================\n");

  /*
   * End of tests.
   */

  pthread_mutexattr_destroy(&ma);

  one = i; /* Dummy assignment to avoid 'variable unused' warning */
  return 0;
}
