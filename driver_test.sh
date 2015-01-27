#!/bin/bash



#Below three commands are just to make the character devices that are being used by the two drivers

#cd /dev/
#mknod my_char_dev c 60 0
#mknod my_char_dev_return c 61 0

rm -rf my_char_dev my_char_dev_return


rm *.mod.o *.mod.c *.o *.ko Module.symvers
rmmod my_char_dev
rmmod my_char_dev_return
rmmod read_inode_list
echo "Making my_char dev now"
mkdir my_char_dev
cp Makefile.my_char_dev my_char_dev/Makefile
cp my_char_dev.c my_char_dev/my_char_dev.c
cd my_char_dev
make
echo "inserting my char dev now"
insmod my_char_dev.ko

cd ..
mkdir my_char_dev_return
echo "Making my_char_dev_return now"
cp Makefile.my_char_dev_return my_char_dev_return/Makefile
cp my_char_dev_return.c my_char_dev_return/my_char_dev_return.c
cd my_char_dev_return
make
echo "inserting my_char_dev_return now"
insmod my_char_dev_return.ko

cd ..

echo "Making read inode list now"

cp Makefile.read_inode_list Makefile
make
insmod read_inode_list.ko
rm -rf my_char_dev my_char_dev_return

#echo "making user call process test now"
#cp Makefile.user_process_call_test Makefile
#make
#rm Makefile
#cp Makefile.write_user Makefile
#make
#rm Makefile
#echo "inserting user process call test now"
#insmod user_process_call_test.ko