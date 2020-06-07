#include <nautilus/nautilus.h>
#include <nautilus/libccompat.h>
#include <nautilus/thread.h>
#include <nautilus/errno.h>
#include <nautilus/random.h>
#include <nautilus/env.h>
//kmp
#include <nautilus/spinlock.h>
//#define __PTHREAD_COND_SHARED_MASK 1
//#define PTHREAD_MUTEX_PSHARED_BIT 128


#define ERROR(fmt, args...) ERROR_PRINT("pthreadhelper: " fmt, ##args)

#define INFO(fmt, args...)   INFO_PRINT("pthreadhelper: " fmt, ##args)
#define DEBUG(fmt, args...)
#ifdef NAUT_CONFIG_OPENMP_RT_DEBUG
#undef DEBUG
#define DEBUG(fmt, args...) DEBUG_PRINT("pthreadhelper: " fmt, ##args)
#endif


int pthread_atfork (void (*prepare) (void), void (*parent) (void),
                  void (*child) (void))
{
  DEBUG("register pthread atfork\n");
  return 0;
}

int sigaction(){
  DEBUG("sigaction\n");
  return 0;
}

int sigemptyset(){
  DEBUG("sigemptyset\n");
  return 0;
}

int atexit(){
  DEBUG("atexit\n");
  // nk_thread_exit();
  return 0;
}
