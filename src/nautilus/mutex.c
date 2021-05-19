
#include <nautilus/mutex.h>
void 
mutex_init (volatile mutex * atom){
  __atomic_store_n(atom, 0, __MEM_ORDER);
  sys_futex((int*)atom, FUTEX_WAIT, INT_MAX, 0, 0, 0);
}

void
mutex_deinit (volatile mutex * atom){
  __atomic_store_n(atom, 0, __MEM_ORDER);
  sys_futex((int*)atom, FUTEX_WAKE, INT_MAX, 0, 0, 0);
}
