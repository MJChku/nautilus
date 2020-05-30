#include <nautilus/nautilus.h>
#include <nautilus/shell.h>
#include <nautilus/libccompat.h>
#ifndef NAUT_CONFIG_DEBUG_GPUDEV
#undef DEBUG_PRINT
#define DEBUG_PRINT(fmt, args...) 
#endif

#define ERROR(fmt, args...) ERROR_PRINT("gpudev: " fmt, ##args)
#define DEBUG(fmt, args...) DEBUG_PRINT("gpudev: " fmt, ##args)
#define INFO(fmt, args...) INFO_PRINT("gpudev: " fmt, ##args)
extern int pthread_init(void);

static int handle_gputest (char * buf, void * priv)
{
    int i;
    if (0 != pthread_init()){
     ERROR("pthread initilize failed\n");
    }
    #pragma omp parallel num_threads(5)
    {
      
      /* long id = getpid(); */
      /* nk_vc_printf("****omptest==thread id %d", id);	 */
       #pragma omp for nowait schedule(static,1) 
        for( i=0;i<10;i++){
	 long id = getpid();
         nk_vc_printf("****omptest==thread id %d\n", id);  
         nk_vc_printf("*****working %d\n",i);
      }

    }

}


static struct shell_cmd_impl omptest_impl = {
    .cmd      = "omptest",
    .help_str = "openmp test",
    .handler  = handle_gputest,
};
nk_register_shell_cmd(omptest_impl);

