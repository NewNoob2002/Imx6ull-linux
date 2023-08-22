#!/bin/sh

insmod sr501_drv_dtb.ko &
insmod gt9147.ko &
insmod ap3216_i2c_driver.ko &
insmod dht11_drv_dtb.ko &
insmod led_drv.ko &
insmod sg90_drv.ko 


