#ifndef __FUTEX_H_
#define __FUTEX_H_

#include <nautilus/nautilus.h>
#include <nautilus/waitqueue.h>
#include <nautilus/linux_futex.h>
#define NUM_FUTEXES 512

#define FUTEX_WAIT 0
#define FUTEX_WAKE 1

#define FUTEX_PRIVATE_FLAG 128


struct futex {
  int* uaddr; // search key; == 0 implies not in use
  int val;    // for wait check
  nk_wait_queue_t* waitq;
};

static int futex_inited = 0;
static spinlock_t futex_lock;

static struct futex futex_pool[NUM_FUTEXES];

static int futex_init() {
  if (!futex_inited) {
    int i;
    char buf[80];
    spinlock_init(&futex_lock);
    for (i = 0; i < NUM_FUTEXES; i++) {
      futex_pool[i].uaddr = 0;
      futex_pool[i].val = 0;
      sprintf(buf, "futex%d-waitq", i);
      futex_pool[i].waitq = nk_wait_queue_create(buf);
      if (!futex_pool[i].waitq) {
        // ERROR
        return -1;
      }
    }
    futex_inited = 1;
  }
  return 0;
}

static struct futex* futex_find(int* uaddr) {
  int i;
  for (i = 0; i < NUM_FUTEXES; i++) {
    if (futex_pool[i].uaddr == uaddr) {
      return &futex_pool[i];
    }
  }
  return 0;
}

static struct futex* futex_allocate(int* uaddr) {
  int i;
  for (i = 0; i < NUM_FUTEXES; i++) {
    if (__sync_bool_compare_and_swap(&futex_pool[i].uaddr, 0, uaddr)) {
      return &futex_pool[i];
    }
  }
  return 0;
}

static void futex_free(int* uaddr) {
  struct futex* f = futex_find(uaddr);
  if (f) {
    __sync_fetch_and_and(&f->uaddr, 0);
  }
}

static int futex_check(void* state) {
  struct futex* f = (struct futex*)state;

  return *(f->uaddr) != f->val;
}


uint64_t sys_futex(uint32_t* uaddr, int op, uint32_t val,
                   /*(struct timespec*)*/ void* utime, uint32_t* uaddr2,
                   uint32_t val3) ;




#endif
