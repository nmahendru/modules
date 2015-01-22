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


//************The below functions are just to read the proc in an lkm

/*struct file* file_open(const char* path, int flags, int rights) {
    struct file* filp = NULL;
    mm_segment_t oldfs;
    int err = 0;

    oldfs = get_fs();
    set_fs(get_ds());
    filp = filp_open(path, flags, rights);
    set_fs(oldfs);
    if(IS_ERR(filp)) {
        err = PTR_ERR(filp);
        return NULL;
    }
    return filp;
}

void file_close(struct file* file) {
    filp_close(file, NULL);
}

int file_write(struct file* file, unsigned long long offset, unsigned char* data, unsigned int size) {
    mm_segment_t oldfs;
    int ret;

    oldfs = get_fs();
    set_fs(get_ds());

    ret = vfs_write(file, data, size, &offset);

    set_fs(oldfs);
    return ret;
}

int file_read(struct file* file, unsigned long long offset, unsigned char* data, unsigned int size) {
    mm_segment_t oldfs;
    int ret;

    oldfs = get_fs();
    set_fs(get_ds());

    ret = vfs_read(file, data, size, &offset);

    set_fs(oldfs);
    return ret;
} 
*/
//***************************************************************************
//#include <unistd.h>


//*************************************************************************************************************
//Code for reading/writing a proc file


//
//*************************************************************************************************************
unsigned long **sys_call_table;
unsigned long original_cr0;

//READ system call overridden
asmlinkage long (*ref_sys_read)(unsigned int fd, char __user *buf, size_t count); //1
asmlinkage long (*ref_sys_write)(unsigned int fd, char __user *buf, size_t count); //2
asmlinkage long (*ref_sys_open)(const char* filename, int flags, int mode); //3
asmlinkage long (*ref_sys_close)(unsigned int fd); //4
//asmlinkage long (*ref_sys_link)(const char * oldname, const char * newname); //5
//asmlinkage long (*ref_sys_unlink)(const char  * pathname); //6
//asmlinkage long (*ref_sys_chdir)(const char * filename); //7
/*asmlinkage long (*ref_sys_mknod)(unsigned int fd, char __user *buf, size_t count); //8
asmlinkage long (*ref_sys_lchown)(unsigned int fd, char __user *buf, size_t count); //9
asmlinkage long (*ref_sys_lseek)(unsigned int fd, char __user *buf, size_t count); //10
asmlinkage long (*ref_sys_mount)(unsigned int fd, char __user *buf, size_t count); //11
asmlinkage long (*ref_sys_fstat)(unsigned int fd, char __user *buf, size_t count); //12
asmlinkage long (*ref_sys_access)(unsigned int fd, char __user *buf, size_t count); //13
asmlinkage long (*ref_sys_sync)(unsigned int fd, char __user *buf, size_t count); //14
asmlinkage long (*ref_sys_mkdir)(unsigned int fd, char __user *buf, size_t count); //15
asmlinkage long (*ref_sys_rmdir)(unsigned int fd, char __user *buf, size_t count); //16
asmlinkage long (*ref_sys_chroot)(unsigned int fd, char __user *buf, size_t count); //17
asmlinkage long (*ref_sys_symlink)(unsigned int fd, char __user *buf, size_t count); //18
asmlinkage long (*ref_sys_lstat)(unsigned int fd, char __user *buf, size_t count); //19
asmlinkage long (*ref_sys_readlink)(unsigned int fd, char __user *buf, size_t count); //20
asmlinkage long (*ref_sys_fchmod)(unsigned int fd, char __user *buf, size_t count); //21
asmlinkage long (*ref_sys_fstats)(unsigned int fd, char __user *buf, size_t count); //22
asmlinkage long (*ref_sys_statfs)(unsigned int fd, char __user *buf, size_t count); //23
asmlinkage long (*ref_sys_fsync)(unsigned int fd, char __user *buf, size_t count); //23
asmlinkage long (*ref_sys_llseek)(unsigned int fd, char __user *buf, size_t count); //24
asmlinkage long (*ref_sys_readv)(unsigned int fd, char __user *buf, size_t count); //25
asmlinkage long (*ref_sys_writev)(unsigned int fd, char __user *buf, size_t count); //26
asmlinkage long (*ref_sys_pread)(unsigned int fd, char __user *buf, size_t count); //27
asmlinkage long (*ref_sys_pwrite)(unsigned int fd, char __user *buf, size_t count); //28
asmlinkage long (*ref_sys_chown)(unsigned int fd, char __user *buf, size_t count); //29
*/
asmlinkage long (*ref_sys_getpid)(void);
asmlinkage long (*ref_sys_readlink)(const char* pathname, char* buf, size_t bufsize);


//



