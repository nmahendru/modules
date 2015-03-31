#!/bin/bash
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
