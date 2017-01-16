#undef pr_fmt
#include "lkdtm.h"
#include <asm/mpxk.h>

/* #define SOFT_TEST */

void try_write(void *ptr, int i);
void mpxk_write_arr_i(char **arr, int i, int j);
noinline void mpxk_write_10_i(int i, int j,
		void *s0, void *s1, void *s2, void *s3, void *s4,
		void *s5, void *s6, void *s7, void *s8, void *s9);
