// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause
#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/list.h>
#include <linux/ktime.h>
#include <linux/slab.h>

MODULE_AUTHOR("Maksym Hushcha <maksymgushcha20@gmail.com>");
MODULE_DESCRIPTION("Hello, world in Linux Kernel Training");
MODULE_LICENSE("Dual BSD/GPL");

static uint count = 1;
module_param(count, uint, 0444);
MODULE_PARM_DESC(count, "Number of times to print 'Hello, world!' message");

struct my_list_item {
	struct list_head list;
	ktime_t time;
};

static LIST_HEAD(my_list);

static int __init hello_init(void)
{
	uint i;
	int ret = 0;

	if (count == 0 || (count >= 5 && count <= 10))
		pr_warn("Warning: count is 0 or between 5 and 10\n");

	if (count > 10) {
		pr_err("Error: count is greater than 10\n");
		ret = -EINVAL;
		goto out;
	}

	for (i = 0; i < count; i++) {
		struct my_list_item *item;

		item = kmalloc(sizeof(*item), GFP_KERNEL);
		if (!item) {
			ret = -ENOMEM;
			goto out;
		}

		item->time = ktime_get();
		list_add_tail(&item->list, &my_list);

		pr_emerg("Hello, world!\n");
	}

out:
	if (ret != 0) {
		struct my_list_item *cursor, *tmp;

		list_for_each_entry_safe(cursor, tmp, &my_list, list) {
			list_del(&cursor->list);
			kfree(cursor);
		}
	}

	return ret;
}

static void __exit hello_exit(void)
{
	struct my_list_item *item, *tmp;

	list_for_each_entry_safe(item, tmp, &my_list, list) {
		pr_info("Time: %lld ns\n", ktime_to_ns(item->time));
		list_del(&item->list);
		kfree(item);
	}
}

module_init(hello_init);
module_exit(hello_exit);

