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
#define DEVICE_SIZE 1024
/**************************************************************************************************
Global declarations for this module.
**************************************************************************************************/
static struct sid_obj **  sid_hash_k = NULL;
static int is_all_hash_written = 0;
static int hash_write_index = 0;
static int hash_list_write_index = 0;


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

EXPORT_SYMBOL(sid_hash_k);


static void replace_1_with_0(char * buff){
  int strl = strlen(buff);
  int i ;
  for(i = 0 ; i < strl ; i++){
    if(buff[i] == (char)1) buff[i] = '\0';
  }
}
/**************************************************************************************************
add_node
This function adds a node to the has data structure and then returns without any value.

**************************************************************************************************/
void add_node_k(struct sid_obj * ptr){
  int hash_value = ptr->inode_number % HASH_SIZE ;
  if(!sid_hash_k[hash_value]){
    sid_hash_k[hash_value] = ptr;
  }else{
    struct sid_obj * entry_ptr = sid_hash_k[hash_value];
	if(entry_ptr->inode_number == ptr->inode_number){
		printk("Duplicate insertion attempted for inode %lu   ignoring and returning\n" ,ptr->inode_number );
		return;
	     }
    while(entry_ptr->next) {
	     if(entry_ptr->inode_number == ptr->inode_number){
		printk("Duplicate insertion attempted for inode %lu   ignoring and returning\n" ,ptr->inode_number );
		return;
	     }
		entry_ptr = entry_ptr->next;
    	
	}
	entry_ptr->next = ptr;
  }

}

struct sid_obj * get_node_k(unsigned long inode_number){
  int hash_value = inode_number % HASH_SIZE;
  if(!sid_hash_k[hash_value]) return NULL;
  else{
    struct sid_obj * entry_ptr = sid_hash_k[hash_value];
    while(entry_ptr->next != NULL && entry_ptr->inode_number != inode_number) //using the shortcircuit property of the && operator
      entry_ptr = entry_ptr->next;
    if(entry_ptr->inode_number == inode_number) return entry_ptr;
    else return NULL;
  }
}

EXPORT_SYMBOL(get_node_k);

/***************************************************************************************************
read_one_line
This function will read one line from the char device, create a node and then add it to the kernel 
hash so that now it's ready for access when checked by hooked system calls
***************************************************************************************************/
static int read_one_line(char * buff){
  //int strl;
  int i;
  //unsigned long inode_number;
  //int r_value;
  //int num_sids;
  struct sid_obj * obj_ptr;
  char value_buff[50];
  //strl = strlen(buff);
  printk("thesis:read one line called \n");
  replace_1_with_0(buff);
  obj_ptr = (struct sid_obj*)vmalloc(sizeof(struct sid_obj));
  
  printk("thesis: memory allocated for hash structure\n");
  // read the inode number from the buffer
  sscanf(buff , "%s" , value_buff);
  buff += strlen(value_buff) + 1;
  sscanf(value_buff , "%d" , &obj_ptr->n_sids);
  sscanf(buff , "%s" , value_buff);
  buff += strlen(value_buff) + 1;
  sscanf(value_buff , "%lu" , &obj_ptr->inode_number);
  obj_ptr->sids = (char **) vmalloc(sizeof(char *) * obj_ptr->n_sids);
  obj_ptr->next = NULL;	
  printk("value read for sids and inode\n");
  i = 0;
    while(i < obj_ptr->n_sids){
    char * sid_value;
    sscanf(buff , "%s" , value_buff);
    buff += strlen(value_buff) + 1;
    sid_value = vmalloc(strlen(value_buff) + 1);
    strcpy(sid_value , value_buff);
    obj_ptr->sids[i++] = sid_value;
    if(i >= 6) break;
  }
  printk("thesis: one line read  inode = %lu  number of sids = %d   first sid = %s\n" , obj_ptr->inode_number , obj_ptr->n_sids , obj_ptr->sids[0]);
// Add the read sid node to the hash in the kernel  
add_node(obj_ptr);
printk("thesis: add node call finished\n");

  return 0;
}
static char * prepare_one_line(struct sid_obj * ptr){
  char * buf , * ret ;
  int r , n; 
  buf = (char *) vmalloc(DEVICE_SIZE);
  ret = buf;
  
  r = sprintf(buf , "%lu" , ptr->inode_number);
  buf += r;
  *buf++ = '$';
  n = ptr->n_sids;
  while(n > 0){
    r = sprintf(buf , "%s" , ptr->sids[--n]);
    buf += r;
    *buf++ = '$';
  }
   --buf;	
  *buf++ = '\n';
  *buf = '\0';
  return ret;

}

