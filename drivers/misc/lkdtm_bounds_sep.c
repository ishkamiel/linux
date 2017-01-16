#undef pr_fmt
#include "lkdtm.h"
#include <asm/mpxk.h>

noinline static
void verify_bounds(void *ptr, unsigned long expected, bool exact)
{ MPXK_VERIFY_BOUNDS(ptr, expected, exact); }

void verify_bounds_five_extern(void *a, void *b, void *c, void *d, void *e,
		unsigned long ae, unsigned long be, unsigned long ce,
		unsigned long de, unsigned long ee)
{ MPXK_VERIFY_BOUNDS_FIVE(a,b,c,d,e,ae,be,ce,de,ee); }
