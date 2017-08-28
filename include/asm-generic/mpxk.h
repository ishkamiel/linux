/*
 * include/asm-generic/mpxk.h
 *
 * Copyright (C) 2017 Aalto University
 */
#ifndef _ASM_MPXK_H_
#define _ASM_MPXK_H_

#include <asm/mpx.h>
#include <linux/types.h>

extern void mpxk_dump_stats(void)
{}

static inline void mpxk_enable_mpx(void)
{}

static inline void mpxk_print_bounds(const char *str, const void *ptr)
{
	pr_info("%s: MPXK disabled, no bounds for pointer %pK\n", str, ptr);
}

#endif /* _ASM_MPXK_H_ */
