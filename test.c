#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(){
	char temp[20] ={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1} ;
	char * s1 = "done";
	char * s2 = "don";
	//int bytes_written = sprintf(temp , "%s%c%s" , s1 , '\0' , s2);
	int bytes_written = sprintf(temp , "%s%c%s" , s1  , (char)1 , s2 );
	temp[bytes_written] = (char)0;
	int strl = strlen(temp);
	//temp[14] = (char)1;
	char read_string[20];
	sscanf(temp, "%s" , read_string);
	printf("string written is =%s   string length =%d bytes written = %d  string read is =%s\n" , temp , strl , bytes_written , read_string);
return 0;
}
