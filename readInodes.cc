//************************************************************
// This program will read a file containing inode information.
// It will initialize a data structure in memory based on that
//************************************************************
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#define input_sid_file sample_sid_input.txt
using namespace std;
typedef struct hs{
	unsigned int key;
	char * values[];
	struct hs * next;
	
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

//void tokenize(const string& input , const)
int main(int argc, char const *argv[])
{
	ifstream infile("input_sid_file");
	string line;
	string delims(1,'$');
	vector<string> tokens;
	while(getline(infile , line)){
			split(line , delims , tokens);

			s_hash * temp = new s_hash;
		
			temp->key = stoi(tokens[0] , nullptr , 10);
		
			int numSids = delims.size() - 1;
			temp->values = new char[numSids];
			for(int i = 0 ; i < numSids ; i++){
				temp->values[i] = tokens.at(i + 1).c_str();
			}

			temp->key next = NULL;
		}
		
	return 0;
} 