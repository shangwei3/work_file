ifeq ($(KERNELRELEASE),)

KERNEL_BUILD = /lib/modules/$(shell uname -r)/build
MODULE_PATH := $(shell pwd)

module:
	$(MAKE) -C $(KERNEL_BUILD) M=$(MODULE_PATH) modules
clean:
	$(MAKE) -C $(KERNEL_BUILD) M=$(MODULE_PATH) clean
else
	obj-m = led_driver.o
endif
		



