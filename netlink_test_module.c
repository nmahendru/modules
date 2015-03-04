#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <net/sock.h>
#include <net/netlink.h>
#include <net/genetlink.h>
#define NETLINK_NITRO 17

MODULE_LICENSE("GPL");
static u32 pid;
struct genl_info *info;

///////////////////////////////////////////////////////////

static int netlink_test(){
/* attributes */
 enum {
       DOC_EXMPL_A_UNSPEC,
       DOC_EXMPL_A_MSG,
       __DOC_EXMPL_A_MAX,
 };
 #define DOC_EXMPL_A_MAX (__DOC_EXMPL_A_MAX - 1)
 /* attribute policy */
 static struct nla_policy doc_exmpl_genl_policy[DOC_EXMPL_A_MAX + 1] = {
       [DOC_EXMPL_A_MSG] = { .type = NLA_NUL_STRING },
 };
 /* family definition */
 static struct genl_family doc_exmpl_gnl_family = {
       .id = GENL_ID_GENERATE,
       .hdrsize = 0,
       .name = "DOC_EXMPL",
       .version = 1,
       .maxattr = DOC_EXMPL_A_MAX,
 };
/* handler */
 //int doc_exmpl_echo(struct sk_buff *skb, struct genl_info *info) 
 int doc_exmpl_echo(struct sk_buff *skb, struct genl_info *info) 
 {		
 		if(skb == NULL){printk(KERN_INFO "thesis:The message received has null buffer value \n"); return -1;}
       printk(KERN_INFO "thesis: message recieved. Value is ===%s\n" ,skb->data);
       pid = info->snd_pid;
       return 0;
 }
 /* commands */
 enum {
       DOC_EXMPL_C_UNSPEC,
       DOC_EXMPL_C_ECHO,
       __DOC_EXMPL_C_MAX,
 };
 #define DOC_EXMPL_C_MAX (__DOC_EXMPL_C_MAX - 1)
 /* operation definition */
 static struct genl_ops doc_exmpl_gnl_ops_echo = {
       .cmd = DOC_EXMPL_C_ECHO,
       .flags = 0,
       .policy = doc_exmpl_genl_policy,
       .doit = NULL,
       .dumpit = NULL,
 };

doc_exmpl_gnl_ops_echo.doit = doc_exmpl_echo;

int rc;
 rc = genl_register_family(&doc_exmpl_gnl_family);
 if (rc != 0)
 	return -1;

 rc = genl_register_ops(&doc_exmpl_gnl_family, &doc_exmpl_gnl_ops_echo);
 if (rc != 0) return -1;

 struct sk_buff *skb;
 skb = genlmsg_new(NLMSG_GOODSIZE, GFP_KERNEL);
 if (skb == NULL)
     return -1;

 
 void *msg_head;
 /* create the message headers */
 msg_head = genlmsg_put(skb, pid, /*sequence number*/0, &doc_exmpl_gnl_family, /**flags**/0 , DOC_EXMPL_C_ECHO);
 if (msg_head == NULL) {
     rc = -ENOMEM;
     return -1;
 }
 /* add a DOC_EXMPL_A_MSG attribute */
 rc = nla_put_string(skb, DOC_EXMPL_A_MSG, "Generic Netlink Rocks");
 if (rc != 0)
     return -1;
 /* finalize the message */
 genlmsg_end(skb, msg_head);

struct genl_info *  temp_info = genl_info_net(info);
rc = genlmsg_unicast(temp_info ,skb, pid);
 if (rc != 0)
     return -1;

}
///////////////////////////////////////////////////////////////////



static int __init my_module_init(void)
{
printk(KERN_INFO "Initializing Netlink Socket");
netlink_test();
return 0;
}

static void __exit my_module_exit(void)
{
printk(KERN_INFO "Goodbye");

}

module_init(my_module_init);
module_exit(my_module_exit);