/* 
 * This file is part of the Nautilus AeroKernel developed
 * by the Hobbes and V3VEE Projects with funding from the 
 * United States National  Science Foundation and the Department of Energy.  
 *
 * The V3VEE Project is a joint project between Northwestern University
 * and the University of New Mexico.  The Hobbes Project is a collaboration
 * led by Sandia National Laboratories that includes several national 
 * laboratories and universities. You can find out more at:
 * http://www.v3vee.org  and
 * http://xstack.sandia.gov/hobbes
 *
 * Copyright (c) 2017, Peter Dinda <pdinda@northwestern.edu>
 * Copyright (c) 2017, The V3VEE Project  <http://www.v3vee.org> 
 *                     The Hobbes Project <http://xstack.sandia.gov/hobbes>
 * All rights reserved.
 *
 * Author: Peter Dinda <pdinda@northwestern.edu>
 *
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "LICENSE.txt".
 */

#include <nautilus/nautilus.h>
#include <nautilus/shell.h>
#include <nautilus/task.h>
#include <nautilus/scheduler.h>

#define DO_PRINT       0

#define DO_ARGS        0

#if DO_PRINT
#define PRINT(...) nk_vc_printf(__VA_ARGS__)
#else
#define PRINT(...) 
#endif

#define NUM_PASSES 10
#define NUM_TASKS  65536
#define DEPTH      15     // should be 1 less than log NUM_TASKS

static struct nk_task *tasks[NUM_TASKS];

struct test_arg {
    int pass;
    int task;
};

static void *task_func(void *in)
{
#if DO_ARGS    
    struct test_arg *arg = (struct test_arg *)in;

    PRINT("Hello from task %lu pass %d\n", arg->task, arg->pass);

    void *ret = (void*)(uint64_t)(arg->task);
    
    free(arg);
    
    return ret;
#else
    return 0;
#endif
}

#define MAX(x,y) ((x)>(y) ? (x) : (y))
#define MIN(x,y) ((x)<(y) ? (x) : (y))

