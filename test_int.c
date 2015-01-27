#include <stdio.h>

int main(){
	int a = 4;
	int * p = &a;
	int i;
	for( i = 0 ; i < sizeof(int) ; i++) printf("%c" , p[i]);
	
	printf("\nsize of int is %d \n" , sizeof(int));
return 0;
}
