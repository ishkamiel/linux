#undef pr_fmt
#include "lkdtm_mpxk.h"
#include <linux/list.h>
#include <linux/sched.h>
#include <linux/time.h>
#include <linux/slab.h>
#include <linux/printk.h>
#include <asm/mpxk.h>


void lkdtm_MPXK_KMALLOC(void)
{
	void *ptr = kmalloc(10, GFP_KERNEL);

	pr_info("attempting good write\n");
	try_write(ptr, 1);

	pr_info("attempting bad write\n");
	try_write(ptr, 11);

	kfree(ptr);
}


#define ARR_SIZE 40
#define BND_BASIC 0
#define BND_ARRAY 1
#define BND_FUNC10 2
#define SET_IJ(x) do {			\
	i = x;				\
	j = ARR_SIZE * (x+1) - 1;	\
} while (0);

void do_the_test(const int test_num) {
	int i;
	int j;
	/* Wrapper should ensure we get bounds set for arr. */
	char **arr = (char **) kmalloc(10 * sizeof(char *), GFP_KERNEL);

	for (i = 0; i < 10; i++)
		/* BNDSTX will be needed to store all bounds. */
		arr[i] = (char *) kmalloc(ARR_SIZE * (i + 1), GFP_KERNEL);

	switch(test_num) {
		case BND_BASIC:
			/* Bad attempt should BT due to the arr bounds themselves.
			 * This happens provided that the normal wrapper is okay
			 * and the "normal" MPX bound passing is intact. */
			pr_info("attempting good ptr write\n");
			SET_IJ(0);
			mpxk_write_arr_i(arr, i, j);

			pr_info("attempting bad ptr write\n");
			mpxk_write_arr_i(arr, 10, 0);
			break;
		case BND_ARRAY:
			/* Tests bounds passed via function arguments. The
			 * offending bounds are in the strings, and thus need
			 * to be loaded via BNDLDX.
			 *
			 * The first bad attempt might succeed. */
			pr_info("attempting good ptr write\n");
			SET_IJ(2);
			mpxk_write_arr_i(arr, i, j);

			SET_IJ(3);
			pr_info("attempting exact (+1) bad ptr write (can succeed)");
			mpxk_write_arr_i(arr, i, j + 1);

			pr_info("attempting real bad ptr write (should be caught)\n");
			mpxk_write_arr_i(arr, i, j * 16);
			break;
		case BND_FUNC10:
			/* Tests bounds passed in via a function taking 10 strings.
			 * The bounds cannot be passed via bound registers, so the
			 * function argument passing will use BNDSTX+BNDSTX.
			 *
			 * The first bad attempt might succeed depending due to the
			 * mpxk load retrieving bounds based on MM metadata.
			 *
			 * The second bad attempt should hopefully be "bad enough". */
			pr_info("attempting good ptr write\n");
			SET_IJ(8);
			mpxk_write_10_i(2, 40 * 3 -1,
					arr[0], arr[1], arr[2], arr[3], arr[4],
					arr[5], arr[6], arr[7], arr[8], arr[9]);

			SET_IJ(7);
			pr_info("attempting exact bad ptr write\n");
			mpxk_write_10_i(2, 40 * 3 -1,
					arr[0], arr[1], arr[2], arr[3], arr[4],
					arr[5], arr[6], arr[7], arr[8], arr[9]);
			mpxk_write_arr_i(arr, 8, 40 * 9);

			SET_IJ(6);
			pr_info("attempting real bad ptr write\n");
			mpxk_write_arr_i(arr, 2, 40 * 16);
			break;
	}

	for (i = 0; i < 10; i++)
		kfree(arr[i]);
}

void lkdtm_MPXK_BND_BASIC(void) { do_the_test(BND_BASIC); }
void lkdtm_MPXK_BND_ARRAY(void) { do_the_test(BND_ARRAY); }
void lkdtm_MPXK_BND_FUNC10(void) { do_the_test(BND_FUNC10);}

noinline
void memcpy_test(void **arr, int s, int d, int count)
{
	/* Use array to force BNDSTX/BNDLDX */
	void *dst = arr[d];
	void *src = arr[s];

	mpxk_print_bounds("dst", dst);
	mpxk_print_bounds("src", src);

	pr_info("Copying '%s' to '%s'\n", (char *)src, (char *)dst);

	memcpy(dst, src, count);

	pr_info("Dst is now '%s'\n", (char *)dst);
}

void lkdtm_MPXK_BAD_MEMCPY(void)
{
	char *s  = "123456789";
	char *d = kmalloc(4 * sizeof(char), GFP_KERNEL);
	char *o1 = "000000000ABC";
	char *o2 = "BBBBBBBBBBBB";
	void *arr[4];

	arr[0] = s;
	arr[1] = d;
	arr[2] = o1;
	arr[3] = o2;

	pr_info("performing okay memcpy\n");
	memcpy_test(arr, 0, 1, 1);

	/* pr_info("performing bad, but uncaught memcpy\n"); */
	/* Bounds of sack pointer not loaded by mpxk_load_bounds */
	/* memcpy_test(arr, 2, 0, 12); */

	pr_info("performing bad, and caught memcpy\n");
	memcpy_test(arr, 0, 1, 2049);
}
