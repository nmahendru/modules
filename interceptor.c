//#################################################################################################
//This module will be used to hookup to system calls and then track them and insert appropriate 
//records which are having information about those system calls being made.
//#################################################################################################
//#################################################################################################
// Documenting the system call design here for use in coding
// name :  sys_manage_sids
// p1 : type of function
//    1. add sid
//    2. remove sid
//    3. check sid
// p2 : input buffer
// p3 : output buffer
// return value :  integer 
// 
//#################################################################################################
//#################################################################################################
// ACTIONS POSSIBLE: This basically lists down the actions possible in every system call:
// 1. MODIFY
// 2. CREATE
// 3. DELETE
// 4. LIKE
// 5. COPY
// 6. SHARE
//#################################################################################################

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
#include <linux/slab.h>
#include <linux/list.h>
#define DB_INSERT_UTIL "/home/nitin/thesis/modules/db_insert.py"
#include "char_dev.h"
#include <linux/unistd.h>
unsigned long **sys_call_table;
unsigned long original_cr0;

LIST_HEAD(sid_linked_list);

MODULE_LICENSE("GPL");

static int umh_test(void){
  struct subprocess_info *sub_info;
  char *argv[] = { "/home/nitin/thesis/modules/db_insert.py", NULL };
  static char *envp[] = {
        "HOME=/",
        "TERM=linux",
        "PATH=/sbin:/bin:/usr/sbin:/usr/bin", NULL };

  sub_info = call_usermodehelper_setup( argv[0], argv, envp, GFP_ATOMIC );
  if (sub_info == NULL) return -ENOMEM;

  call_usermodehelper_exec( sub_info, UMH_WAIT_PROC );
  
  return 0;
}


//TRIED to implement my own list data structure but now I got to that the kernel has its own list thing so I will use that :P

//Adding a structure here to store the list of the current SIDs that are active
struct sid_list{
	struct list_head my_list;
	char * sid;
};
//list of the current sids active in the system at this point

void add_sid_to_list(char * input){
	struct sid_list * elem = (struct sid_list * )kmalloc(sizeof(struct sid_list) , GFP_KERNEL);
		elem->sid = (char *)kmalloc(strlen(input) , GFP_KERNEL);
		strcpy(elem->sid , input);
		
		
	list_add(&elem->my_list , &sid_linked_list);	
}
void remove_sid_from_list(char * input){
	struct sid_list *ptr  = NULL;
	int flag = 0;
	list_for_each_entry(ptr , &sid_linked_list , my_list){
		if(strcmp(ptr->sid , input) == 0) {
			flag = 1;
			break;
		}
	
		if(flag){
			list_del(&ptr->my_list);
			kfree(ptr -> sid);
			kfree(ptr);
		}
	}
}


int check_sid_in_list(char * input){
	struct sid_list *ptr  = NULL;
	//int flag = 0;
	list_for_each_entry(ptr , &sid_linked_list , my_list){
		if(strcmp(ptr->sid , input) == 0) {
			return 1;
		}
	}
	return 0;
}



//READ system call overridden
asmlinkage long (*ref_sys_read)(unsigned int fd, char __user *buf, size_t count); //1
asmlinkage long (*ref_sys_write)(unsigned int fd, char __user *buf, size_t count); //2
asmlinkage long (*ref_sys_open)(const char* filename, int flags, int mode); //3
asmlinkage long (*ref_sys_close)(unsigned int fd); //4
asmlinkage long (*ref_sys_link)(const char * oldname, const char * newname); //5
asmlinkage long (*ref_sys_unlink)(const char  * pathname); //6
asmlinkage long (*ref_sys_chdir)(const char * filename); //7
asmlinkage long (*ref_sys_mknod)(const char *pathname, mode_t mode, dev_t dev); //8
asmlinkage long (*ref_sys_lchown)(const char *path, uid_t owner, gid_t group); //9
asmlinkage long (*ref_sys_lseek)(int fd, off_t offset, int whence); //10
asmlinkage long (*ref_sys_mount)(const char *source, const char *target,
          const char *filesystemtype, unsigned long mountflags,
          const void *data); //11
