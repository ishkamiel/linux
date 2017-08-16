/*
 * arch/x86/lib/mpxk-wrappers.h
 *
 * Copyright (C) 2017 Aalto University
 */
#include <linux/page-flags.h>
#include <linux/vmalloc.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <asm/page.h>
#include <asm/pgtable_64.h>

#define MPXK_BUILTIN_DEF(r, f, ...) extern r f(__VA_ARGS__);
#define MPXK_WRAPPER_DEF(r, f, ...) extern r mpxk_wrapper_##f(__VA_ARGS__);

#include "../../../../scripts/gcc-plugins/mpxk_builtins.def"

struct mpxk_f_stats {
	long unsigned int miss;
	long unsigned int hit;
};

struct mpxk_rt_stats {
	struct mpxk_f_stats memset;
	struct mpxk_f_stats memcpy;
	struct mpxk_f_stats memmove;
};

struct mpxk_rt_stats mpxk_stats;

#define dump_one(type) pr_info("%s,%lu,%lu\n", #type, mpxk_stats.type.hit, mpxk_stats.type.miss)

void mpxk_dump_stats(void)
{
	dump_one(memset);
	dump_one(memmove);
	dump_one(memcpy);
}

inline static bool has_bounds(const void *ptr)
{
	if (__bnd_get_ptr_ubound(ptr) != (void *) NULL - 1)
		return true;
	if (__bnd_get_ptr_lbound(ptr) != (void *) NULL)
		return true;
	return false;
}

inline static void log_ptr_bounds(struct mpxk_f_stats *f_stat, const void *ptr)
{
	if (has_bounds(ptr))
		f_stat->hit++;
	else
		f_stat->miss++;
}

#undef MPXK_WRAPPER_DEF
#undef MPXK_BUILTIN_DEF

#define wrapper_base(func) __mpxk_wrapper_base_##func
#define export_wrapper(func) EXPORT_SYMBOL(mpxk_wrapper_##func)

#define wrapper1(func, retval, a1t, a1v)		\
	__attribute__((bnd_legacy)) static inline	\
	retval __mpxk_wrapper_base_##func(a1t a1v)	\
	{						\
		return func(a1v);			\
	}						\
							\
	retval mpxk_wrapper_##func(a1t a1v)

#define wrapper2(func, retval, a1t, a1v, a2t, a2v)		\
	__attribute__((bnd_legacy)) static inline		\
	retval __mpxk_wrapper_base_##func(a1t a1v, a2t a2v)	\
	{							\
		return func(a1v, a2v);				\
	}							\
								\
	retval mpxk_wrapper_##func(a1t a1v, a2t a2v)

#define wrapper3(func, retval, a1t, a1v, a2t, a2v, a3t, a3v)		\
	__attribute__((bnd_legacy)) static inline			\
	retval __mpxk_wrapper_base_##func(a1t a1v, a2t a2v, a3t a3v)	\
	{								\
		return func(a1v, a2v, a3v);				\
	}								\
									\
	retval mpxk_wrapper_##func(a1t a1v, a2t a2v, a3t a3v)

wrapper2(kmalloc, void *, size_t, s, gfp_t, f)
{
	void *p = wrapper_base(kmalloc)(s, f);

	if (p)
		return __bnd_set_ptr_bounds(p, s);
	return __bnd_null_ptr_bounds(p);
}
export_wrapper(kmalloc);

wrapper3(krealloc, void *, void *, p, size_t, s, gfp_t, f)
{
	if (!p)
		return mpxk_wrapper_kmalloc(s, f);

	__bnd_chk_ptr_lbounds(p);
	p = wrapper_base(krealloc)(p, s, f);

	if (p)
		return __bnd_set_ptr_bounds(p, s);
	return __bnd_null_ptr_bounds(p);
}
export_wrapper(krealloc);

wrapper3(memmove, void *, void *, d, const void *, s, size_t, c)
{
	if (c == 0)
		return d;

	log_ptr_bounds(&mpxk_stats.memmove, d);
	log_ptr_bounds(&mpxk_stats.memmove, s);

	__bnd_chk_ptr_bounds(d, c);
	__bnd_chk_ptr_bounds(s, c);

	return wrapper_base(memmove)(d, s, c);
}
export_wrapper(memmove);

