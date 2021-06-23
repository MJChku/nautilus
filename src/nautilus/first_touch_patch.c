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
 * Copyright (c) 2015, Kyle C. Hale <kh@u.northwestern.edu>
 * Copyright (c) 2015, The V3VEE Project  <http://www.v3vee.org>
 *                     The Hobbes Project <http://xstack.sandia.gov/hobbes>
 * All rights reserved.
 *
 * Author: Kyle C. Hale <kh@u.northwestern.edu>
 *
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "LICENSE.txt".
 */
#include <nautilus/nautilus.h>
#include <nautilus/paging.h>
#include <nautilus/first_touch_patch.h>
#include <nautilus/naut_string.h>
#include <nautilus/mb_utils.h>
#include <nautilus/idt.h>
#include <nautilus/cpu.h>
#include <nautilus/errno.h>
#include <nautilus/cpuid.h>
#include <nautilus/backtrace.h>
#include <nautilus/macros.h>
#include <nautilus/naut_assert.h>
#include <nautilus/numa.h>
#include <nautilus/mm.h>
#include <lib/bitmap.h>
#include <nautilus/percpu.h>

#ifdef NAUT_CONFIG_XEON_PHI
#include <nautilus/sfi.h>
#endif

#ifdef NAUT_CONFIG_HVM_HRT
#include <arch/hrt/hrt.h>
#endif

#ifdef NAUT_CONFIG_ASPACES
#include <nautilus/aspace.h>
#endif

#ifndef NAUT_CONFIG_DEBUG_PAGING
#undef DEBUG_PRINT
#define DEBUG_PRINT(fmt, args...)
#endif


extern uint8_t boot_mm_inactive;

extern ulong_t pml4;


static char * ps2str[3] = {
    [PS_4K] = "4KB",
    [PS_2M] = "2MB",
    [PS_1G] = "1GB",
};


extern uint8_t cpu_info_ready;

/*
 * align_addr
 *
 * align addr *up* to the nearest align boundary
 *
 * @addr: address to align
 * @align: power of 2 to align to
 *
 * returns the aligned address
 *
 */
static inline ulong_t
align_addr (ulong_t addr, ulong_t align)
{
    ASSERT(!(align & (align-1)));
    return (~(align - 1)) & (addr + align);
}


static inline int
gig_pages_supported (void)
{
    cpuid_ret_t ret;
    struct cpuid_amd_edx_flags flags;
    cpuid(CPUID_AMD_FEATURE_INFO, &ret);
    flags.val = ret.d;
    return flags.pg1gb;
}


static page_size_t
largest_page_size (void)
{
    return PS_2M;

    if (gig_pages_supported()) {
        return PS_1G;
    }

    return PS_2M;
}


static uint64_t infer_page_size(page_size_t ps){

  switch(ps){
     case PS_4K:
	 return  PAGE_SIZE_4KB;
     case PS_2M:
	 return  PAGE_SIZE_2MB;
     case PS_1G:
	 return  PAGE_SIZE_1GB;

    default:
	 panic("ps not supported \n");
  }

  return 0;
}


static NK_LOCK_T drill_lock;

