#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/init.h>


static struct kobject *my_kobject;

/*
 * An unnamed attribute group will put all of the attributes directly in
 * the kobject directory.  If we specify a name, a subdirectory will be
 * created for the attributes with the directory being the name of the
 * attribute group.
 */
static struct attribute_group attr_group = {
	.attrs = attrs,
};

static int __init init_sulution(void)
{
    int retval;

    my_kobject = kobject_create_and_add("kobject_example", kernel_kobj);
	if (!my_kobject)
	{
		retval = -ENOMEM;
	}
	else
	{

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