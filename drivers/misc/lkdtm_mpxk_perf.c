#undef pr_fmt
#include <linux/printk.h>
#include <linux/string.h>
#include <linux/time64.h>
#include <linux/ktime.h>
#include <linux/random.h>
#include <linux/timekeeping.h>
#include <linux/slab.h>

#define ITERATIONS 10000

struct counter {
	unsigned long total;
	int count;
};

struct counter counter_legacy = { .total = 0, .count = 0 };
struct counter counter_got_bounds = { .total = 0, .count = 0 };
struct counter counter_load_bounds = { .total = 0, .count = 0 };

struct timespec64 mpxk_bench_start = { .tv_sec = 0, .tv_nsec = 0 };
struct timespec64 mpxk_bench_end = { .tv_sec = 0, .tv_nsec = 0 };

#define save_time(x) getrawmonotonic64(x)
#define time_start() (mpxk_bench_start.tv_nsec)
#define time_end() (mpxk_bench_end.tv_nsec)
#define time_diff() (time_end() - time_start())

#define start_timer() do {		\
	preempt_disable();		\
	save_time(&mpxk_bench_start);	\
} while(0)

#define end_timer(x) do {			\
	save_time(&mpxk_bench_end);		\
	preempt_enable();			\
	if (time_diff() > time_end()) {		\
		pr_info("timer failed!\n");	\
	} else {				\
		x.total += time_diff(); 	\
		x.count += 1;			\
	}					\
} while(0)

noinline __attribute__((bnd_legacy))
void memcpy_legacy(void *dst, void *src, size_t size)
{
	memcpy(dst, src, size);
}

noinline
void memcpy_got_bounds(void *dst, void *src, size_t size)
{
	memcpy(dst, src, size);
}

noinline __attribute__((bnd_legacy))
void *kmalloc_legacy(size_t size)
{
	return kmalloc(size, GFP_NOWAIT | __GFP_NORETRY);
}

noinline
void *kmalloc_bounds(size_t size)
{
	return kmalloc(size, GFP_NOWAIT | __GFP_NORETRY);
}

noinline
void memcpy_load_bounds(void **arr, size_t size)
{
	/* need to check bounds of array! */
	memcpy(arr[0], arr[1], size);
}

noinline
static void *generate_data(size_t size)
{
	void *ptr;

	do {
		ptr = kmalloc(size, GFP_KERNEL);

		if (ptr == NULL) {
			pr_info("Got a NULL pointer from malloc!\n");
			break;
		}

		get_random_bytes(ptr, size);
		if (ptr == NULL) {
			pr_info("get_random_bytes set pointer to NULL :(\n");
			break;
		}
	} while(0);

	return ptr;
}

noinline
static void memcpy_time(void *dst1, void *dst2, void **arr, int size)
{
	static int counter = 0;
	int i;
	void *src;

	for (i = 0; i < 3; i++) {
		switch((i+counter) % 3) {
		case 0:
			if (dst1 == NULL) {
				pr_info("memcpy dst1 is NULL\n");
				break;
			}

			src = generate_data(size);

			if (src == NULL) {
				pr_info("failed to kmalloc test data\n");
				break;
			}

			start_timer();
			memcpy_legacy(dst1, src, size);
			end_timer(counter_legacy);

			kfree(src);
			break;
		case 1:
			if (dst1 == NULL) {
				pr_info("memcpy dst2 is NULL\n");
				break;
			}

			src = generate_data(size);

			if (src == NULL) {
				pr_info("failed to kmalloc test data\n");
				break;
			}

			start_timer();
			memcpy_got_bounds(dst2, src, size);
			end_timer(counter_got_bounds);

			kfree(src);
			break;
		case 2:
			if (arr[0] == NULL) {
				pr_info("memcpy arr[0] is NULL\n");
				break;
			}

			arr[1] = generate_data(size);

			if (arr[1] == NULL) {
				pr_info("failed to kmalloc test data\n");
				break;
			}

			start_timer();
			memcpy_load_bounds(arr, size);
			end_timer(counter_load_bounds);

			kfree(arr[1]);
			break;
		}
	}
	counter++;
}

noinline
static void memcpy_one_run(const size_t size)
{
	void *arr[10];
	void *dst1;
	void *dst2;

	do {
		dst1 = kmalloc(size, GFP_KERNEL);
		dst2 = kmalloc(size, GFP_KERNEL);
		arr[0] = kmalloc(size, GFP_KERNEL);

		if (dst1 == NULL || dst2 == NULL || arr[0] == NULL) {
			pr_info("Failed to allocated test dst memory of %lu size\n", size);
			break;
		}

		memcpy_time(dst1, dst2, arr, size);
	} while(0);

	if (dst1 != NULL)
		kfree(dst1);
	if (dst2 != NULL)
		kfree(dst2);
	if (arr[0] != NULL)
		kfree(arr[0]);
}

noinline void clear_pointer(void *ptr) {
	kfree(ptr);
}

noinline void kmalloc_one_run(const size_t size)
{
	static int counter = 0;
	int i;
	void *src;

	for (i = 0; i < 2; i++) {
		switch((i+counter) % 2) {
		case 0:
			start_timer();
			src = kmalloc_legacy(size);

			if (src != NULL) {
				end_timer(counter_legacy);
				clear_pointer(src);
			} else {
				pr_info("kmalloc failed\n");
			}

			break;
		case 1:
			start_timer();
			src = kmalloc_bounds(size);
			if (src != NULL) {
				end_timer(counter_got_bounds);
				clear_pointer(src);
			} else {
				pr_info("kmalloc failed\n");
			}

			break;
		}
	}
	counter++;
}


void lkdtm_MPXK_PERF(void)
{
	int i, j;

	size_t sizes[] = { sizeof(int), 256, 65336, 1048576 };

	for (i = 0; i < ARRAY_SIZE(sizes); i++) {
		for (j = 0; j < ITERATIONS; j++) {
			memcpy_one_run(sizes[i]);
		}

		pr_info("memcpy(size=%lu)=>(legacy=%lu(%d),got_bounds=%lu(%d),load_bounds=%lu(%d))\n",
				sizes[i],
				counter_legacy.total, counter_legacy.count,
				counter_got_bounds.total, counter_got_bounds.count,
				counter_load_bounds.total, counter_load_bounds.count);

		counter_legacy.total = 0;
		counter_legacy.count = 0;
		counter_got_bounds.total = 0;
		counter_got_bounds.count = 0;
		counter_load_bounds.total = 0;
		counter_load_bounds.count = 0;
	}

	for (i = 0; i < ARRAY_SIZE(sizes); i++) {
		for (j = 0; j < ITERATIONS; j++) {
			kmalloc_one_run(sizes[i]);
		}

		pr_info("kmalloc(size=%lu)=>(legacy=%lu(%d),got_bounds=%lu(%d))\n",
				sizes[i],
				counter_legacy.total, counter_legacy.count,
				counter_got_bounds.total, counter_got_bounds.count);

		counter_legacy.total = 0;
		counter_legacy.count = 0;
		counter_got_bounds.total = 0;
		counter_got_bounds.count = 0;
		counter_load_bounds.total = 0;
		counter_load_bounds.count = 0;
	}
}
