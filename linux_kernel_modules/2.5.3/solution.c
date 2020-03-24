#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>

#define C_ARR_SIZE 5

static int a = 0;
static int b = 0;
static int c[C_ARR_SIZE];

static struct kobject *my_kobject;


module_param(a, int, 0);
module_param(b, int, 0);
module_param_array(c, int, NULL, 0);


static ssize_t my_sys_show(struct kobject *kobj, struct kobj_attribute *atte, char *buf)
{
	int sum = a + b;
	int i;

	for(i = 0; i < C_ARR_SIZE; i++)
	{
		sum += c[i];
	}

	return sprintf(buf, "%d\n", sum);
}

static ssize_t my_sys_store(struct kobject *kobj, struct kobj_attribute *attr,
			 const char *buf, size_t count)
{
	return 0;
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
}

module_init(init_sulution);
module_exit(cleanup_sulution);

MODULE_LICENSE("GPL");