#undef pr_fmt
#include "lkdtm_mpxk.h"
#include <linux/printk.h>

/**
 * try_write - Attempt write at pointed memory
 *
 * On bad writes this will either cause a bound violation, or
 * when SOFT_TEST is set pritn out "fail".
 */
noinline void try_write(void *ptr, int i)
{
#ifdef SOFT_TEST
	const void *ubound = __bnd_get_ptr_ubound(ptr);
	const void *lbound = __bnd_get_ptr_lbound(ptr);
	if (ptr < lbound || (ptr+i) > ubound)
		pr_info("fail\n");
	else
		pr_info("ok\n");
#else
	((char *)ptr)[i] = '\0';
#endif /* SOFT_TEST */
}

/**
 * mpxk_write_arr_i - Test function that writes to array.
 *
 * The boudns for the inner array cannot be passed in via stack/registers and
 * are therefore loaded with mpxk_load_bounds (and would have been passed in
 * vanilla MPX with BNDSTX+BNDLDX).
 */
noinline void mpxk_write_arr_i(char **arr, int i, int j)
{
	try_write(arr[i], j);
}

/**
 * mpxk_write_10_i - Test function that writes to function arg strings.
 *
 * Because bounds cannot be passed beyond the sixth argument (or the fourth
 * bound) this forces MPXK to use mpxk_load_bounds for the latter pointers.
 */
noinline void mpxk_write_10_i(int i, int j,
		void *s0, void *s1, void *s2, void *s3, void *s4,
		void *s5, void *s6, void *s7, void *s8, void *s9)
{
	/* This is quite messy, but we wish to make sure the compiler doesn't
	 * modify the behavior of these. */
	if (i == 0) try_write(s0, j);
	if (i == 1) try_write(s1, j);
	if (i == 2) try_write(s2, j);
	if (i == 3) try_write(s3, j);
	if (i == 4) try_write(s4, j);
	if (i == 5) try_write(s5, j);
	if (i == 6) try_write(s6, j);
	if (i == 7) try_write(s7, j);
	if (i == 8) try_write(s8, j);
	if (i == 9) try_write(s9, j);
}
