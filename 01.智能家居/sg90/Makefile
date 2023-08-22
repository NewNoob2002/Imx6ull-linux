KERN_DIR = /home/me/linux-imx-rel_imx_4.1.15_2.1.0_ga_alientek

PWD ?= $(shell KERN_DIR)

all:	
	make -C $(KERN_DIR) M=$(PWD) modules
	$(CROSS_COMPILE)gcc -o sg90_test sg90_test.c
	
clean:
	make -C $(KERN_DIR) M=$(PWD) modules clean
	rm -rf modules.order
	rm -f sg90_test

obj-m += sg90_drv.o

