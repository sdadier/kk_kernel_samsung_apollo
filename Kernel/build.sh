#!/bin/sh
echo "Removing old build"
rm arch/arm/boot/zImagekk_lowlat.tar
rm /home/blob/Bureau/DD/zImagekk_lowlat.tar
echo "Building ..."
make -j5
cd arch/arm/boot/
tar -cf zImagekk_lowlat.tar zImage
ls -l zImagekk_lowlat.tar
echo "Moving zImage to sharedfolder"
mv zImagekk_lowlat.tar /home/blob/Bureau/DD