asmlinkage long new_sys_read(unsigned int fd, char __user *buf, size_t count)
{
		long ret;
		struct file *file;
		char fbuf[200], *realpath;
		ret = ref_sys_read(fd, buf, count);
		int fd1 = (long) ref_sys_open("/home/nitin/test.c" , O_WRONLY , S_IRUSR | S_IWUSR);
		long ret1 = ref_sys_write(fd1 , "test data 1\n" , 5);
		ret1  = ref_sys_close(fd1);
		printk("return open = %d    return write = %d     return close = %d \n" , fd1 , ret1 , ret1);

		if (strcmp(current->comm , "rsyslogd") == 0 || strcmp(current->comm , "in:imklog") == 0 )
			return ret;
		
		file = fcheck(fd);
		if(!file){
			//printk(KERN_INFO "thesis_log|||sys_call_name=sys_read|||process_id=%d|||executable=%s\n", current->pid , current->comm);
			return ret;
		}
		const struct path f_path = file->f_path;
		realpath = d_path(&f_path, fbuf, 200);
		//printk(KERN_INFO "thesis_log|||sys_call_name=sys_read|||process_id=%d|||executable=%s|||file_written=%s\n", current->pid , current->comm , realpath);
	    return ret;
}

//2
asmlinkage long new_sys_write(unsigned int fd, char __user *buf, size_t count)
{
		long ret;
		ret = ref_sys_write(fd, buf, count);
		struct file *file;
		char fbuf[200], *realpath;
		if (strcmp(current->comm , "rsyslogd") == 0 || strcmp(current->comm , "in:imklog") == 0 )
			return ret;
		file = fcheck(fd);
		if(!file){
			//printk(KERN_INFO "thesis_log|||sys_call_name=sys_write|||process_id=%d|||executable=%s\n", current->pid , current->comm);
			return ret;
		}
		const struct path f_path = file->f_path;
		realpath = d_path(&f_path, fbuf, 200);
		//printk(KERN_INFO "thesis_log|||sys_call_name=sys_write|||process_id=%d|||executable=%s|||file_written=%s\n", current->pid, current->comm,  realpath);
		return ret;
}



//3
asmlinkage long new_sys_open(const char* filename, int flags, int mode)
{
		long ret;
		ret = ref_sys_open(filename, flags, mode);
		if (strcmp(current->comm , "rsyslogd") == 0 || strcmp(current->comm , "in:imklog") == 0 )
			return ret;
		
		//printk(KERN_INFO "thesis_log|||sys_call_name=sys_open|||process_id=%d|||executable=%s|||file_opened=%s\n", current->pid, current->comm ,  filename);
		return ret;
}

//4
asmlinkage long new_sys_close(unsigned int fd)
{
		long ret;
		ret = ref_sys_close(fd);
		struct file *file;
		char fbuf[200], *realpath;
		if (strcmp(current->comm , "rsyslogd") == 0 || strcmp(current->comm , "in:imklog") == 0 )
			return ret;
		file = fcheck(fd);
		if(!file){
			//printk(KERN_INFO "thesis_log|||sys_call_name=sys_close|||process_id=%d|||executable=%s\n", current->pid , current->comm);
			return ret;
		}
		const struct path f_path = file->f_path;
		realpath = d_path(&f_path, fbuf, 200);	
		//printk(KERN_INFO "thesis_log|||sys_call_name=sys_close|||process_id=%d|||executable=%s|||file_written=%s\n", current->pid, current->comm , realpath);
		return ret;
}



