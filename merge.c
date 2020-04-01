#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
	FILE *fp1 = fopen(argv[1],"a+");
	FILE *fp2 = fopen(argv[2],"r+");
	
	char buffer[1024];
	buffer[1023] = 0;

	if(argc != 3){
		printf("Usage %s <filename>\n",argv[0]);
		exit(1);
	}
	
	if(fp1 == NULL){
		fprintf(stderr, "File open is failed");
		exit(1);
	}

	if(fp2 == NULL){
		fprintf(stderr, "File open is failed");
		exit(1);
	}


	while(fgets(buffer,1023,fp2) != NULL){
		fputs(buffer,fp1);
	}

	fclose(fp1);
	fclose(fp2);
}
