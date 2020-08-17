ifeq ($(KERNELRELEASE),)
KERNEL_DIR ?=/lib/modules/$(shell uname -r)/build
PWD=$(shell pwd)

modules:
	$(MAKE) -C $(KERNEL_DIR)  M=$(PWD) modules

clean:
	$(MAKE) -C $(KERNEL_DIR)  M=$(PWD)  clean
else
	obj-m := mycdev.o
endif 

