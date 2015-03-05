/***************************************************************************************************
Author :Nitin Mahendru
This program is basically used to write down all the inode information to memory when the system is
shut down. It will basically utilize a character device to get data from the kernel memory and keep 
on reading a line at a time from it.
***************************************************************************************************/
#include <cstdio>
#include <cstring>
#include <ctime>
#include <cstdlib>
#include <sys/socket.h>
#include <linux/netlink.h>
#include "netlink_test_user.c"
#include <iostream>
#include <string>
#include <fstream>

using namespace std;


#define OUTPUT_SID_FILE "/home/nitin/thesis/modules/sample_sid_output.txt"
#define RETURN_DEVICE_NAME "/dev/my_char_dev_return"
#define CHAR_DEVICE_NAME "/dev/my_char_dev"



//#################################################################################################
void replace_0_with_1(char * buf , int len){
	for(int i = 0 ; i < len ; i++){
		if(buf[i] == '\0') buf[i] = (char)1 ;
	}	
}
//#################################################################################################

/**************************************************************************************************
Netlink API code. This will be used to communicate with the kernle after this application is 
called from the kernel.
**************************************************************************************************/
#define MAX_PAYLOAD 1024  /* maximum payload size*/
struct sockaddr_nl src_addr, dest_addr;
struct nlmsghdr *nlh = NULL;
struct iovec iov;
int sock_fd;





/**************************************************************************************************
Netlink code ends here.
**************************************************************************************************/
typedef struct hs{
	unsigned long inode_number;
	char ** values;
	//struct hs * next;
	
}s_hash;






int main(int argc, char const *argv[])
{

	ofstream outfile(OUTPUT_SID_FILE , ofstream::out);
	string line;
	
//////////////////////////////////////////////////////////////////////////////////////////////////
//CODE FOR SOCKET SETUP	
	nl_sd = create_nl_socket(NETLINK_GENERIC,0);
        if(nl_sd < 0){
                printf("create failure\n");
                return 0;
        }
        int id = get_family_id(nl_sd);
	struct {
                struct nlmsghdr n;
                struct genlmsghdr g;
                char buf[256];
        } ans;

        struct {
                struct nlmsghdr n;
                struct genlmsghdr g;
                char buf[256];
        } req;
        struct nlattr *na;
//########################################################################################################
while(1){//Request kernel to start writing the file
        /* Send command needed */
        req.n.nlmsg_len = NLMSG_LENGTH(GENL_HDRLEN);
        req.n.nlmsg_type = id;
        req.n.nlmsg_flags = NLM_F_REQUEST;
        req.n.nlmsg_seq = 60;
        req.n.nlmsg_pid = getpid();
        req.g.cmd = 1;//DOC_EXMPL_C_ECHO;
        
        /*compose message*/
        na = (struct nlattr *) GENLMSG_DATA(&req);
        na->nla_type = 1; //DOC_EXMPL_A_MSG

        char message[] = "user:start writing back"; //message
        int mlength = strlen(message) + 2;
        na->nla_len = mlength+NLA_HDRLEN; //message length
        memcpy(NLA_DATA(na), message, mlength);
        req.n.nlmsg_len += NLMSG_ALIGN(na->nla_len);

        /*send message*/
	struct sockaddr_nl nladdr;
        int r;
        
        memset(&nladdr, 0, sizeof(nladdr));
        nladdr.nl_family = AF_NETLINK;
    
	r = sendto(nl_sd, (char *)&req, req.n.nlmsg_len, 0,  
			  (struct sockaddr *) &nladdr, sizeof(nladdr));
/* message to kernel a sent now now wait for 35 msec*/
		usleep(35 * 1000);




	
		int rep_len = recv(nl_sd, &ans, sizeof(ans), 0);
        /* Validate response message */
  	if (ans.n.nlmsg_type == NLMSG_ERROR) { /* error */
                printf("error received NACK - leaving \n");
               	break;
        }
        /*while(rep_len <= 0) {
              usleep(10*1000);
              rep_len = recv(nl_sd, &ans, sizeof(ans), 0);
        }*/
  	if (!NLMSG_OK((&ans.n), rep_len)) {
       printf("invalid reply message received via Netlink\n");
		break;
	}

        rep_len = GENLMSG_PAYLOAD(&ans.n);
        /*parse reply message*/
        na = (struct nlattr *) GENLMSG_DATA(&ans);
        char * reply = (char *)NLA_DATA(na);
        if(strcmp(reply , "kernel:ok. Wrote first line") == 0)
        	break;
}//while for requesting kernel to start ends
//#################################################################################################

//########################################################################################################
while(1){// Loop to write down the line one by one

//##############writing line to file###############################################################		
		ifstream infile(CHAR_DEVICE_NAME);
		getline(infile , line);
		outfile << line << endl;



        req.n.nlmsg_len = NLMSG_LENGTH(GENL_HDRLEN);
        req.n.nlmsg_type = id;
        req.n.nlmsg_flags = NLM_F_REQUEST;
        req.n.nlmsg_seq = 60;
        req.n.nlmsg_pid = getpid();
        req.g.cmd = 1;//DOC_EXMPL_C_ECHO;
        
        /*compose message*/
        na = (struct nlattr *) GENLMSG_DATA(&req);
        na->nla_type = 1; //DOC_EXMPL_A_MSG

        char message[] = "user:write another line please"; //message
        int mlength = strlen(message) + 2;
        na->nla_len = mlength+NLA_HDRLEN; //message length
        memcpy(NLA_DATA(na), message, mlength);
        req.n.nlmsg_len += NLMSG_ALIGN(na->nla_len);

        /*send message*/
	struct sockaddr_nl nladdr;
        int r;
        
        memset(&nladdr, 0, sizeof(nladdr));
        nladdr.nl_family = AF_NETLINK;
    
	r = sendto(nl_sd, (char *)&req, req.n.nlmsg_len, 0,  
			  (struct sockaddr *) &nladdr, sizeof(nladdr));
/* message to kernel a sent now now wait for 35 msec*/
		usleep(35 * 1000);




	
		int rep_len = recv(nl_sd, &ans, sizeof(ans), 0);
        /* Validate response message */
  	if (ans.n.nlmsg_type == NLMSG_ERROR) { /* error */
                printf("error received NACK - leaving \n");
               	break;
        }
        /*while(rep_len <= 0) {
              usleep(10*1000);
              rep_len = recv(nl_sd, &ans, sizeof(ans), 0);
        }*/
  	if (!NLMSG_OK((&ans.n), rep_len)) {
       printf("invalid reply message received via Netlink\n");
		break;
	}

        rep_len = GENLMSG_PAYLOAD(&ans.n);
        /*parse reply message*/
        na = (struct nlattr *) GENLMSG_DATA(&ans);
        char * reply = (char *)NLA_DATA(na);
        if(strcmp(reply , "kernel:written") == 0)
        	continue;
		if(strcmp(reply , "kernel:all data finished") == 0)
        	break;
        
        	
}//while for writing down the file line by line ends
//#################################################################################################



//##############close netlink socket#######################	
	close(nl_sd);

		
	return 0;
} 
