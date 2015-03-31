//#################################################################################################
// This utility will be called from the kernel module interceptor.c.
// It has to read the data written in the character device and then parse it for tokens which
// can then be inserted into the DB.
// The schema for the DB is:
// FILE		TIMESTAMP		SID		ACTION
//
// Sample record written in the character device is:
// /home/nitin/myfile.txt|||<timestamp>|||1712380443|||READ
//  
//#################################################################################################
#include <iostream>
#include <string>
#include <fstream>
#define RETURN_DEVICE_NAME "/dev/my_char_dev_return"
#define CHAR_DEVICE_NAME "/dev/my_char_dev"
using namespace std;


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

int main(int argc, char const *argv[])
{
	ifstream char_device(CHAR_DEVICE_NAME);
	string str;
	getline(char_device , str);
	struct token_class tokens;
	int nTokens = 0;
	split(str.c_str() , "$" , &tokens , &nTokens);
	






	/* code */
	return 0;
}