static int test_create_wait(int nump, int numt)
{
    int i,j;
    struct nk_task_stats stats;
    uint64_t    queue_s=0, queue_s2=0, queue_min=-1, queue_max=0;
    uint64_t    total_s=0, total_s2=0, total_min=-1, total_max=0;
    uint64_t    wait_s=0, wait_s2=0, wait_min=-1, wait_max=0;
#ifdef NAUT_CONFIG_TASK_DEEP_STATISTICS
    uint64_t    create_s=0, create_s2=0, create_min=-1, create_max=0;
    uint64_t    enqueue_s=0, enqueue_s2=0, enqueue_min=-1, enqueue_max=0;
    uint64_t    dequeue_s=0, dequeue_s2=0, dequeue_min=-1, dequeue_max=0;
    uint64_t    destroy_s=0, destroy_s2=0, destroy_min=-1, destroy_max=0;
#endif
    
    PRINT("Starting on task create wait stress test (%d passes, %d tasks)\n",nump,numt);
    
    for (i=0;i<nump;i++) {
	PRINT("Starting to launch %d tasks on pass %d\n",numt,i);
	for (j=0;j<numt;j++) {
#if DO_ARGS
	    struct test_arg *arg = (struct test_arg *) malloc(sizeof(struct test_arg));
	    arg->pass=i;
	    arg->task=j;
	    tasks[j] = nk_task_produce(-1,0,task_func,arg,0);
#else 
	    tasks[j] = nk_task_produce(-1,0,task_func,0,0);
#endif
	    if (!tasks[j]) { 
		PRINT("Failed to launch task %d on pass %d\n", j,i);
		return -1;
	    }
	}
	PRINT("Launched %d tasks in pass %d\n", j, i);
	for (j=0;j<numt;j++) {
	    void *result;
	    if (nk_task_wait(tasks[j], &result, &stats)) {
		PRINT("Failed to wait on task %d pass %d\n", j, i);
		return -1;
	    } else {
		PRINT("Completed task %d pass %d result: %p\n", j,i,result);
		PRINT("  size=%lu, enq=%lu, deq=%lu comp=%lu, waits=%lu, waite=%lu (all ns)\n",
		      stats.size_ns, stats.enqueue_time_ns, stats.dequeue_time_ns,
		      stats.complete_time_ns, stats.wait_start_ns, stats.wait_end_ns);
#ifdef NAUT_CONFIG_TASK_DEEP_STATISTICS
		PRINT("  overheads: create=%lu, enqueue=%lu, dequeue=%lu, destroy=%lu (all cycles)\n",
		      stats.create_cost_cycles, stats.enqueue_cost_cycles, stats.dequeue_cost_cycles,
		      stats.destroy_cost_cycles);
#endif
		queue_s += stats.dequeue_time_ns - stats.enqueue_time_ns;
		queue_s2 += (stats.dequeue_time_ns - stats.enqueue_time_ns)*(stats.dequeue_time_ns - stats.enqueue_time_ns);
		queue_max = MAX(queue_max, stats.dequeue_time_ns - stats.enqueue_time_ns);
		queue_min = MIN(queue_min, stats.dequeue_time_ns - stats.enqueue_time_ns);
		total_s += stats.complete_time_ns - stats.enqueue_time_ns;
		total_s2 += (stats.complete_time_ns - stats.enqueue_time_ns)*(stats.complete_time_ns - stats.enqueue_time_ns);
		total_max = MAX(total_max, stats.complete_time_ns - stats.enqueue_time_ns);
		total_min = MIN(total_min, stats.complete_time_ns - stats.enqueue_time_ns);
		wait_s += stats.wait_end_ns - stats.wait_start_ns;
		wait_s2 += (stats.wait_end_ns - stats.wait_start_ns)*(stats.wait_end_ns - stats.wait_start_ns);
		wait_max = MAX(wait_max, stats.wait_end_ns - stats.wait_start_ns);
		wait_min = MIN(wait_min, stats.wait_end_ns - stats.wait_start_ns);
#ifdef NAUT_CONFIG_TASK_DEEP_STATISTICS
		create_s += stats.create_cost_cycles;
		create_s2 += stats.create_cost_cycles * stats.create_cost_cycles;
		create_max = MAX(create_max, stats.create_cost_cycles);
		create_min = MIN(create_min, stats.create_cost_cycles);
		enqueue_s += stats.enqueue_cost_cycles;
		enqueue_s2 += stats.enqueue_cost_cycles * stats.enqueue_cost_cycles;
		enqueue_max = MAX(enqueue_max, stats.enqueue_cost_cycles);
		enqueue_min = MIN(enqueue_min, stats.enqueue_cost_cycles);
		dequeue_s += stats.dequeue_cost_cycles;
		dequeue_s2 += stats.dequeue_cost_cycles * stats.dequeue_cost_cycles;
		dequeue_max = MAX(dequeue_max, stats.dequeue_cost_cycles);
		dequeue_min = MIN(dequeue_min, stats.dequeue_cost_cycles);
		destroy_s += stats.destroy_cost_cycles;
		destroy_s2 += stats.destroy_cost_cycles * stats.destroy_cost_cycles;
		destroy_max = MAX(destroy_max, stats.destroy_cost_cycles);
		destroy_min = MIN(destroy_min, stats.destroy_cost_cycles);
#endif
	    }
	}
	PRINT("Waited on %d tasks in pass %d\n", numt, i);
    }
    PRINT("Done with task create wait stress test (SUCCESS)\n");
    nk_vc_printf("queue: avg=%lu var=%lu min=%lu max=%lu (all ns)\n",
		 queue_s/(numt*nump), queue_s2/(numt*nump) - (queue_s/(numt*nump))*(queue_s/(numt*nump)), queue_min, queue_max);
    nk_vc_printf("total: avg=%lu var=%lu min=%lu max=%lu (all ns)\n",
		 total_s/(numt*nump), total_s2/(numt*nump) - (total_s/(numt*nump))*(total_s/(numt*nump)), total_min, total_max);
    nk_vc_printf("wait:  avg=%lu var=%lu min=%lu max=%lu (all ns)\n",
		 wait_s/(numt*nump), wait_s2/(numt*nump) - (wait_s/(numt*nump))*(wait_s/(numt*nump)), wait_min, wait_max);
#ifdef NAUT_CONFIG_TASK_DEEP_STATISTICS
    nk_vc_printf("create: avg=%lu var=%lu min=%lu max=%lu (all cycles)\n",
		 create_s/(numt*nump), create_s2/(numt*nump) - (create_s/(numt*nump))*(create_s/(numt*nump)), create_min, create_max);
    nk_vc_printf("enqueue: avg=%lu var=%lu min=%lu max=%lu (all cycles)\n",
		 enqueue_s/(numt*nump), enqueue_s2/(numt*nump) - (enqueue_s/(numt*nump))*(enqueue_s/(numt*nump)), enqueue_min, enqueue_max);
    nk_vc_printf("dequeue: avg=%lu var=%lu min=%lu max=%lu (all cycles)\n",
		 dequeue_s/(numt*nump), dequeue_s2/(numt*nump) - (dequeue_s/(numt*nump))*(dequeue_s/(numt*nump)), dequeue_min, dequeue_max);
    nk_vc_printf("destroy: avg=%lu var=%lu min=%lu max=%lu (all cycles)\n",
		 destroy_s/(numt*nump), destroy_s2/(numt*nump) - (destroy_s/(numt*nump))*(destroy_s/(numt*nump)), destroy_min, destroy_max);
#endif
    return 0;
}


