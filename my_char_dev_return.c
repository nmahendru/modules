/*
AUTHOR: Nitin Mahendru

*/
 
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/fcntl.h>
#include <linux/stat.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <asm/system.h>
#include <asm/uaccess.h>
 
#define DEVICE_NAME "my_char_dev_return"
#define BUFFER_SIZE 9
 
MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Zobayer Hasan");
MODULE_DESCRIPTION("A simple character device driver.");
MODULE_SUPPORTED_DEVICE(DEVICE_NAME);
 
int device_init(void);
void device_exit(void);
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

 
module_init(device_init);
module_exit(device_exit);
 
static struct file_operations fops = {
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release
};
 
static int device_major = 61;
static int device_opend = 0;
static char device_buffer[BUFFER_SIZE];
static char *buff_rptr;
static char *buff_wptr;








module_param(device_major, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(device_major, DEVICE_NAME " major number");

static int my_char_dev_return_read_k(char * buff , int length){
    int i ;
    for( i = 0 ; i < length ; i++){
        buff[i] = device_buffer[i];
    }
    buff[length] = 0;
    return length;
}
static int my_char_dev_return_write_k(char * buff , int length){
    int i;
    for(i = 0 ; i < length ; i++){
        device_buffer[i] = buff[i];
    }
    device_buffer[length] = 0;
    return length;
}

EXPORT_SYMBOL(my_char_dev_return_read_k);
EXPORT_SYMBOL(my_char_dev_return_write_k);
 
int device_init(){
    int ret;
    ret = register_chrdev(device_major, DEVICE_NAME, &fops);
    if(ret < 0) {
        printk(KERN_ALERT "chardev: cannot obtain major number %d.\n", device_major);
        return ret;
    }
    memset(device_buffer, 0, BUFFER_SIZE);
    printk(KERN_INFO "thesis: my_char_dev_return loaded.\n");
    return 0;
}
 
void device_exit() {
    unregister_chrdev(device_major, DEVICE_NAME);
    printk(KERN_INFO "thesis: my_char_dev_return unloaded.\n");
}
 
static int device_open(struct inode *nd, struct file *fp){
	printk("thesis: my_char_dev_return: Device open called\n");
	if(device_opend) return -EBUSY;
    device_opend++;
    buff_rptr = buff_wptr = device_buffer;
    try_module_get(THIS_MODULE);
    return 0;
}
 
static int device_release(struct inode *nd, struct file *fp) {
    printk("thesis: my_char_dev_return: Device release called\n");
    if(device_opend) device_opend--;
    module_put(THIS_MODULE);
    return 0;
}
 
static ssize_t device_read(struct file *fp, char *buff, size_t length, loff_t *offset) {
    //printk("my_char_dev_return: Length to be read %d\n" , length);	
    int bytes_read = strlen(buff_rptr);
    if(bytes_read > length) bytes_read = length;
    copy_to_user(buff, buff_rptr, bytes_read);
    buff_rptr += bytes_read;
    return bytes_read;
}
 
static ssize_t device_write(struct file *fp, const char *buff, size_t length, loff_t *offset) {
	//printk("my_char_dev_return: Length to be written = %d\n", length);
     int bytes_written;
    memset(device_buffer, 0, BUFFER_SIZE);
    bytes_written = BUFFER_SIZE - (buff_wptr - device_buffer);
    if(bytes_written > length) bytes_written = length;
    copy_from_user(buff_wptr, buff, bytes_written);
    //buff_wptr += bytes_written;
    return bytes_written;
}
 
