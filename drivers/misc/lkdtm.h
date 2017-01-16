#ifndef __LKDTM_H
#define __LKDTM_H

#define pr_fmt(fmt) "lkdtm: " fmt

#include <linux/kernel.h>

/* lkdtm_bugs.c */
void __init lkdtm_bugs_init(int *recur_param);
void lkdtm_PANIC(void);
void lkdtm_BUG(void);
void lkdtm_WARNING(void);
void lkdtm_EXCEPTION(void);
void lkdtm_LOOP(void);
void lkdtm_OVERFLOW(void);
void lkdtm_CORRUPT_STACK(void);
void lkdtm_UNALIGNED_LOAD_STORE_WRITE(void);
void lkdtm_SOFTLOCKUP(void);
void lkdtm_HARDLOCKUP(void);
void lkdtm_SPINLOCKUP(void);
void lkdtm_HUNG_TASK(void);
void lkdtm_ATOMIC_UNDERFLOW(void);
void lkdtm_ATOMIC_OVERFLOW(void);
void lkdtm_CORRUPT_LIST_ADD(void);
void lkdtm_CORRUPT_LIST_DEL(void);

#ifdef CONFIG_X86_INTEL_MPX_KERNEL
void lkdtm_MPX_BOUNDS_LOAD(void);
void lkdtm_MPX_BOUNDS_FUNC_EXTERN(void);
void lkdtm_MPX_BOUNDS_FUNC_INLINE(void);
void lkdtm_MPX_BOUNDS_FUNC_STATIC(void);
void lkdtm_MPX_BOUNDS_FUNC_NON_STATIC(void);
void lkdtm_MPX_BOUNDS_WRAPPER(void);
void lkdtm_MPXK_BND_BASIC(void);
void lkdtm_MPXK_BND_ARRAY(void);
void lkdtm_MPXK_BND_FUNC10(void);
#endif

/* lkdtm_heap.c */
void lkdtm_OVERWRITE_ALLOCATION(void);
void lkdtm_WRITE_AFTER_FREE(void);
void lkdtm_READ_AFTER_FREE(void);
void lkdtm_WRITE_BUDDY_AFTER_FREE(void);
void lkdtm_READ_BUDDY_AFTER_FREE(void);

/* lkdtm_perms.c */
void __init lkdtm_perms_init(void);
void lkdtm_WRITE_RO(void);
void lkdtm_WRITE_RO_AFTER_INIT(void);
void lkdtm_WRITE_KERN(void);
void lkdtm_EXEC_DATA(void);
void lkdtm_EXEC_STACK(void);
void lkdtm_EXEC_KMALLOC(void);
void lkdtm_EXEC_VMALLOC(void);
void lkdtm_EXEC_RODATA(void);
void lkdtm_EXEC_USERSPACE(void);
void lkdtm_ACCESS_USERSPACE(void);

/* lkdtm_rodata.c */
void lkdtm_rodata_do_nothing(void);

/* lkdtm_usercopy.c */
void __init lkdtm_usercopy_init(void);
void __exit lkdtm_usercopy_exit(void);
void lkdtm_USERCOPY_HEAP_SIZE_TO(void);
void lkdtm_USERCOPY_HEAP_SIZE_FROM(void);
void lkdtm_USERCOPY_HEAP_FLAG_TO(void);
void lkdtm_USERCOPY_HEAP_FLAG_FROM(void);
void lkdtm_USERCOPY_STACK_FRAME_TO(void);
void lkdtm_USERCOPY_STACK_FRAME_FROM(void);
void lkdtm_USERCOPY_STACK_BEYOND(void);
void lkdtm_USERCOPY_KERNEL(void);


/* TODO: MPXK temporary stuff, remove after completion
 *
 * These are here to decrease copy-pase during testing, will remove later!
 *
 * */
#define MPXK_VERIFY_BOUNDS(ptr, expected, exact) do {						\
	unsigned long range = __bnd_get_ptr_ubound(ptr) - __bnd_get_ptr_lbound(ptr) + 1;	\
	if (exact)										\
		if (range != expected)								\
			pr_info("%s: BAD_BOUNDS, was %lu, expected %lu\n",			\
					__func__, range, expected);				\
		else										\
			pr_info("%s: bounds okay, was %lu, expected %lu\n",			\
					__func__, range, expected);				\
	 else											\
		if (range < expected || range > expected * 16)					\
			pr_info("%s: BAD_BOUNDS, was %lu, expected %lu <= bound <= %lu\n",	\
					__func__, range, expected, expected*2);			\
		else										\
			pr_info("%s: bounds okay, was %lu, expected %lu <= bound <= %lu\n",	\
					__func__, range, expected, expected*2);			\
	} while (0)

#define MPXK_VERIFY_BOUNDS_FIVE(a,b,c,d,e,ae,be,ce,de,ee) do {	\
	verify_bounds(a, ae, false);				\
	verify_bounds(b, be, false);				\
	verify_bounds(c, ce, false);				\
	verify_bounds(d, de, false);				\
	verify_bounds(e, ee, false);				\
} while (0)


#endif
