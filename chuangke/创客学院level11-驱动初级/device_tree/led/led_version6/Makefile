ifeq ($(KERNELRELEASE),)

#KERNEL_BUILD = /lib/modules/$(shell uname -r)/build
KERNEL_BUILD = /home/linux/workdir/fs4412/kernel/linux-3.14
MODULE_PATH := $(shell pwd)

module:
	$(MAKE) -C $(KERNEL_BUILD) M=$(MODULE_PATH) modules

clean:
	$(MAKE) -C $(KERNEL_BUILD) M=$(MODULE_PATH) clean

else
	obj-m = led-driver.o
endif
