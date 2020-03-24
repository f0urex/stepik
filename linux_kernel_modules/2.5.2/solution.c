#include <linux/module.h>
#include <linux/slab.h>
#include <linux/device.h>

static int __init init_sulution(void)
{
    return 0;
}

static void __exit cleanup_sulution(void)
{

}

module_init(init_sulution);
module_exit(cleanup_sulution);

MODULE_LICENSE("GPL");