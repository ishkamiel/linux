#undef pr_fmt
#include "lkdtm_mpxk.h"
#include <linux/list.h>
#include <linux/sched.h>
#include <linux/time.h>
#include <linux/slab.h>
#include <linux/printk.h>
#include <asm/mpxk.h>

/** lkdtm_MPXK_LOAD_BOUNDS - test mpxk_bound_load
 *
 * Tests mpxk_load_bounds function by passing pointers into function via an
 * array. The bounds for the array itself are passed via the bnd0 register, but
 * MPX cannot do that for the internal pointers, hence it uses BNDSTX+BNDLDX.
 * MPXK therefore must use mpxk_load_bounds to retrieve the bounds inside the
 * called function.
 */
void lkdtm_MPXK_LOAD_BOUNDS(void)
{
	int i;
	char *arr[10];

	for (i = 0; i < 10; i++)
		arr[i] = kmalloc(16, GFP_KERNEL);

	pr_info("attempting good ptr write\n");
	mpxk_write_arr_i(arr, 2, 0);

	/* This could succeed because mpxk_load_bounds retrieved the size based
	 * on the pointer value via ksize, which in turn doesn't necessarily
	 * return the exact size that was passed into kmalloc. The size is none
	 * the less guaranteed to be "safe" in that it will not be reserved
	 * elsewhere.
	 */
	pr_info("attempting exact (+1) bad ptr write (can succeed)");
	mpxk_write_arr_i(arr, 4, 16);

	pr_info("attempting real bad ptr write (should be caught)\n");
	mpxk_write_arr_i(arr, 5, 1024);

	for (i = 0; i < 10; i++)
		kfree(arr[i]);
}

/** lkdtm_MPXK_FUNCTION_ARGS - test function argument bound propagation
 *
 * Note that the four first pointers will have their bounds passed into the
 * function via the bnd0-bnd3 registers. The rest are in vanilla MPX passed in
 * via BNDSTX+BNDLDX, but in the case of MPXK they are simply loaded inside the
 * called function using mpxk_load_bounds.
 */
void lkdtm_MPXK_FUNCTION_ARGS(void)
{
	int i;
	char *arr[10];

	for (i = 0; i < 10; i++)
		arr[i] = kmalloc(16, GFP_KERNEL);

	pr_info("attempting good ptr write\n");
	mpxk_write_10_i(8, 0,
			arr[0], arr[1], arr[2], arr[3], arr[4],
			arr[5], arr[6], arr[7], arr[8], arr[9]);

	pr_info("attempting exact bad ptr write\n");
	mpxk_write_10_i(9, 2,
			arr[0], arr[1], arr[2], arr[3], arr[4],
			arr[5], arr[6], arr[7], arr[8], arr[9]);

	pr_info("attempting real bad ptr write\n");
	mpxk_write_10_i(7, 1024,
			arr[0], arr[1], arr[2], arr[3], arr[4],
			arr[5], arr[6], arr[7], arr[8], arr[9]);

	for (i = 0; i < 10; i++)
		kfree(arr[i]);
}

/** lkdtm_MPXK_KMALLOC
 *
 * Make suer kmalloc is properly instrumented, i.e. it returns proper pointer
 * bounds on allocation.
 */
void lkdtm_MPXK_KMALLOC(void)
{
	void *ptr = kmalloc(10, GFP_KERNEL);

	pr_info("attempting good write\n");
	try_write(ptr, 1);

	pr_info("attempting bad write\n");
	try_write(ptr, 11);

	kfree(ptr);
}


/** lkdtm_MPXK_MEMCPY - test memcpy instrumentation
 *
 * Test memcpy instrumentation, which should check that both target and source
 * are within bounds (this exercises only destination bounds).
 */
void lkdtm_MPXK_MEMCPY(void)
{
	char *s  = "123456789";
	char *s_big = "12345678901234567890123456789012";
	char *d = kmalloc(4 * sizeof(char), GFP_KERNEL);

	pr_info("performing okay memcpy\n");
	memcpy(d, s, 1);

	/* The source is okay, but target is too small. */
	pr_info("performing bad memcpy\n");
	memcpy(d, s_big, 32 * sizeof(char));
}
