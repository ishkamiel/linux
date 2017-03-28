#ifndef _X86_INCLUDE_ASM_MPXK_H_
#define _X86_INCLUDE_ASM_MPXK_H_

#include <linux/types.h>

#ifndef CONFIG_X86_INTEL_MPX_KERNEL
/* Use the generic headaer that provides empty definitions */
#include <asm-generic/mpxk.h>
#else /*CONFIG_X86_INTEL_MPX_KERNEL */

extern void mpxk_print_bounds(const char* str, const void* ptr);

#define MPXK_BUILTIN_DEF(r,f,...) extern r f(__VA_ARGS__);
#define MPXK_WRAPPER_DEF(r,f,...) extern r mpxk_wrapper_##f(__VA_ARGS__);

#include "../../../../scripts/gcc-plugins/mpxk_builtins.def"

#undef MPXK_WRAPPER_DEF
#undef MPXK_BUILTIN_DEF

#endif /*CONFIG_X86_INTEL_MPX_KERNEL */
#endif /* _X86_INCLUDE_ASM_MPXK_H_ */
