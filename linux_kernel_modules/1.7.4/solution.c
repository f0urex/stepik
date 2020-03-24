#include <linux/module.h>
#include <linux/slab.h>
#include <linux/device.h>

#include "checker.h"

static void *void_ptr = NULL;
static int *int_array_ptr = NULL;
static struct device *struct_device_ptr = NULL;

int init_module(void)
{
	ssize_t void_size = -1, int_array_size = -1;

	// 1
	void_size = get_void_size();
	if( void_size <= 0 )
		return -1;

	void_ptr = kmalloc( void_size, GFP_KERNEL );
	if( void_ptr == NULL )
		return -1;

	submit_void_ptr(void_ptr);

	// 2
	int_array_size = get_int_array_size();
	if( int_array_size <= 0 )
		return -1;

	int_array_ptr = kmalloc( sizeof(int) * int_array_size, GFP_KERNEL );
	if( int_array_ptr == NULL )
		return -1;

	submit_int_array_ptr(int_array_ptr);

	// 3
	struct_device_ptr = kmalloc( sizeof(struct device), GFP_KERNEL );
	if( struct_device_ptr == NULL )
		return -1;

	submit_struct_ptr(struct_device_ptr);

    return 0;
}

void cleanup_module(void)
{
	if( void_ptr != NULL )
		checker_kfree(void_ptr);

	if( int_array_ptr != NULL )
		checker_kfree(int_array_ptr);

	if( struct_device_ptr != NULL )
		checker_kfree(struct_device_ptr);
}

MODULE_LICENSE("GPL");