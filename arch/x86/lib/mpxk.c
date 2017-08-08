/*
 * arch/x86/lib/mpxk.c
 *
 * Copyright (C) 2017 Aalto University
 */
#include <asm/siginfo.h>
#include <linux/vmalloc.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <asm/pgtable_64.h>

#include <asm/mpx.h>
#include <asm/mpxk.h>

static struct msr bnd_cfg_s;

__attribute__((bnd_legacy))
static void mpxk_enable_mpx_cfgs_cpu(void *info)
{
	(void) info;
	wrmsrl(MSR_IA32_BNDCFGS, bnd_cfg_s.q);
}

__attribute__((bnd_legacy))
void mpxk_enable_mpx(void)
{
	void *ptr = get_vm_area(MPX_BD_SIZE_BYTES_64 + PAGE_SIZE, VM_MAP);

	if (!cpu_feature_enabled(X86_FEATURE_MPX)) {
		pr_info("mpxk: Intel MPX not available\n");
		kfree(ptr); /* TODO: Check on whether kfree is correct */
		return;
	}

	bnd_cfg_s.q = PAGE_ALIGN((unsigned long) ptr);
	bnd_cfg_s.q |= MPX_BNDCFG_ENABLE_FLAG;

	pr_info("mpxk: Setting up Intel MPX for kernel\n");

	/* Config is passed via the global bnd_cfg_s.q */
	on_each_cpu(mpxk_enable_mpx_cfgs_cpu, NULL, 1);
}

void mpxk_print_bounds(const char *str, const void *ptr)
{
	const unsigned long range = (((unsigned long)__bnd_get_ptr_ubound(ptr))
			- ((unsigned long)__bnd_get_ptr_lbound(ptr)));

	pr_info("%s: pointer %pK (bounds %pK + %ld\n",
			str, ptr, __bnd_get_ptr_lbound(ptr), range);
}

void *mpxk_load_bounds(void *ptr)
{
	size_t size;

	do {
		if (ptr == NULL)
			break;

		if (!virt_addr_valid(ptr))
			break;

		if (!PageSlab(virt_to_page(ptr)))
			break;

		size = ksize(ptr);

		if (size == 0)
			return __bnd_null_ptr_bounds(ptr);
		return __bnd_set_ptr_bounds(ptr, size);
	} while (0);

	return __bnd_init_ptr_bounds(ptr);
}
EXPORT_SYMBOL(mpxk_load_bounds);
