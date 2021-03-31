#include <nautilus/nautilus.h>
#include <nautilus/shell.h>



static int handle_set_omp_num_threads(char* buf, void* priv);

static struct shell_cmd_impl omp_num_threads_impl = {
    .cmd      = "set-omp-num-threads",
    .help_str = "set omp number of threads",
    .handler  = handle_set_omp_num_threads,
};
nk_register_shell_cmd(omp_num_threads_impl);


static int handle_set_omp_num_threads(char* buf, void* priv){

    long nthreads;
    if((sscanf(buf, "set-omp-num-threads %d", &nthreads) !=1)) {
      nk_vc_printf("Don't understand %s please input single number\n", buf);
      return -1;
    }
    omp_set_num_threads(nthreads);
    return 0;
}

