#!/bin/bash

cp /home/nitin/thesis/modules/create_devices.sh /dev/.
cd /dev/
chmod 700 /dev/create_devices.sh

mknod my_char_dev c 60 0
mknod my_char_dev_return c 61 0

cd -