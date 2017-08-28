/*
 * arch/x86/include/asm/mpxk.h
 *
 * Copyright (C) 2017 Aalto University
 */
#ifndef _X86_INCLUDE_ASM_MPXK_H_
#define _X86_INCLUDE_ASM_MPXK_H_

#ifndef CONFIG_X86_INTEL_MPX_KERNEL
/* Use the generic header that provides empty definitions */
#include <asm-generic/mpxk.h>
#else /*CONFIG_X86_INTEL_MPX_KERNEL */

extern void mpxk_dump_stats(void);
extern void mpxk_enable_mpx(void);
extern void mpxk_print_bounds(const char *str, const void *ptr);

#endif /*CONFIG_X86_INTEL_MPX_KERNEL */
#endif /* _X86_INCLUDE_ASM_MPXK_H_ */
