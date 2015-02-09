//************************************************************
// This program will read a file containing inode information.
// It will initialize a data structure in memory based on that
//************************************************************
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cstdio>
#include <cstring>
#include <time.h>
#include <unistd.h>
#define INPUT_SID_FILE "/home/nitin/thesis/modules/sample_sid_input.txt"
#define RETURN_DEVICE_NAME "/dev/my_char_dev_return"
#define CHAR_DEVICE_NAME "/dev/my_char_dev"

using namespace std;

typedef struct hs{
	unsigned long inode_number;
	const char ** values;
	//struct hs * next;
	
}s_hash;

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
void write_line_to_device(s_hash * input , int size){
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
	int  bytes_written = sprintf(temp_buff , "%d%lu" , size , input->inode_number);

	for(int i = 0 ; i < size ; i++){
		temp += bytes_written;
		bytes_written = sprintf(temp , "%s%c" , input->values[i] , (char)1);
	}
	temp[bytes_written] = '\0'

	fprintf( fp , "%s", temp_buff);
	fclose(fp);
}
void write_signal_for_kernel_to_read(){
	FILE * fp = fopen(RETURN_DEVICE_NAME , "w");
	fprintf(fp, "%s", "done" );

	fclose(fp);
}
void check_if_kernel_has_read(){
	FILE * fp = fopen(RETURN_DEVICE_NAME , "r");
	char temp[9];
	while(strncmp(temp , "read" , 4) == 0){
//usleep takes an argument which is microseconds so I multiplied 10 milliseconds by 1000	
	usleep(10 * 1000); 
	fscanf(fp , "%s", temp);
	}
	fclose(fp);
}
//void tokenize(const string& input , const)
int main(int argc, char const *argv[])
{
	ifstream infile(INPUT_SID_FILE);
	string line;
	string delims(1,'$');
	vector<string> tokens;
	while(getline(infile , line)){
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
			check_if_kernel_has_read();			

			tokens.clear();
			free(temp);
	}
	infile.close();
		
	return 0;
} 