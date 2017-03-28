#include <asm/mpx.h>
#include <asm/mpxk.h>

void mpxk_print_bounds(const char* str, const void* ptr)
{
	const unsigned long range = (((unsigned long)__bnd_get_ptr_ubound(ptr)) -
			((unsigned long)__bnd_get_ptr_lbound(ptr)));

	pr_info("%s: pointer %pK (bounds %pK + %ld\n",
			str, ptr, __bnd_get_ptr_lbound(ptr), range);
}

void *mpxk_load_bounds(void *ptr)
{
	size_t size;

	if (PageSlab(virt_to_page(ptr))) {
		size = ksize(ptr);

		if (size == 0)
			return __bnd_null_ptr_bounds(ptr);
		return __bnd_set_ptr_bounds(ptr, size);
	}

	return __bnd_init_ptr_bounds(ptr);
}