asmlinkage long (*ref_sys_fstat)(int fd, struct stat *buf); //12
asmlinkage long (*ref_sys_access)(const char *pathname, int mode); //13
asmlinkage long (*ref_sys_sync)(void); //14
asmlinkage long (*ref_sys_mkdir)(const char *pathname, mode_t mode); //15
asmlinkage long (*ref_sys_rmdir)(const char *pathname); //16
asmlinkage long (*ref_sys_chroot)(const char *path); //17
asmlinkage long (*ref_sys_symlink)(const char *target, const char *linkpath); //18
asmlinkage long (*ref_sys_lstat)(const char *pathname, struct stat *buf); //19
asmlinkage long (*ref_sys_readlink)(const char *pathname, char *buf, size_t bufsiz); //20
asmlinkage long (*ref_sys_fchmod)(int fd, mode_t mode); //21
asmlinkage long (*ref_sys_fstatfs)(int fd, struct statfs *buf);//22
asmlinkage long (*ref_sys_statfs)(const char *path, struct statfs *buf); //23
asmlinkage long (*ref_sys_fsync)(int fd); //23
//asmlinkage long (*ref_sys_llseek)(unsigned int fd, unsigned long offset_high,
//                   unsigned long offset_low, loff_t *result,
//`                   unsigned int whence); //24
asmlinkage long (*ref_sys_readv)(int fd, const struct iovec *iov, int iovcnt); //25
asmlinkage long (*ref_sys_writev)(int fd, const struct iovec *iov, int iovcnt); //26
//asmlinkage long (*ref_sys_pread)(int fd, void *buf, size_t count, off_t offset); //27
//asmlinkage long (*ref_sys_pwrite)(int fd, const void *buf, size_t count, off_t offset); //28
asmlinkage long (*ref_sys_chown)(const char *pathname, uid_t owner, gid_t group); //29

//asmlinkage long (*ref_sys_getpid)(void);

asmlinkage long (*ref_sys_manage_sids)(int action , char * input , char * output , int * rValue);

asmlinkage long new_sys_manage_sids(int action , char * input , char * output , int * rValue){
	switch(action){
// Add SID		
		case 1:
			add_sid_to_list(input);
			*rValue = 0;
		break;
// remove SID		
		case 2:
			remove_sid_from_list(input);
			*rValue = 0;
		break;
// check SID
		case 3:
			*rValue = check_sid_in_list(input);
		break;

	} 
	return 0;
}



asmlinkage long new_sys_chown(const char *pathname, uid_t owner, gid_t group){
	long ret;
	struct sid_list *ptr  = NULL;
	char buf[400];
	ret = ref_sys_chown(pathname, owner , group);
	if (strcmp(current->comm , "rsyslogd") == 0 || strcmp(current->comm , "in:imklog") == 0 )
			return ret;
	
	
	list_for_each_entry(ptr , &sid_linked_list , my_list){
		sprintf(buf , "%s$%s$%s" , pathname , ptr->sid , "MODIFY");
		my_char_dev_write_k(buf , strlen(buf));
		umh_test();
	}

	
		////printk(KERN_INFO "thesis_log|||sys_call_name=sys_chown|||process_id=%d|||executable=%s|||filename=%s\n", current->pid , current->comm , pathname);
		return ret;
}

