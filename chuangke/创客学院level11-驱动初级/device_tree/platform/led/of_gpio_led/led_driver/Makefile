ifeq ($(KERNELRELEASE),)

#KERNEL_DIR ?=/lib/modules/$(shell uname -r)/build 
KERNEL_DIR = /home/linux/workdir/exynos4412/kernel/linux-3.14-fs4412 
PWD :=$(shell pwd)

modules:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) modules 

.PHONY:modules clean 
clean:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) clean

install:
	cp  led_driver.ko /home/linux/workdir/exynos4412/fs/rootfs  
else 
	obj-m := led_driver.o 
endif
