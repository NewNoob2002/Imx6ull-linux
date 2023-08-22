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
#include <linux/delay.h>
#include <linux/timekeeping.h>
#include <linux/wait.h>
#include <linux/irqflags.h>
#include <linux/pwm.h>

static int major;
static struct class *class;

static struct pwm_device *pwm_test;

static int sg90_probe(struct platform_device *pdev)
{
    struct device_node *node = pdev->dev.of_node;

    printk("sg90 match success \n");
    if (node){
        /* 从子节点中获取PWM设备 */
        pwm_test = devm_of_pwm_get(&pdev->dev, node, NULL);  
        if (IS_ERR(pwm_test)){
            printk(KERN_ERR" pwm_test,get pwm  error!!\n");
            return -1;
        }
    }
    else{
        printk(KERN_ERR" pwm_test of_get_next_child  error!!\n");
        return -1;
    }

    pwm_config(pwm_test, 500000, 20000000);   /* 配置PWM：0.5ms，0度，周期：20000000ns = 20ms */
    pwm_set_polarity(pwm_test, PWM_POLARITY_NORMAL); /* 设置输出极性：占空比为高电平 */
    pwm_enable(pwm_test);    /* 使能PWM输出 */

    return 0;
}

static int sg90_remove(struct platform_device *dev)
{
	pwm_config(pwm_test, 500000, 20000000);  /* 配置PWM：0.5ms，0度 */
	pwm_free(pwm_test);

	return 0;
}

static const struct of_device_id sg90_of_match[] = {
	{ .compatible = "hc-sg90" },
	{ }
};

static struct platform_driver sg90_platform_driver = {
	.driver = {
		.name		= "my_sg90",
		.of_match_table	= sg90_of_match,
	},
	.probe			= sg90_probe,
	.remove			= sg90_remove,
};


static int sg90_open (struct inode *node, struct file *filp)
{
	
	return 0;
}

static ssize_t sg90_write (struct file *filp, const char __user *buf, size_t size, loff_t *offset)
{
	int res;
	unsigned char data[1];
	if(size != 1)
		return 1;

	res = copy_from_user(data, buf, size);
	/* 配置PWM：旋转任意角度(单位1度) */
	pwm_config(pwm_test, 500000 + data[0] * 100000 / 9, 20000000);   
	return 1;
}

static int sg90_release (struct inode *node, struct file *filp)
{
	return 0;
}


static struct file_operations sg90_ops = {
	.owner		=	THIS_MODULE,
	.open 		= 	sg90_open,
	.write 		= 	sg90_write,
	.release 	=	sg90_release,
};

static int sg90_init(void)
{
	major = register_chrdev(0 , "sg90", &sg90_ops);
	class = class_create(THIS_MODULE, "sg90_class");
	device_create(class, NULL, MKDEV(major, 0), NULL, "sg90");

	platform_driver_register(&sg90_platform_driver);
	
	return 0;
}

static void sg90_exit(void)
{
	platform_driver_unregister(&sg90_platform_driver);
	
	device_destroy(class, MKDEV(major, 0));
	class_destroy(class);
	unregister_chrdev(major, "sg90");
}

module_init(sg90_init);
module_exit(sg90_exit);
MODULE_LICENSE("GPL");

