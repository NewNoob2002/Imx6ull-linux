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


static int major;
static struct class * class;
//static int irq;
static struct gpio_desc *dht11_desc;
static int dht11_gpio;

static DECLARE_WAIT_QUEUE_HEAD(waitqueue);
static int status = 0;
static int index = 0;              //记录收到dth11多少个中断
static u64 dht11_data_time[100];    //保存dht11每次中断的时刻
//static int dht11_data_value[100];    //保存dht11每次中断时的电平
static int result;

static struct timer_list dht11_timer;

static int DHT11_PIN;
static u8 data[5] = {0};

static int read_dht11_data(void);


static void dht11_timeout(unsigned long arg)
{
	int res;
	printk("recv a data ok, index = %d\n", index);
	
	res = read_dht11_data();
	if(res == 0)
		result = 2;    //表示数据正确
	else
		result = 0;    //表示数据不正确
	index = 0;     //中断记录数清零
	/* 唤醒read函数 */
	status = 1;
	wake_up_interruptible(&waitqueue);
}

static irqreturn_t dht11_handler(int irq, void *dev_id)
{
//	unsigned long flags;

	/* 记录中断时刻 */
	dht11_data_time[index] = ktime_get_ns();
	++index;

//	local_irq_save(flags);	  //保存中断状态，关闭所有中断
//	mod_timer(&dht11_timer, jiffies + 10);  //定时10ms
//	local_irq_restore(flags); 打开中断，恢复关闭前中断状

	return IRQ_HANDLED; 
}

static int dht11_probe(struct platform_device *pdev)
{
	/* get gpio_pin from device tree */
	struct device_node *node = pdev->dev.of_node;
	dht11_gpio = of_get_gpio(node, 0);
 	dht11_desc = gpio_to_desc(dht11_gpio);
	DHT11_PIN = dht11_gpio;
	printk("DHT11_PIN = %d\n", DHT11_PIN );

	gpiod_direction_output(dht11_desc, 1);

	printk("dht11 probe run\n");
	
	return 0;
}

static int dht11_remove(struct platform_device *dev)
{
	return 0;
}

static const struct of_device_id dht11_of_match[] = {
	{ .compatible = "hc-dht11" },
	{ }
};

static struct platform_driver dht11_platform_driver = {
	.driver = {
		.name		= "my_dht11",
		.of_match_table	= dht11_of_match,
	},
	.probe			= dht11_probe,
	.remove			= dht11_remove,
};


static int dht11_open (struct inode *node, struct file *filp)
{
	
	return 0;
}

static int read_dht11_data(void)
{
//	unsigned char data[5] = {0};
//	unsigned char byte = 0;
//	int i, cnt = 0;
//	u64 high_time;
//	unsigned char crc;
//	if(index < 81)
//		return 1;
//	for(i = index - 81; i < index; i+=2)
//	{
//		/* 只计算高电平时间即可，后面有校验位 */
//		high_time = dht11_data_time[i+1] - dht11_data_time[i] ;
//		printk("high_time = %lld\n", high_time);
//		byte <<= 1;
//		/* 如果高电平时间大于60us = 60000ns ，就是 1*/
//		if(high_time > 60000)
//			byte |= 1;	
//		cnt++;
//		/* 已经读了8位了 */
//		if(cnt > 0 && cnt % 8 == 0){
//			data[cnt/8 - 1] = byte;
//			byte = 0;
//			printk("i = %d, data = 0x%02x\n\n", i, data[cnt/8 - 1]);
//		}
//	}
//	crc = data[0] + data[1] + data[2] + data[3];
//	printk(KERN_INFO "DHT11 Temperature: %d°C, Humidity: %d%%\n", data[2], data[0]);
//	if(crc != data[4]){
//		printk("error\n");
//		return 1;
//	}
//
//	return 0;

    u8 checksum = 0;
    int i;

    gpio_direction_output(DHT11_PIN, 0);
    mdelay(18);
    gpio_direction_input(DHT11_PIN);

    gpio_set_value(DHT11_PIN, 1);
    udelay(40);
    gpio_direction_input(DHT11_PIN);

    if (gpio_get_value(DHT11_PIN)) {
        printk(KERN_ERR "DHT11 sensor not responding\n");
        return 1;
    }

    udelay(80);

    if (!gpio_get_value(DHT11_PIN)) {
        printk(KERN_ERR "DHT11 sensor not responding\n");
        return 1;
    }

    udelay(80);

    for (i = 0; i < 5; i++) {
        int j;

        for (j = 7; j >= 0; j--) {
            u32 bit;

            while (!gpio_get_value(DHT11_PIN));
            udelay(50);

            bit = gpio_get_value(DHT11_PIN);
         	
            if (bit){
                data[i] |= (1 << j);
				udelay(40);
			}
        }
    }

    checksum = data[0] + data[1] + data[2] + data[3];

    if (checksum != data[4]) {
//        printk(KERN_ERR "DHT11 data checksum error\n");
//		printk(KERN_INFO "DHT11 Temperature: %d°C, Humidity: %d%%\n", data[2], data[0]);
        return 1;
    }
//	printk(KERN_INFO "DHT11 Temperature: %d°C, Humidity: %d%%\n", data[2], data[0]);
	return 0;

}


