#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc,char *argv[]){
	FILE *file = fopen(argv[1],"r+");
	FILE *tfile = fopen("yerim","w+");
	int offset = atoi(argv[2]);
	char buffer[100];

	if(argc != 4){
		printf("Usage %s <filename>\n",argv[0]);
		exit(1);
	}
	
	if(file == NULL){
		fprintf(stderr, "File open is failed");
		exit(1);
	}
	
	if(tfile == NULL){
		fprintf(stderr,"File open is failed");
		exit(1);
	}

	fseek(file,offset,SEEK_SET);
	fputs(argv[3],tfile);
	
	while(fgets(buffer,sizeof(buffer),file)!= NULL)
		fputs(buffer,tfile);
	fseek(file,offset,SEEK_SET);
	fseek(tfile,0,SEEK_SET);

	while(fgets(buffer,sizeof(buffer),tfile)!=NULL)
		fputs(buffer,file);

	fclose(file);
	fclose(tfile);
	remove("yerim");
}
	
