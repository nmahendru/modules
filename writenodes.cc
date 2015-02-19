/***************************************************************************************************
Author :Nitin Mahendru
This program is basically used to write down all the inode information to memory when the system is
shut down. It will basically utilize a character device to get data from the kernel memory and keep 
on reading a line at a time from it.
***************************************************************************************************/

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

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
@split
This function is used to split strings to tokens 
@str = string containing the string to be split
@delimiters - this string contains the delimiters
@tokens - this store the resulting tokens
***************************************************************************************************/

void split(const string& str, const string& delimiters , vector<string>& tokens)
{
    // Skip delimiters at beginning.
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (string::npos != pos || string::npos != lastPos)
    {
        // Found a token, add it to the vector
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}

/***************************************************************************************************
Function to check if the kernel has written the next line
****************************************************************************************************/

int check_if_next_line_written(fstream& obj){
	char buf[8] = {0};
	obj.getline(buf , 8);	
	while(strcmp(buf,"write") != 0)
	{
/*
The message done from the kernel would mean that it's done writing all the sid information so now
writenodes needs to stop as there is no more data to read or write.
*/		
		if(strcmp(buf, "done") == 0){
			return -1;
		}
		obj.getline(buf , 8);
	}

}

/**************************************************************************************************
@inform_kernel_line_read
This function will write done to the my_char_dev_return so that the kernel can be informed that 
the line has been written.
**************************************************************************************************/

void inform_kernel_line_read(fstream& char_dev_return){
	string temp = "written";
	char_dev_return << temp;
}

/***************************************************************************************************
Function to read one line from the character device and write back to the text file for permanent 
storage so that you can retrive it back on system startup
***************************************************************************************************/

void read_line_write_to_file(ifstream& char_file , ofstream& out_file){
	string line;
	getline(char_file , line);
	out_file << line << "\n";
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
		int r = check_if_next_line_written(char_device_return);
//This check is put to stop reading when we have read all the lines and now have to stop;
		if(r == -1) break;
		read_line_write_to_file(char_device, inode_file);

		inform_kernel_line_read(char_device_return);

	}




	/* code */
	return 0;
}