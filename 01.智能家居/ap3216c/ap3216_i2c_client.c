#include <linux/init.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/uaccess.h>
#include <linux/mod_devicetable.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/delay.h>

struct i2c_client *client = NULL;

static int __init ap3216_i2c_clien_init(void)
{
	int bus = 0;
	struct i2c_adapter *adapter;
	struct i2c_board_info ap3216_info = {
		I2C_BOARD_INFO("ap3216c", 0x1e),
	};

	/* get i2c adapter bus*/
	adapter = i2c_get_adapter(bus);
	if (!adapter) {
		pr_err("%s failed to get i2c adapter %d.\n", __func__, bus);
		return -1;
	}
	/* register i2c device */
	client = i2c_new_device(adapter, &ap3216_info);
	/* release i2c adapter */
	i2c_put_adapter(adapter);
	return 0;
}

static void __exit ap3216_i2c_clien_exit(void)
{
	i2c_unregister_device(client);
}

module_init(ap3216_i2c_clien_init);
module_exit(ap3216_i2c_clien_exit);
MODULE_LICENSE("GPL");