/*asmlinkage long new_sys_pread(int fd, void *buf, size_t count, off_t offset){
	long ret;
	struct file *file;
	char fbuf[200], *realpath;
	ret = ref_sys_pread(fd , buf , count , offset);
	if (strcmp(current->comm , "rsyslogd") == 0 || strcmp(current->comm , "in:imklog") == 0 )
			return ret;

	file = fcheck(fd);
		if(!file){
			////printk(KERN_INFO "thesis_log|||sys_call_name=sys_pread|||process_id=%d|||executable=%s\n", current->pid , current->comm);
			return ret;
		}
		const struct path f_path = file->f_path;
		realpath = d_path(&f_path, fbuf, 200);

		//printk(KERN_INFO "thesis_log|||sys_call_name=sys_pread|||process_id=%d|||executable=%s|||filename=%s\n", current->pid , current->comm , realpath);
		return ret;
} 
asmlinkage long new_sys_pwrite(int fd, const void *buf, size_t count, off_t offset){
	long ret;
	struct file *file;
	char fbuf[200], *realpath;
	ret = ref_sys_pwrite(fd , buf , count , offset);
	if (strcmp(current->comm , "rsyslogd") == 0 || strcmp(current->comm , "in:imklog") == 0 )
			return ret;

	file = fcheck(fd);
		if(!file){
			//printk(KERN_INFO "thesis_log|||sys_call_name=sys_pwrite|||process_id=%d|||executable=%s\n", current->pid , current->comm);
			return ret;
		}
		const struct path f_path = file->f_path;
		realpath = d_path(&f_path, fbuf, 200);

		//printk(KERN_INFO "thesis_log|||sys_call_name=sys_pwrite|||process_id=%d|||executable=%s|||filename=%s\n", current->pid , current->comm , realpath);
		return ret;
}
*/
asmlinkage long new_sys_writev(int fd, const struct iovec *iov, int iovcnt){
	long ret;
	struct file *file;
	char fbuf[200], *realpath;
	ret = ref_sys_writev(fd , iov , iovcnt);
	if (strcmp(current->comm , "rsyslogd") == 0 || strcmp(current->comm , "in:imklog") == 0 )
			return ret;

	file = fcheck(fd);
		if(!file){
			//printk(KERN_INFO "thesis_log|||sys_call_name=sys_writev|||process_id=%d|||executable=%s\n", current->pid , current->comm);
			return ret;
		}
		const struct path f_path = file->f_path;
		realpath = d_path(&f_path, fbuf, 200);

		//printk(KERN_INFO "thesis_log|||sys_call_name=sys_writev|||process_id=%d|||executable=%s|||filename=%s\n", current->pid , current->comm , realpath);
		return ret;
}

asmlinkage long new_sys_readv(int fd, const struct iovec *iov, int iovcnt){
	long ret;
	struct file *file;
	char fbuf[200], *realpath;
	ret = ref_sys_readv(fd , iov , iovcnt);
	if (strcmp(current->comm , "rsyslogd") == 0 || strcmp(current->comm , "in:imklog") == 0 )
			return ret;

	file = fcheck(fd);
		if(!file){
			//printk(KERN_INFO "thesis_log|||sys_call_name=sys_readv|||process_id=%d|||executable=%s\n", current->pid , current->comm);
			return ret;
		}
		const struct path f_path = file->f_path;
		realpath = d_path(&f_path, fbuf, 200);

		//printk(KERN_INFO "thesis_log|||sys_call_name=sys_readv|||process_id=%d|||executable=%s|||filename=%s\n", current->pid , current->comm , realpath);
		return ret;
}



/*asmlinkage long new_sys_llseek(unsigned int fd, unsigned long offset_high,
                   unsigned long offset_low, loff_t *result,
                   unsigned int whence){
	long ret;
	struct file *file;
	char fbuf[200], *realpath;
	ret = ref_sys_llseek(fd, offset_high, offset_low, result, whence);
	if (strcmp(current->comm , "rsyslogd") == 0 || strcmp(current->comm , "in:imklog") == 0 )
			return ret;

	file = fcheck(fd);
		if(!file){
			//printk(KERN_INFO "thesis_log|||sys_call_name=sys_llseek|||process_id=%d|||executable=%s\n", current->pid , current->comm);
			return ret;
		}
		const struct path f_path = file->f_path;
		realpath = d_path(&f_path, fbuf, 200);

		//printk(KERN_INFO "thesis_log|||sys_call_name=sys_llseek|||process_id=%d|||executable=%s|||filename=%s\n", current->pid , current->comm , realpath);
		return ret;

}
*/
asmlinkage long new_sys_fsync(int fd){
	long ret;
	struct file *file;
	char fbuf[200], *realpath;
	ret = ref_sys_fsync(fd);
	if (strcmp(current->comm , "rsyslogd") == 0 || strcmp(current->comm , "in:imklog") == 0 )
			return ret;

	file = fcheck(fd);
		if(!file){
			//printk(KERN_INFO "thesis_log|||sys_call_name=sys_fsync|||process_id=%d|||executable=%s\n", current->pid , current->comm);
			return ret;
		}
		const struct path f_path = file->f_path;
		realpath = d_path(&f_path, fbuf, 200);

		//printk(KERN_INFO "thesis_log|||sys_call_name=sys_fsync|||process_id=%d|||executable=%s|||file synced=%s\n", current->pid , current->comm , realpath);
		return ret;
}

