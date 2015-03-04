//************************************************************
// This program will read a file containing inode information.
// It will initialize a data structure in memory based on that
//************************************************************
//#include <iostream>
//#include <string>
//#include <fstream>
//#include <vector>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include "netlink_test_user.c"
#define INPUT_SID_FILE "/home/nitin/thesis/modules/sample_sid_input.txt"
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


struct token_class{
	char ** tokens;
};

void split(char * line, char * delims , struct token_class * t , int * nTokens)
{

	t->tokens = (char **)malloc(sizeof(char*) * 10);
	char * temp = strtok(line , delims);
	

	while(temp != NULL){
		printf("token extracted = %s\n" , temp);
		t->tokens[(*nTokens)++] = temp;
		temp  = strtok(NULL , delims);
	
	}
	//#####################################
	//below two lines are to remove the newline character in the last token
	//#####################################
	int len = strlen(t->tokens[*nTokens -1]);
	t->tokens[*nTokens -1][len - 1] = '\0';

	printf("Number of tokens read = %d\n" , *nTokens);
}
void write_line_to_device(struct token_class* input , int size){
	/* C++ version does not work for c strings
	ofstream outfile(RETURN_DEVICE_NAME);
	outfile << input->inode_number;
	for(int i = 0 ; i < size ; i++){
		outfile << strlen(input->values[i] << input->values[i];
	}
	outfile.close();
	*/
	FILE * fp = fopen(CHAR_DEVICE_NAME , "w");
	char temp_buff[1023];
	char * temp = temp_buff;
	int  bytes_written = sprintf(temp_buff , "%d" , size);

	for(int i = 0 ; i <= size ; i++){
		temp += bytes_written + 1;
		bytes_written = sprintf(temp , "%s" , input->tokens[i] );
	}
	temp += bytes_written + 1;
	replace_0_with_1(temp_buff, temp - temp_buff);
	temp[0] = '\0';
	fprintf( fp , "%s", temp_buff);
	printf("user program wrote = %s\n" , temp_buff);
	fclose(fp);
}


//void tokenize(const string& input , const)
int main(int argc, char const *argv[])
{

FILE * infile = fopen(INPUT_SID_FILE , "r");
	char line[1024];
	char delims[2] = {'$' , '\0'};
	struct token_class t;
	char ** tokens;
	s_hash * temp;
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

while(1) {//The loop to read the file begins  
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
/*All the processing code to read one line and then wait for kernel to respond goes here */     

            if(!fgets(line ,1023 , infile)) break; // break out of the loop is lines to read finish
            int nTokens = 0;
			split(line , delims , &t , &nTokens);
			
			
			
//write one line to the character device			
			write_line_to_device(&t, nTokens - 1);
//write a signal value to the character device to allow kernel to read
			//write_signal_for_kernel_to_read();
//check if kernel has read the signal
			
			//check_if_kernel_has_read();			

			//tokens.clear();
			//free(temp);







/*code ends. Now inform the kernel that data written and ask it to respond back*/






        char message[] = "user:written one line"; //message
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
        printf("kernel says: %s\n",reply);
        
			free(temp);

        //close(nl_sd);

	}//while for reading file ends        


close(nl_sd);
//////////////////////////////////////////////////////////////////////////////////////////////////////////      
	//ifstream infile(INPUT_SID_FILE);
	//string line;
	//string delims(1,'$');
	//vector<string> tokens;
	//while(getline(infile , line)){
	/*getline(infile , line);
			split(line , delims , tokens);

			s_hash * temp = new s_hash;
		
			temp->inode_number = stoi(tokens[0] , nullptr , 10);
		
			int numSids = tokens.size() - 1;
			temp->values = new const char * [numSids];
			for(int i = 0 ; i < numSids ; i++){
				temp->values[i] = tokens.at(i + 1).c_str();
			}
//write one line to the character device			
			write_line_to_device(temp, numSids);
//write a signal value to the character device to allow kernel to read
			write_signal_for_kernel_to_read();
//check if kernel has read the signal
			usleep(35 * 1000);
			check_if_kernel_has_read();			

			tokens.clear();
			free(temp);
	}*/
	fclose(infile);
		
	return 0;
} 