static void *_test_recursive_create_wait(void *in)
{
    uint64_t depth = (uint64_t) in;
    struct nk_task *left, *right;

    PRINT("Hello from task at depth %lu\n", depth);

    if (depth==DEPTH) { 
	return 0;
    } else {
	if (!(left=nk_task_produce(-1, 0, _test_recursive_create_wait,(void*)(depth+1),0))) {
	    PRINT("Failed to launch left task at depth %lu\n", depth);
	    return 0;
	}
	if (!(right=nk_task_produce(-1, 0, _test_recursive_create_wait,(void*)(depth+1),0))) {
	    PRINT("Failed to launch right task at depth %lu\n", depth);
	    return 0;
	}
	PRINT("tasks launched\n");
	PRINT("waiting on left\n");
	if (nk_task_wait(left,0,0)) {
	    PRINT("Failed to wait on left task at depth %lu\n", depth);
	    return 0;
	}
	PRINT("left wait complete\n");
	PRINT("waiting on right\n");
	if (nk_task_wait(right,0,0)) {
	    PRINT("Failed to wait on left task at depth %lu\n", depth);
	    return 0;
	}
	PRINT("right wait complete\n");
	
    }
    return 0;
}	    

static int test_recursive_create_wait()
{
    int i;
    for (i=0;i<NUM_PASSES;i++) {
	_test_recursive_create_wait(0);
    }
    return 0;
}




int test_tasks()
{
    int create_wait;
    int recursive_create_wait;

    create_wait = test_create_wait(NUM_PASSES,NUM_TASKS);

    nk_vc_printf("Create-wait test of %lu passes with %lu tasks each: %s\n", 
		 NUM_PASSES,NUM_TASKS, create_wait ? "FAIL" : "PASS");

    recursive_create_wait = test_recursive_create_wait();

    nk_vc_printf("Recursive create-wait test of %lu passes with %lu tasks each: %s\n", 
		 NUM_PASSES,NUM_TASKS, recursive_create_wait ? "FAIL" : "PASS");

    return create_wait | recursive_create_wait;

}


static int
handle_tasks (char * buf, void * priv)
{
    return test_tasks();
}

static struct shell_cmd_impl tasks_impl = {
    .cmd      = "tasktest",
    .help_str = "tasktest",
    .handler  = handle_tasks,
};
nk_register_shell_cmd(tasks_impl);