asmlinkage long new_sys_statfs(const char *path, struct statfs *buf){
	long ret;
	ret = ref_sys_statfs(path, buf);
	if (strcmp(current->comm , "rsyslogd") == 0 || strcmp(current->comm , "in:imklog") == 0 )
			return ret;

		//printk(KERN_INFO "thesis_log|||sys_call_name=sys_statfs|||process_id=%d|||executable=%s|||file statted=%s\n", current->pid , current->comm , path);
		return ret;
}

asmlinkage long new_sys_fstatfs(int fd, struct statfs *buf){
	long ret;
	struct file *file;
	char fbuf[200], *realpath;
	ret = ref_sys_fstatfs(fd, buf);

	file = fcheck(fd);
		if(!file){
			//printk(KERN_INFO "thesis_log|||sys_call_name=sys_fstatfs|||process_id=%d|||executable=%s\n", current->pid , current->comm);
			return ret;
		}
		const struct path f_path = file->f_path;
		realpath = d_path(&f_path, fbuf, 200);
	if (strcmp(current->comm , "rsyslogd") == 0 || strcmp(current->comm , "in:imklog") == 0 )
			return ret;

		//printk(KERN_INFO "thesis_log|||sys_call_name=sys_fstatfs|||process_id=%d|||executable=%s|||filename=%s\n", current->pid , current->comm , realpath);
		return ret;
}

asmlinkage long new_sys_fchmod(int fd, mode_t mode){
	long ret;
	struct file *file;
	char fbuf[200], *realpath;
	ret = ref_sys_fchmod(fd, mode);

	file = fcheck(fd);
		if(!file){
			//printk(KERN_INFO "thesis_log|||sys_call_name=sys_fchmod|||process_id=%d|||executable=%s\n", current->pid , current->comm);
			return ret;
		}
		const struct path f_path = file->f_path;
		realpath = d_path(&f_path, fbuf, 200);
	if (strcmp(current->comm , "rsyslogd") == 0 || strcmp(current->comm , "in:imklog") == 0 )
			return ret;

		//printk(KERN_INFO "thesis_log|||sys_call_name=sys_fchmod|||process_id=%d|||executable=%s|||filename=%s\n", current->pid , current->comm , realpath);
		return ret;
}
asmlinkage long new_sys_readlink(const char *pathname, char *buf, size_t bufsiz){
	long ret;
	ret = ref_sys_readlink(pathname, buf , bufsiz);
	if (strcmp(current->comm , "rsyslogd") == 0 || strcmp(current->comm , "in:imklog") == 0 )
			return ret;

		//printk(KERN_INFO "thesis_log|||sys_call_name=sys_readlink|||process_id=%d|||executable=%s|||link read=%s\n", current->pid , current->comm , pathname);
		return ret;
}

asmlinkage long new_sys_lstat(const char *pathname, struct stat *buf){
	long ret;
	ret = ref_sys_lstat(pathname, buf);
	if (strcmp(current->comm , "rsyslogd") == 0 || strcmp(current->comm , "in:imklog") == 0 )
			return ret;

		//printk(KERN_INFO "thesis_log|||sys_call_name=sys_lstat|||process_id=%d|||executable=%s|||file statted=%s\n", current->pid , current->comm , pathname);
		return ret;
}



asmlinkage long new_sys_symlink(const char *target, const char *linkpath){
	long ret;
	ret = ref_sys_symlink(target , linkpath);
	if (strcmp(current->comm , "rsyslogd") == 0 || strcmp(current->comm , "in:imklog") == 0 )
			return ret;

		//printk(KERN_INFO "thesis_log|||sys_call_name=sys_symlink|||process_id=%d|||executable=%s|||created link=%s|||actual util linked=%s\n", current->pid , current->comm , linkpath , target);
		return ret;
}



