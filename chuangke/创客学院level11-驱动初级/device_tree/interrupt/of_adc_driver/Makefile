ifeq ($(KERNELRELEASE),)

#KERNEL_DIR ?=/lib/modules/$(shell uname -r)/build 
KERNEL_DIR = /home/linux/workdir/exynos_4412/kernel/linux-3.14-fs4412 
PWD :=$(shell pwd)

modules:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) modules 
	cp  adc_driver.ko /home/linux/rootfs 

.PHONY:modules clean 
clean:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) clean

install:
	cp  adc_driver.ko /home/cyg/rootfs/module  
else 
	obj-m := adc_driver.o 
endif
