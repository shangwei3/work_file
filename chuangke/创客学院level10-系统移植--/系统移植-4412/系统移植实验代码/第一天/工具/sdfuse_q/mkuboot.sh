
#!/bin/bash

#
# This script will create a u-boot binary for movinand/mmc boot
#

echo "Fuse FS4412 trustzone uboot file into SD card"

if [ -z $1 ]
then
	./sd_fusing_exynos4x12.sh /dev/sdb u-boot-fs4412.bin
else
	./sd_fusing_exynos4x12.sh $1 u-boot-fs4412.bin
fi

