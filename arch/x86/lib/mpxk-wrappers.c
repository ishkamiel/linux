#include <asm/mpxk.h>

#include <linux/export.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <asm/page.h>
#include <asm/pgtable_64.h>
#include <linux/page-flags.h>

void *mpxk_wrapper_kmalloc(size_t s, gfp_t flags)
{
	void *p;

	p = kmalloc(s, flags);

	if (p) {
		p = __bnd_set_ptr_bounds(p, s);
		return p;
	}

	return __bnd_null_ptr_bounds(p);
}
