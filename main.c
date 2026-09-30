#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/atomic.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

#define PROC_NAME   "atomic_counter"
#define MAXLEN  16

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A simple atomic counter");

static atomic64_t counter = ATOMIC64_INIT(0);
static struct proc_dir_entry *proc_entry;

static int counter_display(struct seq_file *m, void *v)
{
	s64 val = atomic64_inc_return(&counter);

	seq_printf(m, "counter = %lld\n", val);
	return 0;
}

static int counter_open(struct inode *inode, struct file *file)
{
	return single_open(file, counter_display, NULL);
}

static ssize_t counter_write(struct file *file,
		const char __user *ubuf,
		size_t count, loff_t *ppos)
{
	char cmd[MAXLEN];
	size_t len = min(count, sizeof(cmd) - 1);

	if (copy_from_user(cmd, ubuf, len))
		return -EFAULT;

	cmd[len] = '\0';

	if (len > 0 && cmd[len - 1] == '\n')
		cmd[len - 1] = '\0';

	if (strcmp(cmd, "reset") == 0) {
		atomic64_set(&counter, 0);
		pr_info("Counter reset to 0\n");
	} else {
		pr_warn("Unknown Command'%s'\n", cmd);
		return -EINVAL;
	}

	return count;
}

static const struct proc_ops counter_fops = {
	.proc_open    = counter_open,
	.proc_read    = seq_read,
	.proc_lseek   = seq_lseek,
	.proc_release = single_release,
	.proc_write   = counter_write,
};

static int __init proc_counter_init(void)
{
	proc_entry = proc_create(PROC_NAME, 0666, NULL, &counter_fops);
	if (!proc_entry) {
		pr_err("counter: failed to create /proc/%s\n", PROC_NAME);
		return -ENOMEM;
	}

	atomic64_set(&counter, 0);
	pr_info("counter: /proc/%s created\n", PROC_NAME);
	return 0;
}

static void __exit proc_counter_exit(void)
{
	proc_remove(proc_entry);
	pr_info("counter: /proc/%s removed, final value=%lld\n",
			PROC_NAME, (s64)atomic64_read(&counter));
}

module_init(proc_counter_init);
module_exit(proc_counter_exit);
