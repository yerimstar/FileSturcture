#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
	FILE *file;
	int size;
	char *buffer;
	
	int offset = atoi(argv[2]);
	int readbyte = atoi(argv[3]);
	
	if(argc != 4){
		printf("Usage %s <filename> \n",argv[0]);
		exit(1);
	}

	file = fopen(argv[1],"r");
	
	if(file == NULL){
		fprintf(stderr,"File open is failed");
		exit(1);
	}

	fseek(file,0,SEEK_END);
	size = ftell(file); // file 크기 구하기
	buffer = malloc(size+1);
	memset(buffer,0,size+1);// buffer를 동적할당 해준 후 0으로 초기화 


	fseek(file,offset,SEEK_SET);//offset값을 이용하여 파일 포인터의 위치 변경 
	if(readbyte < 0){
		fprintf(stderr, "readbyte error\n");
	}

	while(feof(file)==0){
		if((size-offset) < readbyte){
			fread(buffer,sizeof(size-offset),1,file);
			printf("%s",buffer);
			break;
		}
		else{
			fread(buffer,readbyte,1,file);
			printf("%s",buffer);
			break;
		}
	}
	fclose(file);
	return 0;
}
