#include <linux/module.h>
#include <linux/fs.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/gpio.h>
#include <linux/uaccess.h>
#include <linux/string.h>

int major;				//设备号
static struct class *my_dev_class;
int dev_cnt;
char dev_names[10][20]={};    //保存设备树中的名字，用于分辨设备
struct gpio_desc *my_dev_gpio[10];  //保存GPIO描述符
//int gpio_s[10];  			  //保存GPIO子系统号，用于分辨设备


/*=============================file_operations ==============================*/
static ssize_t my_drv_read (struct file *filp, char __user *buf, size_t size, loff_t *offset)
{
	struct inode *inode = file_inode(filp);
	int minor = iminor(inode);
	char status;
	int err;

	status = gpiod_get_value(my_dev_gpio[minor]);  /* 物理电平值 */

	err = copy_to_user(buf, &status, 1);
	
//	printk("drv_read function run.... status = %d\n", status );
	return 1;
}

static ssize_t my_drv_write (struct file *filp, const char __user *buf, size_t size, loff_t *offset)
{
	struct inode *inode = file_inode(filp);
	int minor = iminor(inode);
	char status;
	int err;
	err = copy_from_user(&status, buf, 1);

	gpiod_set_value(my_dev_gpio[minor], status);  /* 逻辑值 */
	
//	printk("drv_write function run.... status = %d\n", status);
	
	return 0;
}

static int my_drv_open (struct inode *node, struct file *filp)
{
//	struct inode *inode = file_inode(filp);
//	int minor = iminor(inode);

	printk("drv_open function run....\n");		
	
	
//	gpiod_set_value(my_dev_gpio[minor], 0);
	return 0;
}

static int my_drv_release (struct inode *node, struct file *filp)
{
	printk("drv_release function run....\n");
	return 0;
}

/* operations结构体：为应用层提供驱动接口 */
static struct file_operations my_dev_ops = {
	.owner		= 	THIS_MODULE,
	.read 		=	my_drv_read,
	.write		=	my_drv_write,
	.open		=	my_drv_open,
	.release	=	my_drv_release,
};


/*=============================platform_driver==============================*/
/*  如果匹配到了内核根据设备树生成的platform_device，
	该函数会被调用，如果有多个匹配的设备节点，该函数
	会被多次调用
*/
static int my_probe(struct platform_device *pdev)
{
	/*  从内核根据设备树生成的 platform_device 
		结构体中获取到设备节点
	*/
	struct device *dev = &pdev->dev;
	struct device_node *np = dev->of_node;
	
//	int gpio_pin;
	char a[20];
	const char *my_name = a;
	
	of_property_read_string(np, "my_name", &my_name);
	//保存设备的名字
	strcpy(dev_names[dev_cnt], my_name);
	
	//从设备树中获取GPIO引脚，并设置成默认输出模式，无效(LOW表示无效逻辑)
	my_dev_gpio[dev_cnt] = gpiod_get(dev, my_name, GPIOD_OUT_LOW);

	gpio_direction_output(desc_to_gpio(my_dev_gpio[dev_cnt]), 1);
	
	//从struct desc结构体转成GPIO子系统标号
//	gpio_pin = desc_to_gpio(my_dev_gpio[dev_cnt]);
//	gpio_s[dev_cnt] = gpio_pin;   //保存GPIO号

	//创建设备节点 /dev/xxx
	device_create(my_dev_class, NULL, MKDEV(major, dev_cnt), NULL, my_name);
	dev_cnt++;

	printk("my_probe run, my_name = %s\n", my_name);
	
	return 0;
}

static int my_remove(struct platform_device *pdev)
{
	/*  从内核根据设备树生成的 platform_device 
		结构体中获取到设备节点
	*/
	struct device *dev = &pdev->dev;
	struct device_node *np = dev->of_node;
	
	int gpio_pin, i;

	char a[20];
	const char *my_name = a;
	
	/* 通过my_name  属性匹配设备 */
	of_property_read_string(np, "my_name", &my_name);
	for(i = 0; i < dev_cnt; i++){
		if(strcmp(dev_names[i],my_name) == 0){
			strcpy(dev_names[i], "");
			gpio_pin = desc_to_gpio(my_dev_gpio[i]); 
			//释放GPIO
			gpiod_put(my_dev_gpio[i]);
			//销毁设备
			device_destroy(my_dev_class, MKDEV(major, i));
			printk("my_remove run, device_destroy %s, my_gpio = %d\n", my_name, gpio_pin);
		}
	}
	return 0;
}

static struct of_device_id my_dev_match[] = {
	{.compatible = "hc-led"}, 
	{.compatible = "hc-beep"}, 
	{.compatible = "hc-jdq"}, 
	{},
};

static struct platform_driver dev_driver = {
	.probe		=	my_probe,	
	.remove		= 	my_remove,
	.driver		= {
		.name	= "my_platform_driver",
		.of_match_table = my_dev_match,
	},
};

/*=============================驱动出入口函数==============================*/
/* 驱动入口函数：insmod xx.ko 时会被调用 */
static int dev_init(void)
{	
	major = register_chrdev(0, "hc_dev_drv", &my_dev_ops);
	if(major < 0){
		printk("register_chrdev famy\n");
		return major;
	}

	my_dev_class = class_create(THIS_MODULE, "my_dev_class");
	if(IS_ERR(my_dev_class)){
		printk("class_create failed\n");
		return 1;
	}

	platform_driver_register(&dev_driver);

	return 0;
}

/* 驱动出口函数： rmmod xx.ko 时会被调用 */
static void dev_exit(void)
{
	platform_driver_unregister(&dev_driver);
	class_destroy(my_dev_class);
	unregister_chrdev(major, "hc_dev_drv");
	printk("my_dev driver exit\n");
}

module_init(dev_init);
module_exit(dev_exit);
MODULE_LICENSE("GPL");
