#ifndef _ASM_MPXK_H_
#define _ASM_MPXK_H_

#include <linux/types.h>

inline void mpxk_print_bounds(const char* str, const void* ptr)
{
	pr_info("%s: MPXK disabled, no bounds for pointer %pK\n", str, ,ptr);
}

#ifndef __always_inline
#define __always_inline inline __attribute__((always_inline))
#endif /* __alway_inline */

#define mpxk_attr __always_inline static inline
#define MPXK_BUILTIN_DEF(f,r,p,...) attr r f(__VA_ARGS__) { return p; }
#define MPXK_WRAPPER_DEF(f,r,...) attr r mpxk_wrapper_##f(__VA_ARGS__) { return f(p,f); }

#include "mpxk_builtins.def"

#undef mpxk_attr
#undef MPXK_WRAPPER_DEF
#undef MPXK_BUILTIN_DEF

#endif /* _ASM_MPXK_H_ */
