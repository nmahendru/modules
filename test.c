#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(){
	char ** temp  = malloc(sizeof(char *) * 2);
	char * t = malloc(20);
	strcpy(t, "hello");
	temp[0] = t;
	temp[1] = t;
	printf("%s\n" , *temp);
return 0;
}
