KERNEL_DIR ?=/lib/modules/$(shell uname -r)/build
PWD=$(shell pwd)

ifeq ($(KERNELRELEASE),)
modules:
	$(MAKE) -C $(KERNEL_DIR)  M=$(PWD) modules

clean:
	$(MAKE) -C $(KERNEL_DIR)  M=$(PWD)  clean
else
	obj-m := s5pv210led_device.o
endif 

