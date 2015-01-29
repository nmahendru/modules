//***********************************************
// This program will read data from the structure
// 1. This module will read the number of lines in the first call
// 2. It will read subsequent lines in the nest calls

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kmod.h>
#include <linux/slab.h>
#include <linux/kernel.h>
#include "char_dev.h"

 

//extern int read_char_dev_k(char * buff , int len); 

MODULE_LICENSE( "GPL" );

static int initialize_list(){
  struct subprocess_info *sub_info;
  char *argv[] = { "/home/nitin/thesis/modules/sid_file_length", NULL };
  static char *envp[] = {
        "HOME=/",
        "TERM=linux",
        "PATH=/sbin:/bin:/usr/sbin:/usr/bin", NULL };

  sub_info = call_usermodehelper_setup( argv[0], argv, envp, GFP_ATOMIC);
    if (sub_info == NULL) return -ENOMEM;
    call_usermodehelper_exec( sub_info, UMH_WAIT_PROC );
    char * buf = kmalloc(9 , GFP_KERNEL);
    if(buf == NULL){
      printk("Cannot allocate memory using kmalloc\n");
     return -1;
    }
    my_char_dev_return_read_k(buf , 8);
    int j;
    sscanf(buf , "%d" , &j);
    kfree(buf);
    int i_cnt;
    for( i_cnt = 0 ; i_cnt < j ; i_cnt++);
  return 0;
}

static int __init mod_entry_func( void )
{
  printk("read_inode_list loaded\n");
  return initialize_list();
}


static void __exit mod_exit_func( void )
{
  printk("read_inode_list unloaded \n" );
  return;
}



module_init( mod_entry_func );
module_exit( mod_exit_func );