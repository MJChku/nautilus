/*
 * create.c
*/

#include "nk/pte_osal.h"
#include "pthread.h"
#include "implement.h"
#include <nautilus/nautilus.h>
#include "astore.h"

static int COUNT=0;
int
pthread_create (pthread_t * ptid,
                const pthread_attr_t * attr,
                void *(*start) (void *), void *arg)
/*
 * RESULTS
 *              0               successfully created thread,
 *              EINVAL          attr invalid,
 *              EAGAIN          insufficient resources.
 *
 * ------------------------------------------------------
 */
{

  DEBUG("thread create and start\n");	
  NK_PROFILE_ENTRY();

  pthread_attr_t a;
  
  int result = EAGAIN, detach_state=0, priority=0;
  long stack_size;
  thread_parms *parms = NULL; 

  pte_osResult osResult;
  
  if (attr != NULL)
    {
      a = *attr;
    }
  else
    {
      a = NULL;
    }
if ((parms = (thread_parms *) malloc (sizeof (*parms))) == NULL)
    {
      goto FAIL0;
    }

  parms->tid = ptid;
  parms->start = start;
  parms->arg = arg;


  if (a != NULL)
    {
      stack_size = a->stacksize;
      detach_state = a->detachstate;
      priority = a->param.sched_priority;
      if (PTHREAD_INHERIT_SCHED == a->inheritsched){
         //complicated logic, not used for now
	 //it should be to retrieve from parent thread
	 //it needs to query the sched state to get priority
	 // the set priority of this thread
      }

    }
  else
    {
      stack_size = PTHREAD_STACK_MIN;
    }
  

  struct sys_info *sys = per_cpu_get(system);
  int cpu_num = (++COUNT)% sys->num_cpus;
 
   
  int ret = nk_thread_start((void*)(&pte_thread_start), parms, NULL, detach_state, (nk_stack_size_t)stack_size, ptid, cpu_num);
  
  if(a == NULL){
   
    ERROR("pthread attr is NULL hence give up\n");
    result = 1;
    goto FAIL0;
  }
  if(a!=NULL){

    // debug_print(a);	  
     int ans = attr_store(*ptid, a);
/*
     pthread_attr_t _tmp = attr_retrieve(*ptid);

     if(!_tmp ) return  1;
     debug_print(_tmp);
*/
    if(ans!=0){
       ERROR("thread store attr error");
       result = 1;
       goto FAIL0;
    }
  }

  if (ret != 0){
    ERROR("create error exit\n");
    osResult = PTE_OS_NO_RESOURCES;
    result = EAGAIN;
     goto FAIL0;

  }else{
    osResult = PTE_OS_OK;
  }

  /*osResult = pte_osThreadCreate((void *)&pte_threadStart,
                                stackSize,
                                priority,
                                parms,
                                &(tp->threadId));*/
  /*
   * ------------
   * Failure Code
   * ------------
   */
  NK_PROFILE_EXIT();

  return osResult;
FAIL0:
  NK_PROFILE_EXIT();

  return (result);

}				/* pthread_create */
