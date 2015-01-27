// code for creating  new device in the kernel.

#include <linux/module.h>
#include <linux/string.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include "char_dev.h"
//module attributes
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Device to transfer data between kernel/user space");
MODULE_AUTHOR("Nitin Mahendru");

#define BUF_SIZE 500

static char buf[BUF_SIZE] = {0};
static short readPos = 0;
static int times = 0;

static int dev_open(struct inode * ,  struct file *);
static int dev_rls(struct inode * ,  struct file *);
static ssize_t dev_write(struct file * , const char * , size_t , loff_t *);
static ssize_t dev_read(struct file * , char * , size_t , loff_t * );
static dev_t  dev_number = MKDEV(89 , 1); 
struct cdev * char_dev;

int write_char_dev_k(char * buff , int len){
	int temp;
	temp = len;
	while(temp >=0){
		buf[temp] = buff[temp];
		temp--;
	}
	return 0;
}

int read_char_dev_k(char * buff , int len){
	printk("kernel level read called with length of bytes = %d\n" , len);
	int temp ;
	temp = len;
	if(--temp > BUF_SIZE - 1) return -1;
	//buff[temp--] = '\0';
	while(temp >= 0){

		buff[temp] = buf[temp];
		temp--;
	}
	return 0;
}

	EXPORT_SYMBOL(read_char_dev_k);
	EXPORT_SYMBOL(write_char_dev_k);

static struct file_operations fops = 
{
	.read = dev_read,
	.write = dev_write,
	.open = dev_open,
	.release = dev_rls,
};

//called when the module is loaded
int init_module(void){
	//dev_t  dev_number = MKDEV(89 , 1); // create a post  2.6 kernel number using the macro. This macro is defines in linux/types.h
	int retValue = register_chrdev_region(dev_number , 1 , "char_dev");
	if(retValue >= 0) {
		printk(KERN_ALERT "Device allocated a device number successfully\n");
		char_dev = cdev_alloc();
		//char_dev->ops = &fops;
		cdev_init(char_dev, &fops);
		int cdevAddRvalue;
		cdevAddRvalue =  cdev_add(char_dev , dev_number , 1);
		if(cdevAddRvalue < 0){
			printk(KERN_ALERT "Device could not be added. return value from cdev_add = %d\n" , cdevAddRvalue );
		}else{
			printk(KERN_ALERT "Device added successfully using cdev_Add\n");
		}
	}
	else {
		printk(KERN_ALERT "Device registration failed.\n return value for chr_dev = %d " , retValue);
	}
	//int t = register_chdev(89 , "char_dev" , &fops);

	//if(t < 0) printk(KERN_ALERT "Device registration failed.\n return value for chr_dev = %d " , t);
	//else printk(KERN_ALERT "Device registration successful\n");


	return retValue;
}

void cleanup_module(void){
	//unregister_chrdev(89 , "char_dev");
	unregister_chrdev_region(dev_number, 1);
	cdev_del(char_dev);
}

static int dev_open(struct inode * inod , struct file * fil){
	times++;
	printk(KERN_ALERT "Device Opened %d times \n" ,  times);
	return 0;
}
//user space program will try to read this device from user space using this function
static ssize_t dev_read(struct file * filp , char * buff , size_t len , loff_t * off){
	printk(KERN_ALERT"length to be read %d\nValue to be read" , (int)len);
	
	short count; 
	count = 0;
	while(len && (buf[readPos] != 0 ) && readPos < BUF_SIZE ){
		printk(KERN_ALERT"%c" , buf[readPos] );
		put_user(buf[readPos] , buff++);
		count++ ;
		len--;
		readPos++;
		
	}
	printk(KERN_ALERT"\n");
	*off = 0;
	//*off = 0;
	//readPos = 0;
	return count ;
	/*








	//function to copy kernel space buffer to user space
	if ( copy_to_user(buff,buf,strlen(buf)) != 0 )
		printk( "Kernel -> userspace copy failed!\n" );
	return strlen(buf);
	*/
}

//user space program will try to write down messages from user space to this device. control mechanism is set.
static ssize_t dev_write(struct file * filp , const char * buff , size_t len , loff_t * off){
	//short ind = len;
	short count = 0;
	printk(KERN_ALERT "length to be written %d\nValue to be written" , (int)len);

	while(len > 0){
		printk(KERN_ALERT "%c" , buff[len - 1]);
		buf[len] = buff[--len];
		count++;
		//(*off)++;
		//len--;
	}
	printk(KERN_ALERT "\n");
	//*off = 0;
	return count;
	



	/* function to copy user space buffer to kernel space
	if ( copy_from_user(buf,buff,len) != 0 )
		printk( "Userspace -> kernel copy failed!\n" );
	return 0;*/
}


static int dev_rls(struct inode * inod , struct file * fil){
	printk(KERN_ALERT "Device char_dev closed \n");
	return 0;
}


