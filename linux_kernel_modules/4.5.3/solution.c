#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/irq.h>
#include <linux/interrupt.h>


#define RTC_IRQ_N 8
#define IRQ_DEV_NAME "rtc"

static struct kobject *my_kobject;
static size_t counter = 0;
static int dev_id = 12345;

static irqreturn_t rtc_irq_handler(int irq, void *dev_id)
{
	if(irq == RTC_IRQ_N)
	{
		counter++;
		return IRQ_HANDLED;
	}
	
	return IRQ_NONE;
}

static ssize_t my_sys_show(struct kobject *kobj, struct kobj_attribute *atte, char *buf)
{
	return sprintf(buf, "%zu\n", counter);;
}

static ssize_t my_sys_store(struct kobject *kobj, struct kobj_attribute *attr,
			 const char *buf, size_t count)
{
	return count;
}

static struct kobj_attribute my_sys_attribute =
	__ATTR(my_sys, 0664, my_sys_show, my_sys_store);

static struct attribute *attrs[] = {
	&my_sys_attribute.attr,
	NULL,
};

static struct attribute_group attr_group = {
	.attrs = attrs,
};

static int __init init_sulution(void)
{
    int retval;

    retval = request_irq(RTC_IRQ_N, rtc_irq_handler, IRQF_SHARED, IRQ_DEV_NAME, (void*)dev_id);

    if(retval)
    	return retval;

    my_kobject = kobject_create_and_add("my_kobject", kernel_kobj);
	if (!my_kobject)
	{
		retval = -ENOMEM;
	}
	else
	{
		retval = sysfs_create_group(my_kobject, &attr_group);
		if (retval)
			kobject_put(my_kobject);
	}

    return retval;
}

static void __exit cleanup_sulution(void)
{
	kobject_put(my_kobject);

	free_irq(RTC_IRQ_N, (void*)dev_id);
}

module_init(init_sulution);
module_exit(cleanup_sulution);

MODULE_LICENSE("GPL");