#ifndef _X86_INCLUDE_ASM_MPXK_H_
#define _X86_INCLUDE_ASM_MPXK_H_

/* #include <linux/types.h> */

#ifndef CONFIG_X86_INTEL_MPX_KERNEL
/* Use the generic headaer that provides empty definitions */
#include <asm-generic/mpxk.h>
#else /*CONFIG_X86_INTEL_MPX_KERNEL */

/* #define MPX_BD_SIZE_BYTES_64		(1UL<<31) */
/* #define MPX_BNDCFG_ENABLE_FLAG		0x1 */
/* #define MPX_BNDCFG_PRESERVE_FLAG	0x2 */

extern void mpxk_enable_mpx(void);
extern void mpxk_print_bounds(const char* str, const void* ptr);

#endif /*CONFIG_X86_INTEL_MPX_KERNEL */
#endif /* _X86_INCLUDE_ASM_MPXK_H_ */
