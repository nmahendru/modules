
obj-m += my_char_dev_return.o

KDIR := /lib/modules/`uname -r`/build

PWD := `pwd`

all:
	        make -C $(KDIR) M=$(PWD) modules

clean:
	        make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
