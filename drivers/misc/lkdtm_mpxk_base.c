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

#define mpxk_func_case(x) do {		\
	if (i == x)			\
		try_write(s##x, j);	\
} while (0)
	mpxk_func_case(0);
	mpxk_func_case(1);
	mpxk_func_case(2);
	mpxk_func_case(3);
	mpxk_func_case(4);
	mpxk_func_case(5);
	mpxk_func_case(6);
	mpxk_func_case(7);
	mpxk_func_case(8);
	mpxk_func_case(9);
#undef mpxk_func_case
}
