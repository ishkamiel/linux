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

#undef MPXK_WRAPPER_DEF
#undef MPXK_BUILTIN_DEF

void *mpxk_wrapper_kmalloc(size_t s, gfp_t f)
{
	void *p = kmalloc(s, f);

	if (p)
		return __bnd_set_ptr_bounds(p, s);
	return __bnd_null_ptr_bounds(p);
}

void *mpxk_wrapper_krealloc(void *p, size_t s, gfp_t f)
{
	if (!p)
		return mpxk_wrapper_kmalloc(s, f);

	__bnd_chk_ptr_lbounds(p);
	p = krealloc(p, s, f);

	if (p)
		return __bnd_set_ptr_bounds(p, s);
	return __bnd_null_ptr_bounds(p);
}

void *mpxk_wrapper_memmove(void *d, const void *s, size_t c)
{
	if (c == 0)
		return d;

	__bnd_chk_ptr_bounds(d, c);
	__bnd_chk_ptr_bounds(s, c);

	return memmove(d, s, c);
}

void *mpxk_wrapper_memcpy(void *d, const void *s, size_t c)
{
	if (c == 0)
		return d;

	__bnd_chk_ptr_bounds(d, c);
	__bnd_chk_ptr_bounds(s, c);

	return memcpy(d, s, c);
}

/* Because the MPXK gcc-plugin works on preprocessed code it cannot properly
 * handle macro expanded calls such as potential memcpy -> __memcpy changes.
 * This probably needs to be solved in some cleaner way, and probably also
 * affects other function besides memcpy.
 */
void *mpxk_wrapper___inline_memcpy(void *d, const void *s, size_t c)
{
	if (c == 0)
		return d;

	__bnd_chk_ptr_bounds(d, c);
	__bnd_chk_ptr_bounds(s, c);

	return __inline_memcpy(d, s, c);
}

void *mpxk_wrapper___memcpy(void *d, const void *s, size_t c)
{
	if (c == 0)
		return d;

	__bnd_chk_ptr_bounds(d, c);
	__bnd_chk_ptr_bounds(s, c);

	return __memcpy(d, s, c);
}

void *mpxk_wrapper_memset(void *s, int c, size_t l)
{
	if (l > 0) {
		__bnd_chk_ptr_bounds(s, l);
		memset(s, c, l);
	}
	return s;
}

char *mpxk_wrapper_strcat(char *d, const char *s)
{
	size_t ds = mpxk_wrapper_strlen(d);
	size_t ss = mpxk_wrapper_strlen(s);

	__bnd_chk_ptr_bounds(d, ds + ss + 1);
	__bnd_chk_ptr_bounds(s, ss + 1);

	return strcat(d, s);
}

char *mpxk_wrapper_strncat(char *d, const char *s, size_t c)
{
	size_t ds = mpxk_wrapper_strlen(d);
	size_t ss = mpxk_wrapper_strnlen(s, c);

	__bnd_chk_ptr_bounds(d, ds + ss + 1);
	__bnd_chk_ptr_bounds(s, (ss < c ? ss + 1 : ss));

	return strncat(d, s, c);
}

char *mpxk_wrapper_strcpy(char *d, const char *s)
{
	size_t ss = mpxk_wrapper_strlen(s);

	__bnd_chk_ptr_bounds(d, ss + 1);
	__bnd_chk_ptr_bounds(s, ss + 1);

	return strcpy(d, s);
}

char *mpxk_wrapper_strncpy(char *d, const char *s, size_t c)
{
	size_t ss = strnlen(s, c);

	__bnd_chk_ptr_bounds(d, c);
	__bnd_chk_ptr_bounds(s, (ss < c ? ss + 1 : ss));

	return strncpy(d, s, c);
}

size_t mpxk_wrapper_strlen(const char *s)
{
	const size_t l = strlen(s);

	__bnd_chk_ptr_bounds(s, l + 1);
	return l;
}

size_t mpxk_wrapper_strnlen(const char *s, size_t c)
{
	const size_t l = strnlen(s, c);

	__bnd_chk_ptr_bounds(s, l + 1);
	return l;
}
