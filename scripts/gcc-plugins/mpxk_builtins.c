/*
 * scripts/gcc-plugins/mpxk_builtins.c
 *
 * Defines various helper functions for identifying interesting functions
 * and converting names for wrapper functions.
 *
 * Copyright (C) 2017 Aalto University
 *
 * This file is released under the GPLv2.
 */
#include "mpxk.h"

static int wrapper_i(const char *name);
static int builtin_i(const char *name);

struct mpxk_builtin {
	const bool is_wrapper;
	const bool is_loader;
	const char *name;
};

static const struct mpxk_builtin mpxk_fndecls[] = {
#define MPXK_BUILTIN_DEF(r, x, ...) \
	{ .is_wrapper = 0, .is_loader = 1, .name = #x },
#define MPXK_WRAPPER_DEF(r, x, ...) \
	{ .is_wrapper = 1, .is_loader = 0, .name = MPXK_WRAPPER_PREFIX #x },
#include "mpxk_builtins.def"
#undef MPXK_WRAPPER_DEF
#undef MPXK_BUILTIN_DEF
};

bool mpxk_is_wrapper(const char *name)
{
	gcc_assert(name != NULL);
	return (bool) (wrapper_i(name) >= 0);
}

const char *mpxk_get_wrapper_name(const char *name)
{
	gcc_assert(name != NULL);

	char wr_name[strlen(MPXK_WRAPPER_PREFIX) + strlen(name) + 1];

	sprintf(wr_name, "%s%s", MPXK_WRAPPER_PREFIX, name);

	const int i = wrapper_i(wr_name);

	if (i >= 0)
		return mpxk_fndecls[i].name;

	return NULL;
}

bool mpxk_is_wrappable(const char *name)
{
	gcc_assert(name != NULL);
	return (bool) (mpxk_get_wrapper_name(name) != NULL);
}

static int builtin_i(const char *name)
{
	gcc_assert(name != NULL);

	for (int i = 0; i < ARRAY_SIZE(mpxk_fndecls); i++) {
		gcc_assert(mpxk_fndecls[i].name != NULL);

		if (strcmp(mpxk_fndecls[i].name, name) == 0)
			return i;
	}

	return -1;
}

static int wrapper_i(const char *name)
{
	gcc_assert(name != NULL);
	const int i = builtin_i(name);

	return (i == -1 ? -1 : (mpxk_fndecls[i].is_wrapper ? i : -1));
}

void mpxk_builitins_sanity_check(void)
{
	(void) gcc_version;

	gcc_assert(strcmp(MPXK_WRAPPER_PREFIX "kmalloc",
			  mpxk_get_wrapper_name("kmalloc")) == 0);

	gcc_assert(builtin_i(MPXK_LOAD_BOUNDS_FN_NAME) >= 0);
	gcc_assert(builtin_i("mpxk_wrapper_kmalloc") >= 0);
	gcc_assert(mpxk_is_wrapper("mpxk_wrapper_kmalloc"));
	gcc_assert(mpxk_is_wrapper(MPXK_WRAPPER_PREFIX "kmalloc"));
	gcc_assert(mpxk_is_wrappable("kmalloc"));

	gcc_assert(!mpxk_is_wrapper(MPXK_LOAD_BOUNDS_FN_NAME));
	gcc_assert(builtin_i("kmalloc") < 0);
	gcc_assert(!mpxk_is_wrapper("kmalloc"));
	gcc_assert(builtin_i("mpxk_wrapper_not_good_at_all") < 0);
	gcc_assert(!mpxk_is_wrapper("mpxk_wrapper_not_good_at_all"));
	gcc_assert(!mpxk_is_wrapper("_" MPXK_WRAPPER_PREFIX "kmalloc"));
	gcc_assert(!mpxk_is_wrappable(MPXK_WRAPPER_PREFIX "kmalloc"));
}
