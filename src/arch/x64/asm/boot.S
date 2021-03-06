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
 * http://xtack.sandia.gov/hobbes
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

.code32
.section .mbhdr
.align 8

#include <asm/lowlevel.h>

/**
 * This header makes us multi-boot compliant
 */
multiboot_hdr:
    .long 0xe85250d6
    .long 0
    .long hdr_end - multiboot_hdr
    .long -(0xe85250d6 + 0 + (hdr_end - multiboot_hdr))

    /* BEGIN TAGS */

    /* sections */
 
    .word 2, 0
    .long 24
    .long multiboot_hdr
    .long _loadStart
    .long _loadEnd
    .long _bssEnd

    /* entry point */
    .word 3, 0
    .long 16
    .long nautilus_entry
    .long 0

    /* tags end */
    .word 0, 0
    .long 8
hdr_end:

.section .boot

/**
 * our entry point into the kernel, this is where
 * GRUB2 will drop us
 */
 GLOBAL(nautilus_entry)

    /* get GRUB multiboot info and stash it */
    movl %ebx, %edi
    movl %eax, %esi

    /* don't trust GRUB's protected-mode GDT */
    movl $gdtr32, %eax
    lgdt (%eax)
    ljmp $0x8, $.gdt1_is_go


.gdt1_is_go:
    movl $0x10, %eax
    movw %ax, %ds
    movw %ax, %ss
    movl $boot_stack_end-1, %esp

    call paging_longmode_setup

    /* now our long mode GDT */
    movl $gdtr64, %eax
    lgdt (%eax)

    ljmp $0x8, $.gdt2_is_go

.code64
.gdt2_is_go:
    movl $0x10, %eax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %ss
    movw %ax, %fs
    movw %ax, %gs

    movq $boot_stack_end, %rsp

    /* multiboot info is still in rdi and rsi */
    movq $main, %rax
    callq *%rax

    /* shouldn't get here */
    cli
    jmp .

.code32
paging_longmode_setup:

    /* PML4[0] -> PDPT */
    movl $pdpt, %eax
    orl $0x3, %eax       /* entry is present, rw */
    movl $pml4, %ebx
    movl %eax, (%ebx)

    // PDPT[0] -> PDT 
    movl $pd, %eax
    orl $0x3, %eax
    movl $pdpt, %ebx
    movl %eax, (%ebx)
   
    //set first 4 entry of PDPT 4GB total
    movl $2, %ecx
    movl $pd, %edx
    movl $0x83, %eax // set PS bit also (PDE -> 2MB page)

.write_pdpe:
    push %ecx

    // Identity map the first GB
    movl $512, %ecx
.write_pde:
    movl %eax, (%edx)
    addl $0x200000, %eax
    addl $0x8, %edx
    loop .write_pde 
    
    push %edx
    orl $0x3, %edx
    addl $0x8, %ebx  //next entry of pdpt
    movl %edx, (%ebx) 
    pop %edx
    pop %ecx
    loop .write_pdpe


    /* put pml4 address in cr3 */
    movl $pml4, %eax
    movl %eax, %cr3

    /* enable PAE */
    mov %cr4, %eax
    or $(1<<5), %eax
    mov %eax, %cr4

    /* enable lme bit in MSR */
    movl $0xc0000080, %ecx
    rdmsr
    orl $(1<<8), %eax
    wrmsr

    /* paging enable */
    movl %cr0, %eax
    orl $(1<<31), %eax

    /* make sure we're in "normal cache mode" */
    movl $~(3<<29), %ebx
    andl %ebx, %eax

    movl %eax, %cr0

    ret
     
.align 8
gdt32:
    .quad 0x0000000000000000 /* null */
    .quad 0x00cf9a000000ffff /* code */
    .quad 0x00cf92000000ffff /* data */

.align 8
GLOBAL(gdt64)
    .quad 0x0000000000000000 /* null */
    .quad 0x00af9a000000ffff /* code (note lme bit) */
    .quad 0x00af92000000ffff /* data (most entries don't matter) */
#ifdef NAUT_CONFIG_USE_IST
    .quad 0x0000000000000000 /* TSS static allocation, update the value in init */
    .quad 0x0000000000000000 /* TSS static allocation, upper half*/

.align 8
GLOBAL(tss64)
    .long 0x00000000         /* Reserved per spec */
    .quad 0x0000000000000000 /* RSP0. All RSPx fields should never be used as we are always in ring 0 */
    .quad 0x0000000000000000 /* RSP1 */
    .quad 0x0000000000000000 /* RSP2 */
    .long 0x00000000         /* Reserved per spec */
    .long 0x00000000         /* Reserved per spec */
    .quad 0x0000000000000000 /* IST1 */
    .quad 0x0000000000000000 /* IST2 */
    .quad 0x0000000000000000 /* IST3 */
    .quad 0x0000000000000000 /* IST4 */
    .quad 0x0000000000000000 /* IST5 */
    .quad 0x0000000000000000 /* IST6 */
    .quad 0x0000000000000000 /* IST7 */
    .long 0x00000000         /* Reserved per spec */
    .long 0x00000000         /* Reserved per spec */
    .word 0x0000             /* Reserved per spec */
    .word 0x0068             /* IOPB offset */
#endif

.align 8
gdtr32:
    .word 23
    .long gdt32

.align 8
GLOBAL(gdtr64)
#ifdef NAUT_CONFIG_USE_IST
    .word 39
#else
    .word 23
#endif
    .quad gdt64

