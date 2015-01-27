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
    char * buf = kmalloc(8 , GFP_KERNEL);
    if(buf == NULL){
      printk("Cannot allocate memory using kmalloc\n");
     return -1;
    }
    my_char_dev_return_read_k(buf , 7);
    /*struct return_value{
      short a;
    };
    struct return_value * temp = (struct return_value *) buf;
    *///int number_of_lines = 0;
    //number_of_lines = *buf;
    //kfree(buf);
    
    int i;
    sscanf(buf , "%d" , &i);
    for( i = 0 ; i < 8 ; i++){
      printk("%c",buf[i]);
    }
    printk("\n");
    //printk("value read of the return device is %d\n" , temp->a);
    printk("value as an int is %d\n" , i);
    kfree(buf);
  return 0;
}

static int __init mod_entry_func( void )
{
  return initialize_list();
}


static void __exit mod_exit_func( void )
{
  return;
}



module_init( mod_entry_func );
module_exit( mod_exit_func );