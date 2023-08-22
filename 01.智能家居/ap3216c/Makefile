KERN_DIR = /home/me/linux-imx-rel_imx_4.1.15_2.1.0_ga_alientek

PWD ?= $(shell KERN_DIR)

all:	
	make -C $(KERN_DIR) M=$(PWD) modules
	$(CROSS_COMPILE)gcc -o test_ap3216 test_ap3216.c
clean:
	make -C $(KERN_DIR) M=$(PWD) modules clean
	rm -rf modules.order
	rm -f ap3216_i2c_driver test_ap3216

obj-m += ap3216_i2c_driver.o ap3216_i2c_client.o