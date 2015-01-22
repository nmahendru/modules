obj-m += user_process_call_test.o

KDIR := /lib/modules/`uname -r`/build

PWD := `pwd`

all:
	make -C $(KDIR) M=$(PWD) modules

