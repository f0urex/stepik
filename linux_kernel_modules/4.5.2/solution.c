#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/cdev.h>


#define MYDEV_NAME "solution"
#define IOC_MAGIC 'k'

#define SUM_LENGTH _IOWR(IOC_MAGIC, 1, char*)
#define SUM_CONTENT _IOWR(IOC_MAGIC, 2, char*)

static dev_t first;
static unsigned int count = 1;
static int my_major = 240, my_minor = 0;
static struct cdev *my_cdev;

static size_t sum_lenght = 0;
static size_t sum_content = 0;

static int solution_open(struct inode *inode, struct file *file)
	{ return 0; }

static int solution_release(struct inode *inode, struct file *file)
	{ return 0; }

static ssize_t solution_read( struct file *file, char __user *buf, size_t lbuf, loff_t *ppos)
	{ return sprintf(buf, "%zu %zu\n", sum_lenght, sum_content); }

static ssize_t solution_write( struct file *file, const char __user *buf, size_t lbuf, loff_t *ppos)
	{ return 0; }

static long solution_unlocked_ioctl(struct file *f, unsigned int cmd , unsigned long arg)
{
	int ret = 0;
	long num;

	if((_IOC_TYPE(cmd) != IOC_MAGIC)) 
		return -ENOTTY;
	 
	switch(cmd)
	{
		case SUM_LENGTH:
			sum_lenght += strlen((char*)arg);
			ret = sum_lenght;
			break;
		case SUM_CONTENT:
			kstrtol((char*)arg, 10, &num);
			sum_content += num;
			ret = sum_content;
			break;
		default: 
	 	return -ENOTTY;	
	}

	return ret;
}

static const struct file_operations mycdev_fops = {
    .owner = THIS_MODULE,
    .read = solution_read,
    .write = solution_write,
    .open = solution_open,
    .release = solution_release,
    .unlocked_ioctl = solution_unlocked_ioctl,
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
		}
	}

    return retval;
}

static void __exit cleanup_sulution(void)
{
    if(my_cdev)
    cdev_del(my_cdev);

    unregister_chrdev_region(first, count);
}

module_init(init_sulution);
module_exit(cleanup_sulution);

MODULE_LICENSE("GPL");