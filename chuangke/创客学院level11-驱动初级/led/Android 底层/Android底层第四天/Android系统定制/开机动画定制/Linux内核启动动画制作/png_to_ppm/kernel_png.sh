
#$1为参数，这样不用每次改图片名称，后几步为执行过程  
pngtopnm $1 > linuxlogo.pnm       
pnmquant 224 linuxlogo.pnm > linuxlogo224.pnm  
pnmtoplainpnm linuxlogo224.pnm > logo_linux_clut224.ppm  
#删除过程中生成不需要的文件  
rm -rf ./linuxlogo.pnm  ./linuxlogo224.pnm   
rm -rf ./linuxlogo.pnm  ./linuxlogo224.pnm 
