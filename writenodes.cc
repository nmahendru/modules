/***************************************************************************************************
Author :Nitin Mahendru
This program is basically used to write down all the inode information to memory when the system is
shut down. It will basically utilize a character device to get data from the kernel memory and keep 
on reading a line at a time from it.
***************************************************************************************************/

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>

/***************************************************************************************************
Defines !
***************************************************************************************************/
#define OUTPUT_SID_FILE "/home/nitin/thesis/modules/sample_sid_output.txt"
#define RETURN_DEVICE_NAME "/dev/my_char_dev_return"
#define CHAR_DEVICE_NAME "/dev/my_char_dev"
using namespace std;

/***************************************************************************************************
inode structure defintion - might not be used
***************************************************************************************************/
typedef struct hs{
	unsigned long inode_number;
	const char ** values;
	//struct hs * next;
	
}s_hash;

/***************************************************************************************************
replace_1_with_0
This function will basically replace (char)1 with (char0) so that we can read the seperated
fragments as strings
***************************************************************************************************/
void replace_1_with_0(char * buff){
  int strl = strlen(buff);
  int i ;
  for(i = 0 ; i < strl ; i++){
    if(buff[i] == (char)1) buff[i] = '\0';
  }
}

/***************************************************************************************************
Function to check if the kernel has written the next line
****************************************************************************************************/

void check_if_next_line_written(fstream& obj){
	char buf[8] = {0};
	obj.getline(buf , 8);	
	while(strcmp(buf,"write") != 0)
	{
		obj.getline(buf , 8);
	}

}

/***************************************************************************************************
Function to read one line from the character device and write back to the text file for permanent 
storage so that you can retrive it back on system startup
***************************************************************************************************/

void read_line_write_to_file(ifstream& char_file , ofstream& out_file){
	char buf[1023] = {0};
	int strl = strlen(buf);
	replace_1_with_0(buf);
	char_file.getline(buf);
	unsigned long inode_number;
	char s_buf[128] = {0};
	char_file >> inode_number;
	

}




/***************************************************************************************************
main
***************************************************************************************************/

int main(int argc, char const *argv[])
{
	ifstream char_device(CHAR_DEVICE_NAME);
	fstream char_device_return(RETURN_DEVICE_NAME , fstream::in | fstream::out);
	ofstream inode_file(OUTPUT_SID_FILE);

	while(true){
		check_if_next_line_written(char_device_return);

	}


	/* code */
	return 0;
}