#include "pthread.h"
#include "implement.h"
#include "astore.h"

#define DEBUG ERROR
static void _debug_print(pthread_attr_t pattr){


DEBUG("valid            = %d \n", pattr->valid);
DEBUG("stackaddr        = %p \n", pattr->stackaddr);
DEBUG("stack size       = %08x  \n", pattr->stacksize);
DEBUG("inheritsched     = %d  \n", pattr->inheritsched);
DEBUG("contention scope = %p \n", pattr->contentionscope);


}
int
pthread_getattr_np (pthread_t thread ,pthread_attr_t * attr)
{ 
 // BOGUS();
  pthread_attr_t pattr;
  do{
  pattr = attr_retrieve(thread);

  }while(!pattr);

/*  ERROR("search thread %p \n", thread);
  if(!pattr){
    ERROR("pattr doesn't exist\n");
    return 0;
  }


  debug_print(pattr);

  ERROR("pritn attr then\n");
  debug_print(*attr);
*/
  
  *attr = pattr;

  //  *attr = thread.attr;  
  return 0;

}
