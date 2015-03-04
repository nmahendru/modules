#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static void replace_1_with_0(char * buff){
  int strl = strlen(buff);
  int i ;
  for(i = 0 ; i < strl ; i++){
    if(buff[i] == (char)1) buff[i] = '\0';
  }
}
int main(){
	int a = 2;
	unsigned long b = 23232235235l;
	unsigned long c , f;

	int d = 3;
	int e , r;
	char * buff = (char * )malloc(100);
	r = sprintf(buff , "%lu" , b);
	printf("%s\n" , buff);
	sscanf(buff , "%lu" , &c);
	printf("%lu\n" , c);
	buff += r + 1;

    r = sprintf(buff , "%d" , d);
	printf("%s\n" , buff);
	sscanf(buff , "%d" , &e);
	printf("%d\n" , e);
    buff += r;

	sprintf(buff , "%lu" , b);
	printf("%s\n" , buff);
	sscanf(buff , "%lu" , &f);
	printf("%lu\n" , f);

/*
	char * s = "test";
	char * buff = (char * )malloc(100);
	char * temp = buff;
	int r;
	r = sprintf(temp , "%d" , a );
	temp += r + 1;
	
	printf("bytes written = %d\n" , r);
	
	r = sprintf(temp , "%lu" , b);
	
	printf("bytes written = %d\n and long written = %s" , r , temp);
	temp += r + 1;
	r = sprintf(temp , "%s" , s );
	temp += r + 1;
	printf("bytes written = %d\n" , r);
	int slen = temp - buff;
	slen++;
	replace_0_with_1(buff);
	buff[slen] = '\0';
	printf("%s\n" , buff);

	char * c = (char*)malloc(5) ;
	char * d = (char *) malloc(20);
	r = sscanf(buff, "%s%s" , c , d);
	int num;
	unsigned long inode_number;
	sscanf(c , "%d" , &num);
	sscanf(d , "%lu" , &inode_number);
	
	//r = sscanf(buff, "%d" , &c);
	//buff += r; */
	//printf("sid = %d  inode number = %lu  and string = %s\n"  ,num , inode_number , buff);
return 0;

}
