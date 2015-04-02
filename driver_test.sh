#!/bin/bash



#Below three commands are just to make the character devices that are being used by the two drivers

cd /dev/
mknod my_char_dev c 60 0
	mknod my_char_dev_return c 61 0

cd /home/nitin/thesis/modules

echo "compiling readInodes"
gcc -g -o readInodes readInodes.cc
if [[ ! $? ]]; then
	echo "readInodes compilation failed...exiting"
	exit
fi

echo "compiling write nodes"
g++ -std=c++11 -o writenodes writenodes.cc

if [[ ! $? ]]; then
	echo "writenodes compilation failed...exiting"
	exit
fi

rm -rf my_char_dev my_char_dev_return &> /dev/null


rm *.mod.o *.mod.c *.o *.ko Module.symvers &> /dev/null
rmmod read_inode_list &> /dev/null
rmmod my_char_dev &> /dev/null
rmmod my_char_dev_return &> /dev/null

echo "Making my_char dev now"
mkdir my_char_dev
cp Makefile.my_char_dev my_char_dev/Makefile
cp my_char_dev.c my_char_dev/my_char_dev.c
cd my_char_dev
#rm *.mod.o *.mod.c *.o *.ko Module.symvers
make
if [[ ! $? ]]; then
	echo "make for my_char_dev failed...exiting"
	exit#statements
fi
echo "inserting my char dev now"
insmod my_char_dev.ko
if [[ ! $? ]]; then
	echo " my_char_dev insertion failed ..exiting"
	exit
fi

cd ..
mkdir my_char_dev_return
echo "Making my_char_dev_return now"
cp Makefile.my_char_dev_return my_char_dev_return/Makefile
cp my_char_dev_return.c my_char_dev_return/my_char_dev_return.c
cd my_char_dev_return
#rm *.mod.o *.mod.c *.o *.ko Module.symvers
make
if [[ ! $? ]]; then
	echo "make for my_char_dev_return failed...exiting"
	exit
fi

echo "inserting my_char_dev_return now"
insmod my_char_dev_return.ko
if [[ ! $? ]]; then
	echo "my_char_dev_return insertion failed...exiting"
	exit #statements
fi

cd ..

echo "Making read inode list now"

cp Makefile.read_inode_list Makefile
make
if [[ ! $? ]]; then
	echo " read_inode_list compilation failed...exiting"
	exit
fi
insmod read_inode_list.ko


echo "Making interceptor now"

cp Makefile.interceptor Makefile
make
if [[ ! $? ]]; then
	echo " read_inode_list compilation failed...exiting"
	exit
fi
insmod interceptor.ko
#./readInodes
#./writenodes
