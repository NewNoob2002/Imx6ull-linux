#include <linux/module.h>
#include <linux/fs.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/gpio.h>
#include <linux/uaccess.h>
#include <linux/string.h>
#include <linux/interrupt.h>
#include <linux/irqreturn.h>
#include <linux/of_gpio.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/timer.h>
#include <linux/gpio/consumer.h>


static int major;
static struct class * class;
static int irq;
static struct timer_list timer;
static struct gpio_desc *sr501_desc;
static int sr501_gpio;


struct fasync_struct *sr_fasync;


static void timeout_handler(unsigned long data)
{
	int res;
	res = gpiod_get_value(sr501_desc);
	kill_fasync(&sr_fasync, SIGIO, POLL_IN);   //异步通知
//	if(res)
//		printk("have people\n");
//	else
//		printk("no people\n");
}


static irqreturn_t sr501_handler(int irq, void *dev_id)
{
//	printk("sr501_handler\n");
	mod_timer(&timer, jiffies+HZ/50);
	return IRQ_HANDLED; 
}

static int sr501_probe(struct platform_device *pdev)
{
	int res;
//	struct device *dev = &pdev->dev;
//	/* get gpio from device tree */
//	sr501_desc = gpiod_get(dev, "sr501", GPIOD_OUT_LOW);
//	sr501_gpio = desc_to_gpio(sr501_desc);

	struct device_node *node = pdev->dev.of_node;
	sr501_gpio = of_get_gpio(node, 0);
	sr501_desc = gpio_to_desc(sr501_gpio);

	/* gpio to irq && request irq */
	irq = gpio_to_irq(sr501_gpio); 
	res = request_irq(irq, sr501_handler, 
					 IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, "sr501_irq", NULL);
	if(res){		
		printk("request_irq failed\n");
		return -1;
	}
	printk("request_irq success\n");

	setup_timer(&timer, timeout_handler, (unsigned long)NULL);
	timer.expires = 0;
	add_timer(&timer);
	
	return 0;
}

static int sr501_remove(struct platform_device *dev)
{
	gpiod_put(sr501_desc);
	free_irq(irq, NULL);
	return 0;
}

static const struct of_device_id sr501_of_match[] = {
	{ .compatible = "hc-sr501" },
	{ }
};

static struct platform_driver sr501_platform_driver = {
	.driver = {
		.name		= "my_sr501",
		.of_match_table	= sr501_of_match,
	},
	.probe			= sr501_probe,
	.remove			= sr501_remove,
};

static int sr501_open (struct inode *node, struct file *filp)
{
	return 0;
}

static ssize_t sr501_read (struct file *filp, char __user *buf, size_t size, loff_t *offset)
{
	char val;
	int res;
	val = gpiod_get_value(sr501_desc);
	res = copy_to_user(buf, &val, 1);
//	printk("sr501 read\n");
	return 1;
}

static int sr501_release (struct inode *node, struct file *filp)
{
	return 0;
}

static int sr501_fasync (int fd, struct file *filp, int on)
{
	int retval;
	retval = fasync_helper(fd, filp, on, &sr_fasync);
	if (retval < 0)
		return retval;

	return 0;
}


static struct file_operations sr501_ops = {
	.owner		=	THIS_MODULE,
	.open 		= 	sr501_open,
	.release 	=	sr501_release,
	.fasync     =   sr501_fasync,
	.read       =   sr501_read,
};

static int sr501_init(void)
{
	major = register_chrdev(0 , "sr501", &sr501_ops);
	class = class_create(THIS_MODULE, "sr501_class");
	device_create(class, NULL, MKDEV(major, 0), NULL, "sr501");

	platform_driver_register(&sr501_platform_driver);
	
	return 0;
}

static void sr501_exit(void)
{
	del_timer(&timer);
	platform_driver_unregister(&sr501_platform_driver);
	
	device_destroy(class, MKDEV(major, 0));
	class_destroy(class);
	unregister_chrdev(major, "sr501");
}

module_init(sr501_init);
module_exit(sr501_exit);
MODULE_LICENSE("GPL");

