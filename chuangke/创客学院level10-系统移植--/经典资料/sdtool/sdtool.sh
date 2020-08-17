#!/bin/bash

#--------+--------------------------+--------+
#  img   |   bl1  |   bl2  | u-boot |   env  |
#--------+--------+--------+--------+--------+
#  size  |   8K   |   16K  |  472K  |   16K  |
#--------+--------+--------+--------+--------+
#   SD   |  1-16  |  17-48 | 49-992 |993-1023|
#--------+--------+--------+--------+--------+
#  eMMC  |  0-15  +  16-47 | 48-991 |992-1023|
#--------+--------+--------+--------+--------+

bl1_start=1
bl2_size=16384

usage()
{
	echo "usage: ./sdtool.sh clean"
	echo "or"
	echo "       ./sdtool.sh update"
	echo "or"
	echo "       ./sdtool.sh mkuboot <u-boot filename>"
	echo "or"
	echo "       ./sdtool.sh fuse <SD Reader's device file> <filename>"
}

if [ $# -lt 1 -o $# -gt 3 ]; then
	usage
	exit 1
elif [ $# -eq 1 ]; then
	if [ "$1" = "clean" ]; then
		make -C ./bl2 clean
		make -C ./tool clean
		exit 0
	fi

	if [ "$1" = "update" ]; then
		make -C ./bl2 clean
		make -C ./bl2
		make -C ./tool clean
		make -C ./tool
		./tool/mkbl2 ./bl2/stage2.bin ./bl2/bl2.bin $bl2_size
		exit 0
	fi

	usage
	exit 1
elif [ $# -eq 2 ]; then
	if [ "$1" = "mkuboot" ]; then
		make -C ./bl2 clean
		make -C ./bl2
		make -C ./tool clean
		make -C ./tool
		./tool/mkbl2 ./bl2/stage2.bin ./bl2/bl2.bin $bl2_size
		cat bl1/E4412_N.bl1.bin bl2/bl2.bin $2 > u-boot-fs4412.bin
		echo "u-boot-fs4412.bin is ready now"
		exit 0
	fi

	usage
	exit 1
else
	if [ ! -f ./tool/mkbl2 ]; then
		make -C ./tool
	fi

	if [ ! -f ./bl2/stage2.bin ]; then
		make -C ./bl2
	fi

	if [ ! -f ./bl2/bl2.bin ]; then
		./tool/mkbl2 ./bl2/stage2.bin ./bl2/bl2.bin $bl2_size
	fi

	echo "---------------------------------------"
	echo "U-Boot fusing"
	dd iflag=dsync oflag=dsync if=$3 of=$2 seek=$bl1_start
	if [ $? -ne 0 ]; then
		sync
		exit 1
	fi

	sync

	echo "---------------------------------------"
	echo "Image is fused successfully."
	echo "Eject SD card and insert it again."
fi
