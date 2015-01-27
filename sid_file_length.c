#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

#define FILE_NAME "/home/nitin/thesis/modules/sample_sid_input.txt"
#define RETURN_DEVICE "/dev/my_char_dev_return"



int main(){
	int number_of_lines = 0;
	FILE * fp  = fopen( FILE_NAME , "r" );
	char buf[1024];
	if(fp > 0){
	while(fgets(buf , 100 , fp)) number_of_lines++;
		FILE * f_return = fopen(RETURN_DEVICE , "w");
		fprintf(f_return , "%d", number_of_lines);
		fclose(f_return);	
	}
	printf("%d\n" , number_of_lines);
	fclose(fp);
    	
	return 0;
}