asmlinkage long new_sys_chroot(const char *path){
	long ret;
	ret = ref_sys_chroot(path);
	if (strcmp(current->comm , "rsyslogd") == 0 || strcmp(current->comm , "in:imklog") == 0 )
			return ret;

		//printk(KERN_INFO "thesis_log|||sys_call_name=sys_chroot|||process_id=%d|||executable=%s|||pathname=%s\n", current->pid , current->comm , pathname);
		return ret;
}


asmlinkage long new_sys_rmdir(const char *pathname){
	long ret;
	ret = ref_sys_rmdir(pathname);
	if (strcmp(current->comm , "rsyslogd") == 0 || strcmp(current->comm , "in:imklog") == 0 )
			return ret;

		//printk(KERN_INFO "thesis_log|||sys_call_name=sys_rmdir|||process_id=%d|||executable=%s|||filename=%s\n", current->pid , current->comm , pathname);
		return ret;
}

asmlinkage long new_sys_mkdir(const char *pathname, mode_t mode){
	long ret;
	ret = ref_sys_mkdir(pathname, mode);
	if (strcmp(current->comm , "rsyslogd") == 0 || strcmp(current->comm , "in:imklog") == 0 )
			return ret;

		//printk(KERN_INFO "thesis_log|||sys_call_name=ref_sys_mkdir|||process_id=%d|||executable=%s|||filename=%s\n", current->pid , current->comm , pathname);
		return ret;

}

asmlinkage long new_sys_access(const char *pathname, int mode){
	long ret;
	ret = ref_sys_access(pathname , mode);
	if (strcmp(current->comm , "rsyslogd") == 0 || strcmp(current->comm , "in:imklog") == 0 )
			return ret;

		//printk(KERN_INFO "thesis_log|||sys_call_name=sys_access|||process_id=%d|||executable=%s|||filename=%s\n", current->pid , current->comm , pathname);
		return ret;
}

asmlinkage long new_sys_sync(void){
	long ret;
	ret = ref_sys_sync();
	//printk(KERN_INFO "thesis_log|||sys_call_name=sys_sync||process_id=%d|||executable=%s\n");
	return ret;
}


asmlinkage long new_sys_fstat(int fd, struct stat *buf){
	long ret;

	struct file *file;
	char fbuf[200], *realpath;
	ret = ref_sys_fstat(fd , buf);

	file = fcheck(fd);
		if(!file){
			//printk(KERN_INFO "thesis_log|||sys_call_name=sys_fstat|||process_id=%d|||executable=%s\n", current->pid , current->comm);
			return ret;
		}
		const struct path f_path = file->f_path;
		realpath = d_path(&f_path, fbuf, 200);
	if (strcmp(current->comm , "rsyslogd") == 0 || strcmp(current->comm , "in:imklog") == 0 )
			return ret;

		//printk(KERN_INFO "thesis_log|||sys_call_name=sys_fstat|||process_id=%d|||executable=%s|||filename=%s\n", current->pid , current->comm , realpath);
		return ret;
}

asmlinkage long new_sys_mount(const char *source, const char *target,
          const char *filesystemtype, unsigned long mountflags,
          const void *data){
	long ret;
	ret = ref_sys_mount(source,target,
          filesystemtype, mountflags,
          data);
	if (strcmp(current->comm , "rsyslogd") == 0 || strcmp(current->comm , "in:imklog") == 0 )
			return ret;

		//printk(KERN_INFO "thesis_log|||sys_call_name=sys_mount|||process_id=%d|||executable=%s|||mount name=%s|||actual thing mounted=%s\n", current->pid , current->comm , source , target);
	return ret;
}