static void write_one_line_back_to_user(void){
  if(hash_write_index >= HASH_SIZE ){ 
      is_all_hash_written = 1;
      return;
    }else if(hash_list_write_index == 0){
    struct sid_obj * ptr;
    int n = 0;
    char * temp;
    ptr = sid_hash_k[hash_write_index];
    while(n++ < hash_list_write_index && ptr->next) ptr = ptr->next;


    temp = prepare_one_line(ptr);
    my_char_dev_write_k(temp, strlen(temp));
    vfree(temp);
    
    if(ptr->next) hash_list_write_index++; 
    else{
     hash_list_write_index = 0;
     
     while(++hash_write_index < HASH_SIZE){
      ptr = sid_hash_k[hash_write_index];
      if(ptr) break;
     }
     
    }

   }// hash no yet written fully else. 
}

 


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
  const char reply_message_init_hash[] = "kernel:read one line";
  const char reply_message_start_writing[] = "kernel:ok. Wrote first line";
  const char reply_message_wrote_line[] = "kernel:written";
  const char reply_message_data_finished[] = "kernel:all data finished";
  

  const char user_message_start_writing[] = "user:start writing back";
  const char user_message_request_more_writes[] = "user:write another line please";
  const char user_message_read_more_lines[] = "user:written one line";

  printk("doc example function called\n");
  
        if (info == NULL)
                goto out;
  
        /*for each attribute there is an index in info->attrs which points to a nlattr structure
         *in this structure the data is given
         */
        na = info->attrs[DOC_EXMPL_A_MSG];
        if (na) {
      mydata = (char *)nla_data(na);
    if (mydata == NULL){
      printk("error while receiving data\n");
      goto out;
    }
    else
      printk("received: %s\n", mydata);
//#################################################################################################
//Need to check if the call from userspace is to initialize the hash or write back the hash to user space  
    if(strcmp(mydata , user_message_start_writing) == 0){
	printk("thesis: case 1 about to write the first line\n");
        struct sid_obj * ptr = sid_hash_k[hash_write_index];
        while(!ptr && hash_write_index < HASH_SIZE) {
          ptr = sid_hash_k[hash_write_index++];
        }
	hash_write_index--;
        if(hash_write_index >= HASH_SIZE) 
          is_all_hash_written = 1;
        else 
          write_one_line_back_to_user();

    }//if for start writing back ends
    else if(strcmp(mydata , user_message_request_more_writes) == 0){
	printk("thesis: writing back subsequent lines\n");
        write_one_line_back_to_user();

    }//if for writing more ends
    else if(strcmp(mydata , user_message_read_more_lines) == 0){
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

    }//if for writing more ends
    else{
//###### BLANk CASE#################################################################################

    }

    }//if na ends
    else{
      printk("no info->attrs %i exiting\n", DOC_EXMPL_A_MSG);
        goto out;
      }




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


//#################################################################################################
//Decide what message to send back so that user program takes an action accordingly  
  if(is_all_hash_written){
      rc = nla_put_string(skb, DOC_EXMPL_A_MSG, reply_message_data_finished);
      if (rc != 0)
        goto out;
  }
  else if(strcmp(mydata , user_message_start_writing) == 0){
      rc = nla_put_string(skb, DOC_EXMPL_A_MSG, reply_message_start_writing);
      if (rc != 0)
        goto out;

  }//if for start writing back ends
  else if(strcmp(mydata , user_message_request_more_writes) == 0){
      rc = nla_put_string(skb, DOC_EXMPL_A_MSG, reply_message_wrote_line);
      if (rc != 0)
        goto out;

      }//if for writing more ends
  else if(strcmp(mydata , user_message_read_more_lines) == 0){
    rc = nla_put_string(skb, DOC_EXMPL_A_MSG, reply_message_init_hash);
    if (rc != 0)
      goto out;

      }//if for writing more ends
  else{
  //###### BLANk CASE#################################################################################

      }








  
  
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
  int rc , i;
        printk("INIT GENERIC NETLINK EXEMPLE MODULE\n");
        
  sid_hash_k = (struct sid_obj **) vmalloc(sizeof(struct sid_obj *) * HASH_SIZE);
  for( i = 0 ; i < HASH_SIZE ; i++) sid_hash_k[i] = NULL;
  printk("memory allocated for hash in kernel\n");
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
        vfree(sid_hash_k);
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