static int drill_addr(addr_t fault_addr, int invalidate,  cpu_id_t cpuid){


    page_size_t ps = largest_page_size();


    pml4e_t * pml4 = (pml4e_t*)read_cr3();
    // _pml4 should exist
    uint_t pml4_idx = PADDR_TO_PML4_IDX(fault_addr);
    ASSERT(PML4E_PRESENT(pml4[pml4_idx]));

    //pdpte_t * pdpt  = (pdpte_t*)(pml4[pml4_idx] & PTE_ADDR_MASK);
    pdpte_t * pdpt  = (pdpte_t*) PTE_ADDR(pml4[pml4_idx] );

    uint_t pdpt_idx = PADDR_TO_PDPT_IDX(fault_addr);

    int kernel_mem = fault_addr <= 0x100000000 ? 1 : 0;

    uint64_t page_size = infer_page_size(ps);

   
    //check if fault_addr still fault

    if (ps == PS_1G){
       //printk("ps 1 G begin drill \n");
       //we know it's identity mapping
       //we don't want to disturb alignment at boot allocated using mm_boot_alloc_aligned
       if (!kernel_mem){
	   // pdpt[pdpt_idx] = fault_addr |  PTE_PRESENT_BIT | PTE_WRITABLE_BIT  | PTE_PAGE_SIZE_BIT;

           //allocate to

	   if (invalidate)  pdpt[pdpt_idx]  = fault_addr | PTE_WRITABLE_BIT | PTE_PAGE_SIZE_BIT;

	   else{

	      NK_LOCK(&drill_lock);

	      if(PTE_PRESENT(pdpt[pdpt_idx])){NK_UNLOCK(&drill_lock); return 0; }
	      	   
	      addr_t pa = (addr_t) kmem_malloc_specific(page_size, cpuid, 0);
              DEBUG_PRINT("PAGING: numa aware reallocate %p to %p\n", fault_addr, pa);
	      pdpt[pdpt_idx] = pa |  PTE_PRESENT_BIT | PTE_WRITABLE_BIT  | PTE_PAGE_SIZE_BIT;
	      NK_UNLOCK(&drill_lock);
           }
       }

    }else if (ps == PS_2M) {

        // printk("ps 2M begin drill \n");
         pde_t * pd = (pde_t*) PTE_ADDR(pdpt[pdpt_idx]);
	 ASSERT(PD_PRESENT(pd));
         uint_t pd_idx = PADDR_TO_PD_IDX(fault_addr);

	 if ( !kernel_mem ){
	   //pd[pd_idx] = fault_addr |  PTE_PRESENT_BIT | PTE_WRITABLE_BIT | PTE_PAGE_SIZE_BIT;

	   //allocate to
	   if (invalidate) pd[pd_idx] = fault_addr | PTE_WRITABLE_BIT | PTE_PAGE_SIZE_BIT;
           else{

	       NK_LOCK(&drill_lock);
	       if(PTE_PRESENT(fault_addr)){NK_UNLOCK(&drill_lock); return 0; }

               addr_t pa = (addr_t) kmem_malloc_specific(page_size, cpuid, 0);
               DEBUG_PRINT("PAGING: numa aware reallocate %p to %p\n", fault_addr, pa);
	       pd[pd_idx] = pa |  PTE_PRESENT_BIT | PTE_WRITABLE_BIT | PTE_PAGE_SIZE_BIT;

	      NK_UNLOCK(&drill_lock);
	    }
	 }
    }

    //idealy copy the original data in fault address
    NK_UNLOCK(&drill_lock);

    return 0;

}


/*
 * invalidate pages from 4gb to 8gb where
 * resides the bss of benchmarks
 * later in page fault, remap it to near the processor
 * possible problems: the remapped pages is unaware of kmem allocator and
 * kmem allocator maintain a va that map to this pa, will be a corruption
 * we will see...
 *
 */

int nk_invalidate_4gb_to_8gb_pages(){

   NK_LOCK_INIT(&drill_lock);

   extern addr_t _nasbssStart, _nasbssEnd;

   addr_t nasbssStart = (addr_t) &_nasbssStart;
   addr_t nasbssEnd = (addr_t) &_nasbssEnd;

   printk("invalidate all mem pags from %p to %p\n", nasbssStart, nasbssEnd);

   addr_t tmp_addr = ROUND_DOWN_TO_PAGE( nasbssStart);

   page_size_t ps = largest_page_size();

   uint64_t pagesize = infer_page_size(ps);

   DEBUG_PRINT("start at tmp_addr %p\n", tmp_addr);
   while(tmp_addr < nasbssEnd){

     //force not present
     drill_addr(tmp_addr, 1, 0);
     tmp_addr = ROUND_DOWN_TO_PAGE (tmp_addr+pagesize);
  
    // printk("next tmp_addr %p\n", tmp_addr);
   }

}

/*
 * nk_pf_handler
 *
 * page fault handler
 *
 */
int
nk_pf_handler (excp_entry_t * excp,
               excp_vec_t     vector,
               void         * state)
{

    cpu_id_t id = cpu_info_ready ? my_cpu_id() : 0xffffffff;
    uint64_t fault_addr = read_cr2();

#ifdef NAUT_CONFIG_HVM_HRT
    if (excp->error_code == UPCALL_MAGIC_ERROR) {
        return nautilus_hrt_upcall_handler(NULL, 0);
    }
#endif

#ifdef NAUT_CONFIG_ASPACES
    if (!nk_aspace_exception(excp,vector,state)) {
	return 0;
    }
#endif

#ifdef NAUT_CONFIG_ENABLE_MONITOR
    int nk_monitor_excp_entry(excp_entry_t * excp,
			      excp_vec_t vector,
			      void *state);

    return nk_monitor_excp_entry(excp, vector, state);
#endif

/*
    printk("\n+++ Page Fault +++\n"
            "RIP: %p    Fault Address: 0x%llx \n"
            "Error Code: 0x%x    (core=%u)\n",
            (void*)excp->rip,
            fault_addr,
            excp->error_code,
            id);
*/

    //idealy copy the original data in fault address


    drill_addr(fault_addr, 0, id);

    return 0;
    /*
    struct nk_regs * r = (struct nk_regs*)((char*)excp - 128);
    nk_print_regs(r);
    backtrace(r->rbp);

    panic("+++ HALTING +++\n");
    return 0;
    */
}
