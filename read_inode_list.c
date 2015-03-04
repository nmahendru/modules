/**************************************************************************************************
This program will read data from the structure
1. This module will read the number of lines in the first call
2. It will read subsequent lines in the


READ THIS WHEN YOU START REPROGRAMMING THIS CODE AS THIS WILL HELP!!!!!
The program will start with 
initialize_list()
This function will basically read the inode list in to the kernel memory and then store in a hashed 
data structure. This will basically call different functions to read the inode file in user space
and then read that in line by line using a character device


write_back_list()
oppositte function of the above function


**************************************************************************************************/
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kmod.h>
#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include "char_dev.h"
#include <net/genetlink.h>
#include "netlink_kernel_module.c"

/**************************************************************************************************
List of defines used as constants
**************************************************************************************************/
#define HASH_SIZE 1000
/**************************************************************************************************
Global declarations for this module.
**************************************************************************************************/
static struct sid_obj **  sid_hash_k = NULL;



/***************************************************************************************************
This section of the code basically builds a hashing API that will be used to cater to the hashed 
list of inodes to be stored in the kernel memory. The memory here refers to the kernel memory

***************************************************************************************************/
struct sid_obj{
  unsigned long inode_number;
  
  int n_sids;
  struct sid_obj * next; 
  char ** sids;
};

static void replace_1_with_0(char * buff){
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
  //int strl;
  int i;
  unsigned long inode_number;
  int r_value;
  int num_sids;
  struct sid_obj * obj_ptr;
  char sid_value_buff[20];
  //strl = strlen(buff);
  printk("thesis:read one line called \n");
  replace_1_with_0(buff);
  // read the inode number from the buffer
  r_value = sscanf(buff , "%d" , &num_sids );
  buff += r_value;
  r_value = sscanf(buff , "%lu" , &inode_number);
  buff += r_value;
  obj_ptr = (struct sid_obj*)vmalloc(sizeof(struct sid_obj));
  obj_ptr->inode_number = inode_number;
  obj_ptr->n_sids = num_sids;
  obj_ptr->sids = (char **)vmalloc(sizeof(char *) * num_sids);
  i = 0;
  while(i < num_sids){
    r_value = sscanf(buff , "%s" , sid_value_buff);
    buff += r_value;
    char * sid_value = vmalloc(strlen(sid_value_buff) + 1);
    strcpy(sid_value , sid_value_buff);
    obj_ptr->sids[i++] = sid_value;
  }

// Add the read sid node to the hash in the kernel  
//add_node(obj_ptr);
}



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
    struct sid_obj * entry_ptr = sid_hash_k[hash_value];
    while(entry_ptr->next != NULL && entry_ptr->inode_number != inode_number) 
      entry_ptr = entry_ptr->next;
    if(entry_ptr->inode_number == inode_number) return entry_ptr;
    else return NULL;
  }
}







/***************************************************************************************************
Hashing code section ends
***************************************************************************************************/
 


/***************************************************************************************************
Initializer code for netlink socket

***************************************************************************************************/

int doc_exmpl_echo(struct sk_buff *skb_2, struct genl_info *info)
{

        struct nlattr *na;
        struct sk_buff *skb;
        int rc;
  void *msg_head;
  char * mydata;
  char * line_buff ;
  printk("doc example function called\n");
  
        if (info == NULL)
                goto out;
  
        /*for each attribute there is an index in info->attrs which points to a nlattr structure
         *in this structure the data is given
         */
        na = info->attrs[DOC_EXMPL_A_MSG];
        if (na) {
    mydata = (char *)nla_data(na);
    if (mydata == NULL)
      printk("error while receiving data\n");
    else
      printk("received: %s\n", mydata);
    }
  else
    printk("no info->attrs %i\n", DOC_EXMPL_A_MSG);




/**********************************************************************************************************
code for reading one line from the device
**********************************************************************************************************/

          line_buff = vmalloc(1024);
          printk("thesis: memory allocated for one\n");
          my_char_dev_read_k(line_buff, 1023);
          printk("thesis: line read from user space =%s\n" , line_buff);
          read_one_line(line_buff);


          vfree(line_buff);
/**********************************************************************************************************
code for reading one line from the device ends 
**********************************************************************************************************/
        /* send a message back*/
        /* allocate some memory, since the size is not yet known use NLMSG_GOODSIZE*/ 
        skb = genlmsg_new(NLMSG_GOODSIZE, GFP_KERNEL);
  if (skb == NULL)
    goto out;

  /* create the message headers */
        /* arguments of genlmsg_put: 
           struct sk_buff *, 
           int (sending) pid, 
           int sequence number, 
           struct genl_family *, 
           int flags, 
           u8 command index (why do we need this?)
        */
        msg_head = genlmsg_put(skb, 0, info->snd_seq+1, &doc_exmpl_gnl_family, 0, DOC_EXMPL_C_ECHO);
  if (msg_head == NULL) {
    rc = -ENOMEM;
    goto out;
  }
  /* add a DOC_EXMPL_A_MSG attribute (actual value to be sent) */
  rc = nla_put_string(skb, DOC_EXMPL_A_MSG, "kernel:read one line");
  if (rc != 0)
    goto out;
  
        /* finalize the message */
  genlmsg_end(skb, msg_head);

        /* send the message back */
  //rc = genlmsg_unicast(skb,info->snd_pid );
  rc = genlmsg_unicast(genl_info_net(info), skb,info->snd_pid );
  if (rc != 0)
    goto out;
  return 0;

 out:
        printk("an error occured in doc_exmpl_echo:\n");
  
      return 0;
}
/* commands: mapping between the command enumeration and the actual function*/
struct genl_ops doc_exmpl_gnl_ops_echo = {
  .cmd = DOC_EXMPL_C_ECHO,
  .flags = 0,
  .policy = doc_exmpl_genl_policy,
  .doit = doc_exmpl_echo,
  .dumpit = NULL,
};

/***************************************************************************************************
initializer code ends
***************************************************************************************************/



static int __init gnKernel_init(void)
{
  int rc;
        printk("INIT GENERIC NETLINK EXEMPLE MODULE\n");
        
        /*register new family*/
  rc = genl_register_family(&doc_exmpl_gnl_family);
  if (rc != 0)
    goto failure;
        /*register functions (commands) of the new family*/
  rc = genl_register_ops(&doc_exmpl_gnl_family, &doc_exmpl_gnl_ops_echo);
  if (rc != 0){
                printk("register ops: %i\n",rc);
                genl_unregister_family(&doc_exmpl_gnl_family);
    goto failure;
        }

  return 0;
  
  failure:
        printk("an error occured while inserting the generic netlink example module\n");
  return -1;
  
  
}

static void __exit gnKernel_exit(void)
{
        int ret;
        printk("EXIT GENERIC NETLINK EXEMPLE MODULE\n");
        /*unregister the functions*/
  ret = genl_unregister_ops(&doc_exmpl_gnl_family, &doc_exmpl_gnl_ops_echo);
  if(ret != 0){
                printk("unregister ops: %i\n",ret);
                return;
        }
        /*unregister the family*/
  ret = genl_unregister_family(&doc_exmpl_gnl_family);
  if(ret !=0){
                printk("unregister family %i\n",ret);
        }
}


module_init(gnKernel_init);
module_exit(gnKernel_exit);
MODULE_LICENSE("GPL");