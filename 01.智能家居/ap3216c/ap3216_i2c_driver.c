#include <linux/init.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/uaccess.h>
#include <linux/mod_devicetable.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/delay.h>

static int major;
static struct class *ap3216_class;
static struct i2c_client * ap3216_client;

static int ap3216_open (struct inode *node, struct file *filp)
{
	/* reset: write 0x4 to reg 0 */
	i2c_smbus_write_byte_data(ap3216_client, 0, 0x4);   
	/* delay for reset */
	mdelay(15);

	/* enable: write 0x3 to reg 0 */
	i2c_smbus_write_byte_data(ap3216_client, 0, 0x3);
	return 0;
}

static ssize_t ap3216_read (struct file *filp, char __user *buf, size_t size, loff_t *offset)
{
	int var, err;
	char data[6];

	if(size != 6)
		return -EINVAL;
	
	/* read IR */
	var = i2c_smbus_read_word_data(ap3216_client, 0xa);
	data[0] = (var >> 8) & 0xff; 
	data[1] = var & 0xff;

	/* read light */
	var = i2c_smbus_read_word_data(ap3216_client, 0xc);
	data[2] = (var >> 8) & 0xff; 
	data[3] = var & 0xff;

	/* read dis */
	var = i2c_smbus_read_word_data(ap3216_client, 0xe);
	data[4] = (var >> 8) & 0xff; 
	data[5] = var & 0xff;

	err = copy_to_user(buf, data, size);

	return 0;
}

//用于和设备树匹配
static const struct of_device_id ap3216_dt_match[] = {
	{ .compatible = "alientek,ap3216c", },
	{ }, 
};

//用于和一般的i2c设备匹配，不管i2c设备来自设备树还是手工创建
static const struct i2c_device_id ap3216_i2c_id[] = {
	{ "ap3216c", },
	{ }
};

static struct file_operations ap3216_fops = {
	.owner	= 	THIS_MODULE,
	.open 	= 	ap3216_open,
	.read 	= 	ap3216_read,
};

static int ap3216_i2c_probe(struct i2c_client *client, const struct i2c_device_id * i2c_id)
{
	struct device *result;
	ap3216_client = client;
	
	/* register chrdev */
	major = register_chrdev(0, "ap3216", &ap3216_fops);
	
	ap3216_class = class_create(THIS_MODULE, "ap3216_class");
	if (IS_ERR(ap3216_class)){
		printk("ap3216 class_create failed!\n");
		
		unregister_chrdev(major, "ap3216");
		return PTR_ERR(ap3216_class);
	}
	result = device_create(ap3216_class, NULL, MKDEV(major, 0), NULL, "ap3216");  /* /dev/ap3216 */
	if (IS_ERR(result)){
		printk("ap3216 device_create failed\n");
		class_destroy(ap3216_class);
		unregister_chrdev(major, "ap3216");
		return -ENODEV;
	}
			
	printk("ap3216_i2c probe\n");

	return 0;
}

static int ap3216_i2c_remove(struct i2c_client *client)
{
	device_destroy(ap3216_class, MKDEV(major, 0));
	class_destroy(ap3216_class);
	unregister_chrdev(major, "ap3216");
	return 0;
}


static struct i2c_driver ap3216_i2c_driver = {
	.driver = {
		.owner = THIS_MODULE,
		.name = "ap3216",
		.of_match_table	= ap3216_dt_match,
	},
	.probe = ap3216_i2c_probe,
	.remove = ap3216_i2c_remove,
	.id_table = ap3216_i2c_id,
};


static int __init ap3216_i2c_init(void)
{
	int ret;
	ret = i2c_add_driver(&ap3216_i2c_driver);
	if (ret != 0)
		pr_err("Failed to register ap3216 I2C driver: %d\n", ret);
	
	return 0;
}

static void __exit ap3216_i2c_exit(void)
{
	i2c_del_driver(&ap3216_i2c_driver);
}

module_init(ap3216_i2c_init);
module_exit(ap3216_i2c_exit);
MODULE_LICENSE("GPL");


