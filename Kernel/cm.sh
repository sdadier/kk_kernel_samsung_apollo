echo "copy modules"
rm ../modules/*.ko
make -j12 modules
cd modules
cd fm_si4709
make clean
make -j12 KDIR=../..
cd ../bcm4329
make clean
make -j12 KDIR=../..
cd ../..
find . -name *.ko | while read MODULE; do cp $MODULE ../modules/ ; done
ls -l ../modules/*
echo Ready