/*
//5
asmlinkage long new_sys_link(const char * oldname , const char * newname)
{
		long ret;
		ret = ref_sys_link(oldname , newname);
		if (strcmp(current->comm , "rsyslogd") == 0 || strcmp(current->comm , "in:imklog") == 0 )
			return ret;
		printk(KERN_INFO "thesis_log|||sys_call_name=sys_link|||process_id=%d|||executable=%s|||oldname=%s|||newname=%s\n", current->pid, current->comm , oldname , newname);
		return ret;
		
}

//6
asmlinkage long new_sys_unlink(const char * pathname)
{
		long ret;
		ret = ref_sys_unlink(pathname);
		if (strcmp(current->comm , "rsyslogd") == 0 || strcmp(current->comm , "in:imklog") == 0 )
			return ret;
		printk(KERN_INFO "thesis_log|||sys_call_name=sys_unlink|||process_id=%d|||executable=%s|||file_unlinked=%s\n", current->pid, current->comm , pathname);
		
		return ret;
}

//7
asmlinkage long new_sys_chdir(const char * filename)
{
		long ret;
		ret = ref_sys_chdir(filename);
		if (strcmp(current->comm , "rsyslogd") == 0 || strcmp(current->comm , "in:imklog") == 0 )
			return ret;
		printk(KERN_INFO "thesis_log|||sys_call_name=sys_chdir|||process_id=%d|||executable=%s|||target_dir=%s\n", current->pid, current->comm , filename);
		
		return ret;
}


*/
/*
//8
asmlinkage long new_sys_write(unsigned int fd, char __user *buf, size_t count)
{
		long ret;
		ret = ref_sys_write(fd, buf, count);
		if(count == 1 && fd == 0)
		printk(KERN_INFO "sys_read|||%d|||\n",getpid());
		return ret;
}
*/
/*
//9
asmlinkage long new_sys_write(unsigned int fd, char __user *buf, size_t count)
{
		long ret;
		ret = ref_sys_write(fd, buf, count);
		if(count == 1 && fd == 0)
		printk(KERN_INFO "sys_read|||%d|||\n",getpid());
		return ret;
}

//9
asmlinkage long new_sys_write(unsigned int fd, char __user *buf, size_t count)
{
		long ret;
		ret = ref_sys_write(fd, buf, count);
		if(count == 1 && fd == 0)
		printk(KERN_INFO "sys_read|||%d|||\n",getpid());
		return ret;
}
//10
asmlinkage long new_sys_write(unsigned int fd, char __user *buf, size_t count)
{
		long ret;
		ret = ref_sys_write(fd, buf, count);
		if(count == 1 && fd == 0)
		printk(KERN_INFO "sys_read|||%d|||\n",getpid());
		return ret;
}
//11
asmlinkage long new_sys_write(unsigned int fd, char __user *buf, size_t count)
{
		long ret;
		ret = ref_sys_write(fd, buf, count);
		if(count == 1 && fd == 0)
		printk(KERN_INFO "sys_read|||%d|||\n",getpid());
		return ret;
}

//12
asmlinkage long new_sys_write(unsigned int fd, char __user *buf, size_t count)
{
		long ret;
		ret = ref_sys_write(fd, buf, count);
		if(count == 1 && fd == 0)
		printk(KERN_INFO "sys_read|||%d|||\n",getpid());
		return ret;
}

//13
asmlinkage long new_sys_write(unsigned int fd, char __user *buf, size_t count)
{
		long ret;
		ret = ref_sys_write(fd, buf, count);
		if(count == 1 && fd == 0)
		printk(KERN_INFO "sys_read|||%d|||\n",getpid());
		return ret;
}

//14
asmlinkage long new_sys_write(unsigned int fd, char __user *buf, size_t count)
{
		long ret;
		ret = ref_sys_write(fd, buf, count);
		if(count == 1 && fd == 0)
		printk(KERN_INFO "sys_read|||%d|||\n",getpid());
		return ret;
}

//15
asmlinkage long new_sys_write(unsigned int fd, char __user *buf, size_t count)
{
		long ret;
		ret = ref_sys_write(fd, buf, count);
		if(count == 1 && fd == 0)
		printk(KERN_INFO "sys_read|||%d|||\n",getpid());
		return ret;
}

//16
asmlinkage long new_sys_write(unsigned int fd, char __user *buf, size_t count)
{
		long ret;
		ret = ref_sys_write(fd, buf, count);
		if(count == 1 && fd == 0)
		printk(KERN_INFO "sys_read|||%d|||\n",getpid());
		return ret;
}
//17
asmlinkage long new_sys_write(unsigned int fd, char __user *buf, size_t count)
{
		long ret;
		ret = ref_sys_write(fd, buf, count);
		if(count == 1 && fd == 0)
		printk(KERN_INFO "sys_read|||%d|||\n",getpid());
		return ret;
}

//18
asmlinkage long new_sys_write(unsigned int fd, char __user *buf, size_t count)
{
		long ret;
		ret = ref_sys_write(fd, buf, count);
		if(count == 1 && fd == 0)
		printk(KERN_INFO "sys_read|||%d|||\n",getpid());
		return ret;
}

//19
asmlinkage long new_sys_write(unsigned int fd, char __user *buf, size_t count)
{
		long ret;
		ret = ref_sys_write(fd, buf, count);
		if(count == 1 && fd == 0)
		printk(KERN_INFO "sys_read|||%d|||\n",getpid());
		return ret;
}

//20
asmlinkage long new_sys_write(unsigned int fd, char __user *buf, size_t count)
{
		long ret;
		ret = ref_sys_write(fd, buf, count);
		if(count == 1 && fd == 0)
		printk(KERN_INFO "sys_read|||%d|||\n",getpid());
		return ret;
}

//21
asmlinkage long new_sys_write(unsigned int fd, char __user *buf, size_t count)
{
		long ret;
		ret = ref_sys_write(fd, buf, count);
		if(count == 1 && fd == 0)
		printk(KERN_INFO "sys_read|||%d|||\n",getpid());
		return ret;
}

//22
asmlinkage long new_sys_write(unsigned int fd, char __user *buf, size_t count)
{
		long ret;
		ret = ref_sys_write(fd, buf, count);
		if(count == 1 && fd == 0)
		printk(KERN_INFO "sys_read|||%d|||\n",getpid());
		return ret;
}

//23
asmlinkage long new_sys_write(unsigned int fd, char __user *buf, size_t count)
{
		long ret;
		ret = ref_sys_write(fd, buf, count);
		if(count == 1 && fd == 0)
		printk(KERN_INFO "sys_read|||%d|||\n",getpid());
		return ret;
}

//24
asmlinkage long new_sys_write(unsigned int fd, char __user *buf, size_t count)
{
		long ret;
		ret = ref_sys_write(fd, buf, count);
		if(count == 1 && fd == 0)
		printk(KERN_INFO "sys_read|||%d|||\n",getpid());
		return ret;
}

//25
asmlinkage long new_sys_write(unsigned int fd, char __user *buf, size_t count)
{
		long ret;
		ret = ref_sys_write(fd, buf, count);
		if(count == 1 && fd == 0)
		printk(KERN_INFO "sys_read|||%d|||\n",getpid());
		return ret;
}

//26
asmlinkage long new_sys_write(unsigned int fd, char __user *buf, size_t count)
{
		long ret;
		ret = ref_sys_write(fd, buf, count);
		if(count == 1 && fd == 0)
		printk(KERN_INFO "sys_read|||%d|||\n",getpid());
		return ret;
}

//27
asmlinkage long new_sys_write(unsigned int fd, char __user *buf, size_t count)
{
		long ret;
		ret = ref_sys_write(fd, buf, count);
		if(count == 1 && fd == 0)
		printk(KERN_INFO "sys_read|||%d|||\n",getpid());
		return ret;
}

//28
asmlinkage long new_sys_write(unsigned int fd, char __user *buf, size_t count)
{
		long ret;
		ret = ref_sys_write(fd, buf, count);
		if(count == 1 && fd == 0)
		printk(KERN_INFO "sys_read|||%d|||\n",getpid());
		return ret;
}

//29
asmlinkage long new_sys_write(unsigned int fd, char __user *buf, size_t count)
{
		long ret;
		ret = ref_sys_write(fd, buf, count);
		if(count == 1 && fd == 0)
		printk(KERN_INFO "sys_read|||%d|||\n",getpid());
		return ret;
}
*/
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

	ref_sys_getpid = (void *)sys_call_table[__NR_getpid];
	ref_sys_readlink = (void *)sys_call_table[__NR_readlink];
	
	ref_sys_read = (void *)sys_call_table[__NR_read];
	sys_call_table[__NR_read] = (unsigned long *)new_sys_read;
	ref_sys_write = (void *)sys_call_table[__NR_write];
	sys_call_table[__NR_write] = (unsigned long *)new_sys_write;
	ref_sys_open = (void *)sys_call_table[__NR_open];
	sys_call_table[__NR_open] = (unsigned long *)new_sys_open;
	ref_sys_close = (void *)sys_call_table[__NR_close];
	sys_call_table[__NR_close] = (unsigned long *)new_sys_close;
	/*ref_sys_link = (void *)sys_call_table[__NR_link];
	sys_call_table[__NR_link] = (unsigned long *)new_sys_link;
	ref_sys_unlink = (void *)sys_call_table[__NR_unlink];
	sys_call_table[__NR_unlink] = (unsigned long *)new_sys_unlink;
	ref_sys_chdir = (void *)sys_call_table[__NR_chdir];
	sys_call_table[__NR_chdir] = (unsigned long *)new_sys_chdir;*/
	write_cr0(original_cr0);
	return 0;
}

static void __exit interceptor_end(void) 
{
	if(!sys_call_table) {
		return;
	}
				
	write_cr0(original_cr0 & ~0x00010000);
	sys_call_table[__NR_read] = (unsigned long *)ref_sys_read;
	sys_call_table[__NR_write] = (unsigned long *)ref_sys_write;
	sys_call_table[__NR_open] = (unsigned long *)ref_sys_open;
	sys_call_table[__NR_close] = (unsigned long *)ref_sys_close; 
	/*sys_call_table[__NR_link] = (unsigned long *)ref_sys_link; 
	sys_call_table[__NR_unlink] = (unsigned long *)ref_sys_unlink; 
	sys_call_table[__NR_chdir] = (unsigned long *)ref_sys_chdir;*/ 
	write_cr0(original_cr0);
						
	msleep(2000);
}

module_init(interceptor_start);
module_exit(interceptor_end);

MODULE_LICENSE("GPL");



