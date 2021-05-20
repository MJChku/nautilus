
#include <nautilus/mutex.h>
int 
mutex_init (volatile mutex * atom){
  __atomic_store_n(atom, 0, __MEM_ORDER);
  
//    nk_vc_printf("mutex addr %p\n", atom);
  sys_futex((int*)atom, FUTEX_WAIT, INT_MAX, 0, 0, 0);
  
  //futex_init();
  //void* p = (void*) futex_allocate(atom);
/*   struct futex* p= ( struct futex*) futex_find(atom);
  if(!p){
    nk_vc_printf("mutex init error");
    return 1;
  }
*/
  return  0;
}

int
mutex_deinit (volatile mutex * atom){
  __atomic_store_n(atom, 0, __MEM_ORDER);
  sys_futex((int*)atom, FUTEX_WAKE, INT_MAX, 0, 0, 0);
  return 0;
}
