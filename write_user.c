//This program will actually write 500 bytes to a device and then return a success/failure value
//return value = number of bytes written +ve integer
//return value = -1 means that some error

#include <stdio.h>
#include <time.h>
#define LOG_FILE_LOC "/home/nitin/thesis/modules/read_write_log"
#define DEVICE_FILE_NAME "/dev/char_dev"
#define SI_FILE_NAME "/home/nitin/thesis/modules/si_file.txt"

int main(int argc , char ** argv){
	int bytesWritten = 0;

	//This log will be used to maintain a trace of activities 
	FILE * filep_log = fopen(LOG_FILE_LOC , "a");
	time_t t = time(NULL);
	char timeString[50];
	struct tm tm = *localtime(&t);

	sprintf(timeString , "now: %d-%d-%d %d:%d:%d\0", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	
	FILE * devicep = fopen(DEVICE_FILE_NAME , "w");
	if (devicep == NULL){
		fprintf(filep_log , timeString);
		fprintf(filep_log , " could not open device for writing. Exiting the program\n" );
		fclose(filep_log);
		return -1 ;
	}

	FILE * siFile = fopen(SI_FILE_NAME , "r");
	int bytesRead = 0;
	int temp = 0;
	while((temp = fread(timeString , 1, 50 , siFile)) > 0){
		fwrite(timeString , 1, temp , devicep);
		bytesRead +=  temp;
	}
	fclose(siFile);
	fclose(devicep);
	return bytesRead;





	//return bytesWritten;
}
