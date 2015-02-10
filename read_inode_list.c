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
/**************************************************************************************************
List of defines used as constants
**************************************************************************************************/
#define HASH_SIZE 1000
/**************************************************************************************************
Global declarations for this module.
**************************************************************************************************/
static struct sid_obj **  sid_hash_k = NULL;



/***************************************************************************************************
Writing my own version of kmalloc to check if the memory allocation was successful or not and try to
reallocate automatically with 5 retries else print a message to the kernel log
***************************************************************************************************/
void * local_kmalloc(int size){
  void * ptr = kmalloc(size , GFP_KERNEL);
  
  if(!ptr){
    int i = 0;
    while(i < 5){
      ptr = kmalloc(size , GFP_KERNEL);
      if(ptr) break ;
    }
  }
  return ptr;
}


/***************************************************************************************************
This section of the code basically builds a hashing API that will be used to cater to the hashed 
list of inodes to be stored in the kernel memory. The memory here refers to the kernel memory

***************************************************************************************************/
static struct sid_obj{
  unsigned long inode_number;
  char * sids[];
  int n_sids;
  struct sid_obj * next; 
};





/**************************************************************************************************
add_node
This function adds a node to the has data structure and then returns without any value.

**************************************************************************************************/
void add_node(struct sid_obj * ptr){
  int hash_value = ptr->inode_number % HASH_SIZE ;
  if(!sid_hash_k[hash_value]){
    sid_hash_k[hash_value] = ptr;
  }else{
    struct sid_obj * entry_ptr = sid_hash_k[hash_value];
    while(entry_ptr->next) entry_ptr = entry_ptr->next;
    entry_ptr->next = ptr;
  }

}

struct sid_obj * get_node(unsigned long inode_number){
  int hash_value = inode_number % HASH_SIZE;
  if(!sid_hash_k[hash_value]) return NULL;
  else{
    while(entry_ptr->next != NULL && entry_ptr->inode_number != inode_number) 
      entry_ptr = entry_ptr->next;
    if(entry_ptr->inode_number == inode_number) return entry_pointer;
    else return null;
  }
}







/***************************************************************************************************
Hashing code section ends
***************************************************************************************************/
 

//extern int read_char_dev_k(char * buff , int len); 

MODULE_LICENSE( "GPL" );
static replace_1_with_0(char * buff){
  int strl = strlen(buff);
  int i ;
  for(i = 0 ; i < strl ; i++){
    if(buff[i] == (char)1) buff[i] = '\0';
  }
}





/***************************************************************************************************
read_one_line
This function will read one line from the char device, create a node and then add it to the kernel 
hash so that now it's ready for access when checked by hooked system calls
***************************************************************************************************/
static int read_one_line(char * buff){
  int strl = strlen(buff);
  read_one_line(buff);
  unsigned long inode_number;
  int r_value = 0;
  int num_sids;
  // read the inode number from the buffer
  r_value = sscanf(buff , "%d" , num_sids );
  buff += r_value;
  r_value = sscanf(buff , "%lu" , inode_number);
  buff += r_value;
  struct sid_obj * obj_ptr = (struct sid_obj*)local_kmalloc(sizeof(struct sid_obj));
  if(!obj_ptr){
    printk("thesis: Cannot allocate memory for inode object for inode# = %lu\n" , inode_number);
    return -1;
  }
  obj_ptr->inode_number = inode_number;
  obj_ptr->n_sids = num_sids;
  obj_ptr->sids = local_kmalloc(sizeof(char *) * num_sids);
  if(!obj_ptr->sids){
    printk("thesis: Cannot allocate memory for the sid array for inode = %lu\n" , inode_number);
    return -1 ;
  }
  char sid_value_buff[20] = {0};
  int i = 0;
  while(i < num_sids){
    r_value = sscanf(buff , "%s" , sid_value_buff);
    buff += r_value; buff += 1;
    char * sid_value = local_kmalloc(strlen(sid_value_buff) + 1);
    if(!sid_value){
      printk("thesis: Cannot allocate memory for sid value = %s for inode# = %lu. Need to abort now\n" , sid_value_buff , inode_number);
      return -1;
    }

    strcpy(sid_value , sid_value_buff);
    obj_ptr->sids[i++] = sid_value;
  }

// Add the read sid node to the hash in the kernel  
add_node(obj_ptr);
}

static void check_if_line_written(){
  char * buff = kmalloc(9 , GFP_KERNEL);
  my_char_dev_return_read_k(buff, 8);

  while(strncmp(buff , "done" , 4) != 0){
    my_char_dev_return_read_k(buff, 8);
  } 
  kfree(buff);
}

static inform_user_line_read(){
  char temp[] = {'d' , 'o' , 'n' , 'e' , '\0'};
  my_char_dev_return_write_k(temp, 4);
}
/*************************************************************************************************
initialize_list()
Main function call for all groundwork to be done at bootup
*************************************************************************************************/
static int initialize_list(){
  struct subprocess_info *sub_info;
  char *argv[] = { "/home/nitin/thesis/modules/sid_file_length", NULL };
  static char *envp[] = {
        "HOME=/",
        "TERM=linux",
        "PATH=/sbin:/bin:/usr/sbin:/usr/bin", NULL };

  sub_info = call_usermodehelper_setup( argv[0], argv, envp, GFP_ATOMIC);
    if (sub_info == NULL) return -ENOMEM;
    call_usermodehelper_exec( sub_info, UMH_WAIT_PROC);
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

/**************************************************************************************************
Allocating an array of pointers to store the various hashed lists and then use that in retrieval of
data and other things. Allocating an array of HASH_SIZE pointers. the last three digits of the inode 
number will be used as a hash to this array. Link list to be used for collision resolution.
**************************************************************************************************/

  sid_hash_k = (struct sid_obj ** ) local_kmalloc(sizeof(struct sid_obj *) * HASH_SIZE , GFP_KERNEL );
  if(!sid_hash_k){
  printk("thesis: memory allocation for sid_hash array failed. Need to abort now\n");
  return -1;
  }
//change the argv to point to the new user mode program
    argv = {"/home/nitin/thesis/modules/readInodes" , NULL};
    sub_info = call_usermodehelper_setup( argv[0], argv, envp, GFP_ATOMIC);
    if (sub_info == NULL) return -ENOMEM;
    call_usermodehelper_exec( sub_info, UMH_NO_WAIT );  



//read the lines of sid file    
    for( i_cnt = 0 ; i_cnt < j ; i_cnt++){
//check if user mode program has written the line
          check_if_line_written();
//code to read the line from the device

          char * line_buff = kmalloc(1024 , GFP_kernel);
          my_char_dev_read_k(line_buff, 1023);
          if(read_one_line(line_buff) == -1) return -1;




//*******code to read the line ends
//inform the user mode program the line has been read an wait for the next line.
          inform_user_line_read();
    }
  return 0;
}
/***************************************************************************************************
write_back_list
This function will basically dump out the sid list which is there write now in the kernel memory
to a file in userspace.
***************************************************************************************************/
static int write_back_list(){

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