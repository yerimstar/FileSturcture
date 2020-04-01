#include <stdio.h>
#include <stdlib.h>

int main(int argc,char *argv[]){
	FILE *file;
	int offset = atoi(argv[2]);
	char *data = argv[3];

	if(argc != 4){
		printf("Usage %s <filename>\n",argv[0]);
		exit(1);
	}

	file = fopen(argv[1],"r+");

	if(file == NULL){
		fprintf(stderr,"File open is failed");
		exit(1);
	}

	fseek(file,offset,SEEK_SET);
	fputs(data,file);
	
	fclose(file);
	exit(0);
}
