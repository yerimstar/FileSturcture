#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]){
	FILE *file = fopen(argv[1],"r+");
	FILE *tfile = fopen("yerim","w+");

	char arr[100];
	char *buffer;
	int size;
	int count;
	int offset = atoi(argv[2]);
	int dbyte = atoi(argv[3]);

	if(argc != 4){
		printf("Usage %s <filename> \n",argv[0]);
		exit(1);
	}

	if(file == NULL){
		fprintf(stderr,"File open is failed");
		exit(1);
	}

	if(tfile == NULL){
		fprintf(stderr,"File open is failed");
		exit(1);
	}
	fseek(file,0,SEEK_END);
	size =ftell(file);
	buffer = malloc(size+2);
	memset(buffer,0,size+2);

	
	if((size-offset) < dbyte){
		count = size-offset;
		fseek(file,offset,SEEK_SET);
		ftruncate(fileno(file),offset);
			
	}	
	else{
		fseek(file,offset+dbyte,SEEK_SET);
		while(fgets(buffer,size-offset-dbyte,file)!=0)
		fputs(buffer,tfile);
	
		fseek(file,offset,SEEK_SET);
		fseek(tfile,0,SEEK_SET);
	
		while(fgets(buffer,size-offset-dbyte,tfile)!=0)
			fputs(buffer,file);	
		ftruncate(fileno(file),size-dbyte);
	}

	fclose(file);
	fclose(tfile);
	remove("yerim");
}

