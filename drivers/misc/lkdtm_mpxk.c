#include <linux/slab.h>
#undef pr_fmt
#include "lkdtm.h"

void try_write(void *ptr, int i);
void mpxk_write_arr_i(char **arr, int i, int j);
noinline void mpxk_write_10_i(int i, int j,
		void *s0, void *s1, void *s2, void *s3, void *s4,
		void *s5, void *s6, void *s7, void *s8, void *s9);

/**
 * try_write - Attempt write at pointed memory
 *
 * On bad writes this will either cause a bound violation.
 */
noinline void try_write(void *ptr, int i)
{
	((char *)ptr)[i] = '\0';
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

	pr_info("cleaning up\n");
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

	pr_info("cleaning up\n");
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

	pr_info("cleaning up\n");
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

	pr_info("cleaning up\n");
	kfree(d);
}
