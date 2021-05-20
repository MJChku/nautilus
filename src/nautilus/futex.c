#include <nautilus/futex.h>

#define SYSCALL_NAME "futex"
#define ERROR(fmt, args...) ERROR_PRINT(SYSCALL_NAME ": " fmt, ##args)
#define WARN(fmt, args...) WARN_PRINT(SYSCALL_NAME ": " fmt, ##args)
#define DEBUG(fmt, args...) DEBUG_PRINT(SYSCALL_NAME ": " fmt, ##args)
#define INFO(fmt, args...) INFO_PRINT(SYSCALL_NAME ": " fmt, ##args)
/*
#undef DEBUG
#define DEBUG 
*/
uint64_t sys_futex(uint32_t* uaddr, int op, uint32_t val,
                   /*(struct timespec*)*/ void* utime, uint32_t* uaddr2,
                   uint32_t val3) {
  // DEBUG("Called with args:\n0: %p\n1: %d\n2: %d\n3: %p\n4: %p\n5: %d\n",
  //             uaddr, op, val, utime, uaddr2, val3);

  struct futex* f;

  futex_init();
  NK_LOCK(&futex_lock);
/*
  if (!(op & FUTEX_PRIVATE_FLAG)) {
    // This may break things ???
    // But we continue anyway just in case it's ok
    DEBUG("Non-private futex.\n");
  }

  op &= ~FUTEX_PRIVATE_FLAG;
*/
  switch (op) {
  case FUTEX_WAIT:
    if (utime) {
      DEBUG("timeout unsupported\n");
      NK_UNLOCK(&futex_lock);
      return -1;
    }
    f = futex_find(uaddr);
    if (!f) {

      DEBUG(" allocate futex uaddr=%p\n", uaddr);
      f = futex_allocate(uaddr);
    }
    if (!f) {
      DEBUG("cannot find or allocate futex\n");
      NK_UNLOCK(&futex_lock);
      return -1;
    }

    DEBUG("Starting futex wait on %p %p %d %d\n", f, f->uaddr, *f->uaddr, val);
    f->val = val;
    NK_UNLOCK(&futex_lock);
    nk_wait_queue_sleep_extended(f->waitq, futex_check, f);
    DEBUG("Finished futex wait on %p %p %d %d\n", f, f->uaddr, *f->uaddr, val);

    return 0;

    break;

  case FUTEX_WAKE: {
    uint64_t awoken_threads = 0;
    f = futex_find(uaddr);
    if (!f) {
      NK_UNLOCK(&futex_lock);
      DEBUG("Futex for uaddr=%p does not exist\n", uaddr);
      return 0; // no one to wake - probably race with a FUTEX_WAIT
    }

    DEBUG("Starting futex wake on %p %p %d (waking %d%s)\n", f, f->uaddr,
          *f->uaddr, val, val == INT_MAX ? " ALL" : "");

    if (val != INT_MAX) {
      int i;
      for (i = 0; i < val; i++) {
        awoken_threads += nk_wait_queue_wake_one(f->waitq);
      }
    } else {
      awoken_threads += nk_wait_queue_wake_all(f->waitq);
    }
    NK_UNLOCK(&futex_lock);
    return awoken_threads;
    break;
  }
  default:
    DEBUG("Unsupported FUTEX OP\n");
    NK_UNLOCK(&futex_lock);
    return -1;
  }
}

