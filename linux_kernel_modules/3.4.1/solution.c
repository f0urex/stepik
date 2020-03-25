#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>



#define MYDEV_NAME "solution"
#define NODE_NAME_MAX_LEN 256

static char node_name[NODE_NAME_MAX_LEN];
module_param_string(node_name, node_name, sizeof(node_name), 0);

static struct class *node_name_class;

static dev_t first;
static unsigned int count = 1;
static int my_major = 0;
static struct cdev *my_cdev;




static int solution_open(struct inode *inode, struct file *file)
{
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
		return sprintf(buf, "%u\n", my_major);
	}
	else
	{
		eof_flag = 0;
		return -1;
	}
}

static ssize_t solution_write( struct file *file, const char __user *buf, size_t lbuf, loff_t *ppos)
{
    return 0;
}

static const struct file_operations mycdev_fops = {
    .owner = THIS_MODULE,
    .read = solution_read,
    .write = solution_write,
    .open = solution_open,
    .release = solution_release
};

static int __init init_sulution(void)
{
    int ret = 0;
      
    ret = alloc_chrdev_region(&first, 0, count, node_name);
    if(!ret)
    { 
        my_major = MAJOR(first);

        my_cdev = cdev_alloc();

        if(my_cdev == NULL)
        {
            unregister_chrdev_region(first, count);
            ret = -ENOMEM;
        }
        else
        {
            cdev_init(my_cdev, &mycdev_fops);
            cdev_add(my_cdev, first, count);
        }    
 
        node_name_class = class_create(THIS_MODULE, MYDEV_NAME);
        if (IS_ERR(node_name_class))
        {
            unregister_chrdev_region(first, count);
            return PTR_ERR(node_name_class);
        }
 
        device_create(node_name_class, NULL, first, "%s", node_name);
    }

    return ret;
}

static void __exit cleanup_sulution(void)
{

    device_destroy(node_name_class, first);

    class_destroy(node_name_class);


    if(my_cdev)
        cdev_del(my_cdev);

    unregister_chrdev_region(first, count);
}


module_init(init_sulution);
module_exit(cleanup_sulution);

MODULE_LICENSE("GPL");