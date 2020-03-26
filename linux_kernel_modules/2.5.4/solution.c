#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/cdev.h>

#define MYDEV_NAME "solution"
#define KBUF_SIZE (size_t)( (10) * PAGE_SIZE )

static dev_t first;
static unsigned int count = 1;
static int my_major = 240, my_minor = 0;
static struct cdev *my_cdev;

static size_t open_count = 0;
static size_t wrote_bytes = 0;

static int solution_open(struct inode *inode, struct file *file)
{
    open_count++;
    return 0;
}

static int solution_release(struct inode *inode, struct file *file)
{
    return 0;
}

static ssize_t solution_read( struct file *file, char __user *buf, size_t lbuf, loff_t *ppos)
{   
	static int eof_flag = 0;
	
	if(eof_flag == 0)
	{
		eof_flag = 1;
		return sprintf(buf, "%zu %zu\n", open_count, wrote_bytes);
	}
	else
	{
		eof_flag = 0;
		return -1;
	}
}

static ssize_t solution_write( struct file *file, const char __user *buf, size_t lbuf, loff_t *ppos)
{
	wrote_bytes += lbuf;
    return lbuf;
}

static const struct file_operations mycdev_fops = {
    .owner = THIS_MODULE,
    .read = solution_read,
    .write = solution_write,
    .open = solution_open,
    .release = solution_release
};

static int __init init_solution(void)
{
	int ret = 0;

	first = MKDEV(my_major, my_minor);	

	ret = register_chrdev_region(first, count, MYDEV_NAME);	
	if(!ret)
	{
		my_cdev = cdev_alloc();

		if(my_cdev == NULL)
		{
			ret = -ENOMEM;
		}
		else
		{
			cdev_init(my_cdev, &mycdev_fops);
			cdev_add(my_cdev, first, count);
		}
	}
	
	return ret;
}

static void __exit cleanup_solution(void)
{
    if(my_cdev)
        cdev_del(my_cdev);

    unregister_chrdev_region(first, count);
}

module_init(init_solution);
module_exit(cleanup_solution);

MODULE_LICENSE("GPL");