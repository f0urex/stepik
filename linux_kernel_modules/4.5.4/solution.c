#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/ktime.h>
#include <linux/hrtimer.h>

#include "checker.h"

#define MAX_NUM_TIMERS 256

static int delays[MAX_NUM_TIMERS];
static int n_timers;
static int cur_delay = 0;
module_param_array(delays, int, &n_timers, 0);

static bool timerExpired;
struct hrtimer timer;

static enum hrtimer_restart timer_callback(struct hrtimer *val)
{
	ktime_t period;
	check_timer();

	cur_delay++;

	if(cur_delay >= n_timers)
		return HRTIMER_NORESTART;

	period = ktime_set(delays[cur_delay]/1000, delays[cur_delay]%1000*(unsigned long)1000*(unsigned long)1000);
	hrtimer_forward_now(&timer, period);

	return HRTIMER_RESTART;
}

static int __init init_solution(void)
{
    int retval = 0, i;
    ktime_t period;

    hrtimer_init(&timer, CLOCK_REALTIME, HRTIMER_MODE_REL);
    timer.function = timer_callback;

	period = ktime_set(delays[i]/1000, delays[i]%1000*(unsigned long)1000*(unsigned long)1000);
	check_timer();
	hrtimer_start(&timer, period, HRTIMER_MODE_REL);   	

    return retval;
}

static void __exit cleanup_solution(void)
{
	hrtimer_cancel(&timer);
}

module_init(init_solution);
module_exit(cleanup_solution);

MODULE_LICENSE("GPL");