asmlinkage long new_sys_lseek(int fd, off_t offset, int whence){
	long ret;

	ret = ref_sys_lseek(fd , offset, whence);
	struct file *file;
		char fbuf[200], *realpath;
		ret = ref_sys_lseek(fd, offset, whence);
		
		if (strcmp(current->comm , "rsyslogd") == 0 || strcmp(current->comm , "in:imklog") == 0 )
			return ret;
		
		file = fcheck(fd);
		if(!file){
			//printk(KERN_INFO "thesis_log|||sys_call_name=sys_lseek|||process_id=%d|||executable=%s\n", current->pid , current->comm);
			return ret;
		}
		const struct path f_path = file->f_path;
		realpath = d_path(&f_path, fbuf, 200);
		//printk(KERN_INFO "thesis_log|||sys_call_name=sys_lseek|||process_id=%d|||executable=%s|||file_written=%s\n", current->pid , current->comm , realpath);
	    return ret;

} //10

asmlinkage long new_sys_lchown(const char *path, uid_t owner, gid_t group){
	long ret;
	ret = ref_sys_lchown(path , owner, group);
	if (strcmp(current->comm , "rsyslogd") == 0 || strcmp(current->comm , "in:imklog") == 0 )
			return ret;

		//printk(KERN_INFO "thesis_log|||sys_call_name=sys_chown|||process_id=%d|||executable=%s|||file_written=%s\n", current->pid , current->comm , path);

		return ret;
}
asmlinkage long new_sys_mknod(const char *pathname, mode_t mode, dev_t dev){
	long ret;
	ret = ref_sys_mknod(pathname , mode , dev);
	if (strcmp(current->comm , "rsyslogd") == 0 || strcmp(current->comm , "in:imklog") == 0 )
			return ret;

		//printk(KERN_INFO "thesis_log|||sys_call_name=sys_mknod|||process_id=%d|||executable=%s|||file_written=%s\n", current->pid , current->comm , pathname);
		return ret;

}
//1