static ssize_t dht11_read (struct file *filp, char __user *buf, size_t size, loff_t *offset)
{
	int res;
	unsigned long flags;
 
	/* 因为DHT11的时序要求很高，所以在读温湿度的时候要让代码进入临界区，防止内核调度和抢占 */
    local_irq_save(flags);  
    res = read_dht11_data();
    local_irq_restore(flags); 

//	if(res == 0){
//		printk(KERN_INFO "DHT11 Temperature: %d°C, Humidity: %d%%\n", data[2], data[0]);
		res = copy_to_user(buf, data, size);
		return 0;
//	}


//	int minor;
//	struct inode * node = filp->f_inode;
//	minor = iminor(node);

	/* 0.设置定时器，判断数据接收完毕，为了对后面接收中断影响小一些，把定时器初始化放前面 */
//	setup_timer(&dht11_timer, dht11_timeout, (unsigned long)NULL);
//	dht11_timer.expires = jiffies + 5;
//	add_timer(&dht11_timer);
	
	/* 1.拉低总线 > 18ms */
//	res = gpio_request(dht11_gpio, "dth11");
//	gpiod_direction_output(dht11_desc, 0);
//	mdelay(18);
//
//	/* 2.设置为输入方向 */
//	gpiod_direction_input(dht11_desc);	

	/* 3.释放GPIO */
//	gpio_free(dht11_gpio);
//
//	/* 5.申请中断 */
//	irq = gpio_to_irq(dht11_gpio);
//	res = request_irq(irq, dht11_handler, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, "dht11_irq", NULL);
//	if(res){		
//		printk("request_irq failed\n");
//		return -1;
//	}

	/* 6.休眠等待数据 */
//	wait_event_interruptible(waitqueue, status);
//	printk("wake up\n");
//	status = 0;
//
//	del_timer(&dht11_timer);
//	free_irq(irq, NULL);

	return 1;
}

static int dht11_release (struct inode *node, struct file *filp)
{
	return 0;
}


static struct file_operations dht11_ops = {
	.owner		=	THIS_MODULE,
	.open 		= 	dht11_open,
	.read 		= 	dht11_read,
	.release 	=	dht11_release,
};

static int dht11_init(void)
{
	major = register_chrdev(0 , "dht11", &dht11_ops);
	class = class_create(THIS_MODULE, "dht11_class");
	device_create(class, NULL, MKDEV(major, 0), NULL, "dht11");

	platform_driver_register(&dht11_platform_driver);
	
	return 0;
}

static void dht11_exit(void)
{
	platform_driver_unregister(&dht11_platform_driver);
	
	device_destroy(class, MKDEV(major, 0));
	class_destroy(class);
	unregister_chrdev(major, "dht11");
}

module_init(dht11_init);
module_exit(dht11_exit);
MODULE_LICENSE("GPL");

