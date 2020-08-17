#!/bin/sh 

if [ ! -f "u-boot.bin" ]
then
	echo "You must have u-boot.bin"
	exit
fi

gcc add_padding.c -o add_padding 
./add_padding
rm -rf u-boot-fs4412.bin
cat E4412_N.bl1.SCP2G.bin bl2.bin all00_padding.bin u-boot.bin tzsw_SMDK4412_SCP_2GB.bin > u-boot-fs4412.bin
