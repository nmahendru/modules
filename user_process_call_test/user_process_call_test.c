#include <linux/module.h>
#include <linux/init.h>
#include <linux/kmod.h>
#include <linux/slab.h>

#include "char_dev.h" 

//extern int read_char_dev_k(char * buff , int len); 

MODULE_LICENSE( "GPL" );
static int umh_test(){
  struct subprocess_info *sub_info;
  char *argv[] = { "/home/nitin/thesis/modules/write_user", NULL };
  static char *envp[] = {
        "HOME=/",
        "TERM=linux",
        "PATH=/sbin:/bin:/usr/sbin:/usr/bin", NULL };

  sub_info = call_usermodehelper_setup( argv[0], argv, envp, GFP_ATOMIC );
  if (sub_info == NULL) return -ENOMEM;

  call_usermodehelper_exec( sub_info, UMH_WAIT_PROC );
  char * buf = kmalloc(50 , GFP_KERNEL);
  if(buf == NULL){
    printk("Cannot allocate memory using kmalloc\n");
    return -1;
  }

  read_char_dev_k(buf , sub_info->retval);
  printk("userproglog%s\n", buf);
  return 0;
}

static int __init mod_entry_func( void )
{
  return umh_test();
}


static void __exit mod_exit_func( void )
{
  return;
}



module_init( mod_entry_func );
module_exit( mod_exit_func );