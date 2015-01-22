obj-m += char_dev.o

KDIR := /lib/modules/`uname -r`/build

PWD := `pwd`

all:
	make -C $(KDIR) M=$(PWD) modules