asmlinkage long new_sys_read(unsigned int fd, char __user *buf, size_t count)
{
		long ret;
		struct file *file;
		char fbuf[200], *realpath;
		ret = ref_sys_read(fd, buf, count);
		
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
		struct sid_list *ptr  = NULL;
		char buf[400];
		ret = ref_sys_open(filename, flags, mode);
		if (strcmp(current->comm , "rsyslogd") == 0 || strcmp(current->comm , "in:imklog") == 0 )
			return ret;
		list_for_each_entry(ptr , &sid_linked_list , my_list){
		sprintf(buf , "%s$%s$%s" , filename , ptr->sid , "OPEN");
		my_char_dev_write_k(buf , strlen(buf));
		umh_test();
		
	}
	printk(KERN_INFO "thesis: open called for %s\n" , filename);
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




//5
asmlinkage long new_sys_link(const char * oldname , const char * newname)
{
		long ret;
		ret = ref_sys_link(oldname , newname);
		if (strcmp(current->comm , "rsyslogd") == 0 || strcmp(current->comm , "in:imklog") == 0 )
			return ret;
		//printk(KERN_INFO "thesis_log|||sys_call_name=sys_link|||process_id=%d|||executable=%s|||oldname=%s|||newname=%s\n", current->pid, current->comm , oldname , newname);
		return ret;
		
}

//6
asmlinkage long new_sys_unlink(const char * pathname)
{
		long ret;
		ret = ref_sys_unlink(pathname);
		if (strcmp(current->comm , "rsyslogd") == 0 || strcmp(current->comm , "in:imklog") == 0 )
			return ret;
		//printk(KERN_INFO "thesis_log|||sys_call_name=sys_unlink|||process_id=%d|||executable=%s|||file_unlinked=%s\n", current->pid, current->comm , pathname);
		
		return ret;
}

//7
asmlinkage long new_sys_chdir(const char * filename)
{
		long ret;
		ret = ref_sys_chdir(filename);
		if (strcmp(current->comm , "rsyslogd") == 0 || strcmp(current->comm , "in:imklog") == 0 )
			return ret;
		//printk(KERN_INFO "thesis_log|||sys_call_name=sys_chdir|||process_id=%d|||executable=%s|||target_dir=%s\n", current->pid, current->comm , filename);
		
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

	//ref_sys_getpid = (void *)sys_call_table[__NR_getpid];
	ref_sys_readlink = (void *)sys_call_table[__NR_readlink];
	
	ref_sys_read = (void *)sys_call_table[__NR_read];
	sys_call_table[__NR_read] = (unsigned long *)new_sys_read;
	ref_sys_write = (void *)sys_call_table[__NR_write];
	sys_call_table[__NR_write] = (unsigned long *)new_sys_write;
	ref_sys_open = (void *)sys_call_table[__NR_open];
	sys_call_table[__NR_open] = (unsigned long *)new_sys_open;
	ref_sys_close = (void *)sys_call_table[__NR_close];
	sys_call_table[__NR_close] = (unsigned long *)new_sys_close;
	ref_sys_link = (void *)sys_call_table[__NR_link];
	sys_call_table[__NR_link] = (unsigned long *)new_sys_link;
	ref_sys_unlink = (void *)sys_call_table[__NR_unlink];
	sys_call_table[__NR_unlink] = (unsigned long *)new_sys_unlink;
	ref_sys_chdir = (void *)sys_call_table[__NR_chdir];
	sys_call_table[__NR_chdir] = (unsigned long *)new_sys_chdir;
	ref_sys_mknod = (void *)sys_call_table[__NR_mknod];
	ref_sys_lchown = (void *)sys_call_table[__NR_lchown];
	ref_sys_lseek = (void *)sys_call_table[__NR_lseek];
	ref_sys_mount = (void *)sys_call_table[__NR_mount];
	ref_sys_fstat = (void *)sys_call_table[__NR_fstat];
	ref_sys_access = (void *)sys_call_table[__NR_access];
	ref_sys_sync = (void *)sys_call_table[__NR_sync];
	ref_sys_mkdir = (void *)sys_call_table[__NR_mkdir];
	ref_sys_rmdir = (void *)sys_call_table[__NR_rmdir];
	ref_sys_chroot = (void *)sys_call_table[__NR_chroot];
	ref_sys_symlink = (void *)sys_call_table[__NR_symlink];
	ref_sys_lstat = (void *)sys_call_table[__NR_lstat];
	ref_sys_readlink = (void *)sys_call_table[__NR_readlink];
	ref_sys_fchmod = (void *)sys_call_table[__NR_fchmod];
	ref_sys_fstatfs = (void *)sys_call_table[__NR_fstatfs];
	ref_sys_statfs = (void *)sys_call_table[__NR_statfs];
	ref_sys_fsync = (void *)sys_call_table[__NR_fsync];
	//ref_sys_llseek = (void *)sys_call_table[__NR_llseek];
	ref_sys_readv = (void *)sys_call_table[__NR_readv];
	ref_sys_writev = (void *)sys_call_table[__NR_writev];
	//ref_sys_pread = (void *)sys_call_table[__NR_pread];
	//ref_sys_pwrite = (void *)sys_call_table[__NR_pwrite];
	ref_sys_chown = (void *)sys_call_table[__NR_chown];
	//ref_sys_getpid = (void *)sys_call_table[__NR_getpid];
	ref_sys_manage_sids = (void *)sys_call_table[__NR_manage_sids];


	sys_call_table[__NR_mknod] = (unsigned long *)new_sys_mknod;
	sys_call_table[__NR_lchown] = (unsigned long *)new_sys_lchown;
	sys_call_table[__NR_lseek] = (unsigned long *)new_sys_lseek;
	sys_call_table[__NR_mount] = (unsigned long *)new_sys_mount;
	sys_call_table[__NR_fstat] = (unsigned long *)new_sys_fstat;
	sys_call_table[__NR_access] = (unsigned long *)new_sys_access;
	sys_call_table[__NR_sync] = (unsigned long *)new_sys_sync;
	sys_call_table[__NR_mkdir] = (unsigned long *)new_sys_mkdir;
	sys_call_table[__NR_rmdir] = (unsigned long *)new_sys_rmdir;
	sys_call_table[__NR_chroot] = (unsigned long *)new_sys_chroot;
	sys_call_table[__NR_symlink] = (unsigned long *)new_sys_symlink;
	sys_call_table[__NR_lstat] = (unsigned long *)new_sys_lstat;
	sys_call_table[__NR_readlink] = (unsigned long *)new_sys_readlink;
	sys_call_table[__NR_fchmod] = (unsigned long *)new_sys_fchmod;
	sys_call_table[__NR_fstatfs] = (unsigned long *)new_sys_fstatfs;
	sys_call_table[__NR_statfs] = (unsigned long *)new_sys_statfs;
	sys_call_table[__NR_fsync] = (unsigned long *)new_sys_fsync;
	//sys_call_table[__NR_llseek] = (unsigned long *)new_sys_llseek;
	sys_call_table[__NR_readv] = (unsigned long *)new_sys_readv;
	sys_call_table[__NR_writev] = (unsigned long *)new_sys_writev;
	//sys_call_table[__NR_pread] = (unsigned long *)new_sys_pread;
	//sys_call_table[__NR_pwrite] = (unsigned long *)new_sys_pwrite;
	sys_call_table[__NR_chown] = (unsigned long *)new_sys_chown;
	//sys_call_table[__NR_getpid] = (unsigned long *)new_sys_getpid;
	sys_call_table[__NR_manage_sids] = (unsigned long *)new_sys_manage_sids;
	write_cr0(original_cr0);

	 //this is to initiate the linked list of SIDs
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
	sys_call_table[__NR_link] = (unsigned long *)ref_sys_link; 
	sys_call_table[__NR_unlink] = (unsigned long *)ref_sys_unlink; 
	sys_call_table[__NR_chdir] = (unsigned long *)ref_sys_chdir; 
	sys_call_table[__NR_mknod] =  (unsigned long * ) ref_sys_mknod;
	sys_call_table[__NR_lchown] =  (unsigned long * ) ref_sys_lchown;
	sys_call_table[__NR_lseek] =  (unsigned long * ) ref_sys_lseek;
	sys_call_table[__NR_mount] =  (unsigned long * ) ref_sys_mount;
	sys_call_table[__NR_fstat] =  (unsigned long * ) ref_sys_fstat;
	sys_call_table[__NR_access] =  (unsigned long * ) ref_sys_access;
	sys_call_table[__NR_sync] =  (unsigned long * ) ref_sys_sync;
	sys_call_table[__NR_mkdir] =  (unsigned long * ) ref_sys_mkdir;
	sys_call_table[__NR_rmdir] =  (unsigned long * ) ref_sys_rmdir;
	sys_call_table[__NR_chroot] =  (unsigned long * ) ref_sys_chroot;
	sys_call_table[__NR_symlink] =  (unsigned long * ) ref_sys_symlink;
	sys_call_table[__NR_lstat] =  (unsigned long * ) ref_sys_lstat;
	sys_call_table[__NR_readlink] =  (unsigned long * ) ref_sys_readlink;
	sys_call_table[__NR_fchmod] =  (unsigned long * ) ref_sys_fchmod;
	sys_call_table[__NR_fstatfs] =  (unsigned long * ) ref_sys_fstatfs;
	sys_call_table[__NR_statfs] =  (unsigned long * ) ref_sys_statfs;
	sys_call_table[__NR_fsync] =  (unsigned long * ) ref_sys_fsync;
	//sys_call_table[__NR_llseek] =  (unsigned long * ) ref_sys_llseek;
	sys_call_table[__NR_readv] =  (unsigned long * ) ref_sys_readv;
	sys_call_table[__NR_writev] =  (unsigned long * ) ref_sys_writev;
	//sys_call_table[__NR_pread] =  (unsigned long * ) ref_sys_pread;
	//sys_call_table[__NR_pwrite] =  (unsigned long * ) ref_sys_pwrite;
	sys_call_table[__NR_chown] =  (unsigned long * ) ref_sys_chown;
	//sys_call_table[__NR_getpid] =  (unsigned long * ) ref_sys_getpid;
	sys_call_table[__NR_manage_sids] =  (unsigned long * ) ref_sys_manage_sids;
	write_cr0(original_cr0);
						
	msleep(2000);
}

module_init(interceptor_start);
module_exit(interceptor_end);
