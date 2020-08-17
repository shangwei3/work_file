ifeq ($(KERNELRELEASE),)
#KERNEL_DIR = /lib/modules/$(shell uname -r)/build
KERNEL_DIR=/home/linux/workdir/exynos4412/kernel/linux-3.14-fs4412
PWD = $(shell pwd)

module:
	$(MAKE) -C $(KERNEL_DIR)  M=$(PWD)  modules
	cp mpu6050.ko /home/linux/workdir/exynos4412/fs/rootfs
clean:
	$(MAKE) -C $(KERNEL_DIR)  M=$(PWD)  clean

else 
	obj-m = mpu6050.o
endif
