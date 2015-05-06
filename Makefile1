
KBUILD_EXTRA_SYMBOLS := /home/nitin/thesis/modules/my_char_dev/Module.symvers /home/nitin/thesis/modules/my_char_dev_return/Module.symvers /home/nitin/thesis/modules/Modules.symvers
ccflags-y := -std=gnu99 -Wno-declaration-after-statement

obj-m += interceptor.o

KDIR := /lib/modules/`uname -r`/build

PWD := `pwd`

all:
	        make -C $(KDIR) M=$(PWD) modules

clean:
	        make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
