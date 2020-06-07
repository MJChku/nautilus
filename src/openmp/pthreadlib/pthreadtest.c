#include <nautilus/nautilus.h>
#include <nautilus/shell.h>
#include <nautilus/libccompat.h>
#include "implement.h"
#ifndef NAUT_CONFIG_DEBUG_GPUDEV
#undef DEBUG_PRINT
#define DEBUG_PRINT(fmt, args...) 
#endif

#define ERROR(fmt, args...) ERROR_PRINT("omptest: " fmt, ##args)
#define DEBUG(fmt, args...) DEBUG_PRINT("omptest: " fmt, ##args)
#define INFO(fmt, args...) INFO_PRINT("omptest: " fmt, ##args)





#define MAXN 100  /* Max value of N */
int N;  /* Matrix size */
int procs;  /* Number of processors to use */

/* Matrices and vectors */
volatile float A[MAXN][MAXN], B[MAXN], X[MAXN];
/* A * X = B, solve for X */


/* Prototype */
void gauss();  /* The function you will provide.
		* It is this routine that is timed.
		* It is called only on the parent.
		*/

/* Initialize A and B (and X to 0.0s) */
void initialize_inputs() {
  int row, col;

  printf("\nInitializing...\n");
  for (col = 0; col < N; col++) {
    for (row = 0; row < N; row++) {
      A[row][col] = (float) (col+row) / 32768.0;
    }
    B[col] = (float)(col)/ 32768.0;
    X[col] = 0.0;
  }

}

/* Print input matrices */
void print_inputs() {
  int row, col;

  if (N < 10) {
    printf("\nA =\n\t");
    for (row = 0; row < N; row++) {
      for (col = 0; col < N; col++) {
	printf("%5.2f%s", A[row][col], (col < N-1) ? ", " : ";\n\t");
      }
    }
    printf("\nB = [");
    for (col = 0; col < N; col++) {
      printf("%5.2f%s", B[col], (col < N-1) ? "; " : "]\n");
    }
  }
}



int doneflag[MAXN] = {0};
pthread_mutex_t lock;
void *cal_zero(void* threadid);


/* Initialize A and B (and X to 0.0s) */
void initialize_inputs() {
  int row, col;

  printf("\nInitializing...\n");
  for (col = 0; col < N; col++) {
    for (row = 0; row < N; row++) {
      A[row][col] = (float)rand() / 32768.0;
    }
    B[col] = (float)rand() / 32768.0;
    X[col] = 0.0;
  }

}

/* Print input matrices */
void print_inputs() {
  int row, col;

  if (N < 10) {
    printf("\nA =\n\t");
    for (row = 0; row < N; row++) {
      for (col = 0; col < N; col++) {
	printf("%5.2f%s", A[row][col], (col < N-1) ? ", " : ";\n\t");
      }
    }
    printf("\nB = [");
    for (col = 0; col < N; col++) {
      printf("%5.2f%s", B[col], (col < N-1) ? "; " : "]\n");
    }
  }
}

void gauss() {
  int norm, row, col;  /* Normalization row, and zeroing
			* element row and col */
  float multiplier;
  int numthreads = N-1;
  doneflag[0] = 1;
  pthread_t threads[procs];
  printf("Computing Using Pthread\n");
  pthread_mutex_init(&lock,NULL);;
  // sem_init(&mutex,0,1);
  // sem_init(&barrier,0,0);
  //pthread_barrier_init(&barrier, NULL, numthreads);
  for (int i=0; i < procs; i++)
  {
     pthread_create(&threads[i],NULL,&cal_zero, (void *)i);
  }

  for(int i=0; i < procs; i++)
  {
     pthread_join(threads[i], NULL); 
  }
  /* Back substitution */
  for (row = N - 1; row >= 0; row--) {
    X[row] = B[row];
    for (col = N-1; col > row; col--) {
      X[row] -= A[row][col] * X[col];
    }
    X[row] /= A[row][row];
  }
}


void *cal_zero(void* threadid) {
  long id = (long) threadid;
  float multiplier;
  for (long norm = 0; norm < N-1 ; norm++){
    
    //busy wait with relinquish cpu
    while(1){
       pthread_mutex_lock(&lock);
     
       if( doneflag[norm] == 1){
        pthread_mutex_unlock(&lock);
        break;
       }else{
         pthread_mutex_unlock(&lock);
         pthread_yield();
       }
    }
    for(int row = id; row < N; row+=procs){
        if(row <= norm)
            continue;
        multiplier = A[row][norm] / A[norm][ norm];
        for (int col = norm; col < N; col++) {
          A[row][col] -= A[norm][col] * multiplier;
        }
        B[row] -= B[norm] * multiplier;
        
        if(row == norm+1 ){
          //critical region
          pthread_mutex_lock(&lock);
          // for(int k=0;k<)
          //     printf("%d", doneflag);
              doneflag[norm+1] = 1;
          pthread_mutex_unlock(&lock);
        }
    }
  }
}



static int handle_omptest (char * buf, void * priv)
{
    

}


static struct shell_cmd_impl omptest_impl = {
    .cmd      = "omptest",
    .help_str = "openmp test",
    .handler  = handle_omptest,
};
nk_register_shell_cmd(omptest_impl);

