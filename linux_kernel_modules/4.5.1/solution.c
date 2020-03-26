#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/list_sort.h>


#define MYDEV_NAME "solution"

static dev_t first;
static unsigned int count = 1;
static int my_major = 240, my_minor = 0;
static struct cdev *my_cdev;

static struct kobject *my_kobject;
static struct module *this_module = NULL;
static LIST_HEAD(head);


struct modile_names
{
	char name[MODULE_NAME_LEN];
	struct list_head list;
};


int compare(void *priv, struct list_head *a, struct list_head *b)
{
	int i;
	struct modile_names *modile_names_pa;
	struct modile_names *modile_names_pb;
	char symb_a, symb_b; 

	modile_names_pa = list_entry(a, struct modile_names, list); 
	modile_names_pb = list_entry(b, struct modile_names, list); 

	for(i = 0; 1; i++)
	{
		symb_a = *(char *)(modile_names_pa->name + i);
		symb_b = *(char *)(modile_names_pb->name + i);

		if(symb_a > symb_b) return 1;
		if(symb_a < symb_b) return -1;
	}

	return 0;
}

static int solution_open(struct inode *inode, struct file *file)
	{ return 0; }

static int solution_release(struct inode *inode, struct file *file)
	{ return 0; }

static ssize_t solution_read( struct file *file, char __user *buf, size_t lbuf, loff_t *ppos)
	{ return 0; }

static ssize_t solution_write( struct file *file, const char __user *buf, size_t lbuf, loff_t *ppos)
	{ return 0; }

static const struct file_operations mycdev_fops = {
    .owner = THIS_MODULE,
    .read = solution_read,
    .write = solution_write,
    .open = solution_open,
    .release = solution_release
};

#define list_first_entry(ptr, type, member) \
	list_entry((ptr)->next, type, member)

static ssize_t my_sys_show(struct kobject *kobj, struct kobj_attribute *atte, char *buf)
{
	struct modile_names *temp_element_list;
	struct list_head *tmp;

	struct list_head* position ;
	struct module *tmp_module = NULL;
	int ret = 0;

	if(!this_module)
	{
		// this_module = find_module(MYDEV_NAME);
		this_module = THIS_MODULE;
	}

	if(this_module)
	{
		// Create and initialize list of modules' name
		list_for_each(position, THIS_MODULE->list.prev)
		{
			tmp_module = list_entry(position, struct module, list); 

			temp_element_list = kmalloc(sizeof(struct modile_names), GFP_KERNEL);
			INIT_LIST_HEAD(&temp_element_list->list);
			strcpy(temp_element_list->name, tmp_module->name);

			list_add(&temp_element_list->list, &head);
		}

		// Sort the list
		list_sort(NULL, &head, compare);
		

		// Prit sorted list to buffer and erease the list
		list_for_each_safe(position, tmp,&head)
		{
			temp_element_list = list_entry(position, struct modile_names, list); 

			ret += sprintf(buf + ret, "%s\n", temp_element_list->name);

			list_del(&temp_element_list->list);
			kfree(temp_element_list);
		}
	}
		
	return ret;
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

    first = MKDEV(my_major, my_minor);

	retval = register_chrdev_region(first, count, MYDEV_NAME);	
	if(!retval)
	{
		my_cdev = cdev_alloc();

		if(my_cdev == NULL)
		{
			retval = -ENOMEM;
		}
		else
		{
			cdev_init(my_cdev, &mycdev_fops);
			cdev_add(my_cdev, first, count);

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
		}
	}

    return retval;
}

static void __exit cleanup_sulution(void)
{
	kobject_put(my_kobject);

    if(my_cdev)
    cdev_del(my_cdev);

    unregister_chrdev_region(first, count);
}

module_init(init_sulution);
module_exit(cleanup_sulution);

MODULE_LICENSE("GPL");