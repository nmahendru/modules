#!/bin/bash

rm *.mod.o *.mod.c *.o *.ko Module.symvers
rmmod user_process_call_test
rmmod char_dev
echo "Making char dev now"
cp Makefile.char_dev Makefile
make
rm Makefile
echo "inserting char dev now"
insmod char_dev.ko
echo "making user call process test now"
cp Makefile.user_process_call_test Makefile
make
rm Makefile
cp Makefile.write_user Makefile
make
rm Makefile
echo "inserting user process call test now"
insmod user_process_call_test.ko