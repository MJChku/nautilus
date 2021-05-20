#ifndef __MUTEX_H__
#define __MUTEX_H__

#ifdef __cplusplus
extern "C" {
#endif
#include <nautilus/naut_types.h>
#include <nautilus/atomic.h>
#include <nautilus/instrument.h>
#include <nautilus/waitqueue.h>
#include <nautilus/futex.h>

#define MUTEX_INITIALIZER 0
#define cmpxchg(ptr,o,n) __sync_val_compare_and_swap(ptr, o, n)

typedef uint32_t mutex;

#define __MEM_ORDER  __ATOMIC_SEQ_CST
#define barrier() __asm__ volatile("": : :"memory")
#define memory_barrier() __sync_synchronize()
void 
mutex_init (volatile mutex * atom);

void
mutex_deinit (volatile mutex * atom);

static inline void
mutex_lock (volatile mutex * atom) 
{
    NK_PROFILE_ENTRY();
    
    volatile int c = cmpxchg(atom, 0, 1);
    // If the lock was previously unlocked, there's nothing else for us to do.
    // Otherwise, we'll probably have to wait.
    barrier();
    if ( *((volatile int*)&c) != 0) {
      do {
        // If the mutex is locked, we signal that we're waiting by setting the
        // atom to 2. A shortcut checks is it's 2 already and avoids the atomic
        // operation in this case.
	barrier();
        if ( *((volatile int*)&c) == 2 || cmpxchg(atom, 1, 2) != 0) {
          // Here we have to actually sleep, because the mutex is actually
          // locked. Note that it's not necessary to loop around this syscall;
          // a spurious wakeup will do no harm since we only exit the do...while
          // loop when atom_ is indeed 0.
	 
	  nk_vc_printf("ERROR: waiters more than 2\n");	
          memory_barrier();
          sys_futex(atom, FUTEX_WAIT, 2, 0, 0, 0);
        }
        // We're here when either:
        // (a) the mutex was in fact unlocked (by an intervening thread).
        // (b) we slept waiting for the atom and were awoken.
        //
        // So we try to lock the atom again. We set teh state to 2 because we
        // can't be certain there's no other thread at this exact point. So we
        // prefer to err on the safe side.
	barrier();
      } while (( *((volatile int*)&c)  = cmpxchg(atom, 0, 2)) != 0);


    }

    NK_PROFILE_EXIT();
}

// returns zero on successful lock acquisition, -1 otherwise
static inline int
mutex_trylock(volatile mutex * atom)
{
    return  cmpxchg(atom, 0, 1) ? -1: 0;
}

static inline void 
mutex_unlock (volatile  mutex * atom) 
{

    NK_PROFILE_ENTRY();
    if (__sync_fetch_and_sub(atom, 1, __MEM_ORDER) != 1) {
      barrier();	
      __atomic_store_n(atom, 0, __MEM_ORDER);
      
      memory_barrier();
      sys_futex((int*)atom, FUTEX_WAKE, 1, 0, 0, 0);
    }
    
    NK_PROFILE_EXIT();
}

// this expects the struct, not the pointer to it
#define NK_MUTEX_LOCK_T         mutex
#define NK_MUTEX_LOCK_INIT(l)   mutex_init(l)
#define NK_MUTEX_LOCK(l)        mutex_lock(l)
#define NK_MUTEX_TRY_LOCK(l)    mutex_trylock(l)
#define NK_MUTEX_UNLOCK(l)      mutex_unlock(l)
#define NK_MUTEX_LOCK_DEINIT(l) mutex_deinit(l)


#ifdef __cplusplus
}
#endif

#endif