wrapper3(memcpy, void *, void *, d, const void *, s, size_t,  c)
{
	if (c == 0)
		return d;

	log_ptr_bounds(&mpxk_stats.memcpy, d);
	log_ptr_bounds(&mpxk_stats.memcpy, s);

	__bnd_chk_ptr_bounds(d, c);
	__bnd_chk_ptr_bounds(s, c);

	return wrapper_base(memcpy)(d, s, c);
}
export_wrapper(memcpy);

/* Because the MPXK gcc-plugin works on preprocessed code it cannot properly
 * handle macro expanded calls such as potential memcpy -> __memcpy changes.
 * This probably needs to be solved in some cleaner way, and probably also
 * affects other function besides memcpy.
 */
wrapper3(__inline_memcpy, void *, void *, d, const void *, s, size_t,  c)
{
	if (c == 0)
		return d;

	log_ptr_bounds(&mpxk_stats.memcpy, d);
	log_ptr_bounds(&mpxk_stats.memcpy, s);

	__bnd_chk_ptr_bounds(d, c);
	__bnd_chk_ptr_bounds(s, c);

	return wrapper_base(__inline_memcpy)(d, s, c);
}
export_wrapper(__inline_memcpy);

wrapper3(__memcpy, void *, void *, d, const void *, s, size_t,  c)
{
	if (c == 0)
		return d;

	log_ptr_bounds(&mpxk_stats.memcpy, d);
	log_ptr_bounds(&mpxk_stats.memcpy, s);

	__bnd_chk_ptr_bounds(d, c);
	__bnd_chk_ptr_bounds(s, c);

	return wrapper_base(__memcpy)(d, s, c);
}
export_wrapper(__memcpy);

wrapper3(memset, void *, void *, s, int,  c, size_t,  l)
{
	log_ptr_bounds(&mpxk_stats.memset, s);

	if (l > 0) {
		__bnd_chk_ptr_bounds(s, l);
		wrapper_base(memset)(s, c, l);
	}
	return s;
}
export_wrapper(memset);

wrapper1(strlen, size_t, const char *, s)
{
	const size_t l = wrapper_base(strlen)(s);

	__bnd_chk_ptr_bounds(s, l + 1);
	return l;
}
export_wrapper(strlen);

wrapper2(strnlen, size_t, const char *, s, size_t,  c)
{
	const size_t l = wrapper_base(strnlen)(s, c);

	__bnd_chk_ptr_bounds(s, l + 1);
	return l;
}
export_wrapper(strnlen);

wrapper2(strcat, char *, char *, d, const char *, s)
{
	size_t ds = wrapper_base(strlen)(d);
	size_t ss = wrapper_base(strlen)(s);

	__bnd_chk_ptr_bounds(d, ds + ss + 1);
	__bnd_chk_ptr_bounds(s, ss + 1);

	return wrapper_base(strcat)(d, s);
}
export_wrapper(strcat);

wrapper3(strncat, char *, char *, d, const char *, s, size_t,  c)
{
	size_t ds = wrapper_base(strlen)(d);
	size_t ss = wrapper_base(strnlen)(s, c);

	__bnd_chk_ptr_bounds(d, ds + ss + 1);
	__bnd_chk_ptr_bounds(s, (ss < c ? ss + 1 : ss));

	return wrapper_base(strncat)(d, s, c);
}
export_wrapper(strncat);

wrapper2(strcpy, char *, char *, d, const char *, s)
{
	size_t ss = wrapper_base(strlen)(s);

	__bnd_chk_ptr_bounds(d, ss + 1);
	__bnd_chk_ptr_bounds(s, ss + 1);

	return wrapper_base(strcpy)(d, s);
}
export_wrapper(strcpy);

wrapper3(strncpy, char *, char *, d, const char *, s, size_t,  c)
{
	size_t ss = wrapper_base(strnlen)(s, c);

	__bnd_chk_ptr_bounds(d, c);
	__bnd_chk_ptr_bounds(s, (ss < c ? ss + 1 : ss));

	return wrapper_base(strncpy)(d, s, c);
}
export_wrapper(strncpy);
