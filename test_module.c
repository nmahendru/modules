#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/delay.h>
#include <asm/paravirt.h>
#include <linux/fs.h>
#include <asm/segment.h>
#include <asm/uaccess.h>
#include <linux/buffer_head.h>
#include <linux/sched.h>
#include <linux/fdtable.h>
#include <linux/list.h>



unsigned long **sys_call_table;
unsigned long original_cr0;


asmlinkage long (*ref_sys_open)(const char* filename, int flags, int mode); //3




//3
asmlinkage long new_sys_open(const char* filename, int flags, int mode)
{
		long ret;
		ret = ref_sys_open(filename, flags, mode);
		if(ret > 2 && strcmp(filename , "/home/nitin/test.c") == 0){
			printk("File descriptor returned is %d\n", (int)ret);
			spin_lock(&(current->files->file_lock));
			if(current->files->fdt != NULL){
				printk("fd table\n");
				if(current->files->fdt->fd[(int)ret] != NULL){
					printk("fd array\n");						
					if(current->files->fdt->fd[(int)ret]->f_path.dentry != NULL){
						printk("dentry\n");
						if(current->files->fdt->fd[(int)ret]->f_path.dentry->d_inode != NULL){
							printk("inode\n");	

			int inode_number = current->files->fdt->fd[(int)ret]->f_path.dentry->d_inode->i_ino;
			printk("inode_number returned is %d\n", inode_number);
						}
					}
				}
			}

			spin_unlock(&(current->files->file_lock));
		}
		return ret;
}


static unsigned long **aquire_sys_call_table(void)
{
	unsigned long int offset = PAGE_OFFSET;
	unsigned long **sct;

	while (offset < ULLONG_MAX) {
		sct = (unsigned long **)offset;

		if (sct[__NR_close] == (unsigned long *) sys_close) 
			return sct;

		offset += sizeof(void *);
	}
					
	return NULL;
}

static int __init interceptor_start(void) 
{
	if(!(sys_call_table = aquire_sys_call_table()))
		return -1;
			
	original_cr0 = read_cr0();

	write_cr0(original_cr0 & ~0x00010000);

	
	ref_sys_open = (void *)sys_call_table[__NR_open];
	sys_call_table[__NR_open] = (unsigned long *)new_sys_open;
	
	write_cr0(original_cr0);
	printk("module is loaded now\n");
		return 0;
}

static void __exit interceptor_end(void) 
{
	if(!sys_call_table) {
		return;
	}
				
	write_cr0(original_cr0 & ~0x00010000);
	
	sys_call_table[__NR_open] = (unsigned long *)ref_sys_open;
	
	write_cr0(original_cr0);
				
	printk("unloading module now\n");					
	msleep(2000);
}

module_init(interceptor_start);
module_exit(interceptor_end);

MODULE_LICENSE("GPL");



