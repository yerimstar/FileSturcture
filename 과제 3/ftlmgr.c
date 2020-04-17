#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "flash.h"
// 필요한 경우 헤더파일을 추가한다

FILE *flashfp;	// fdevicedriver.c에서 사용

int dd_erase(int pbn);
int dd_write(int ppn, char *pagebuf);
int dd_read(int ppn, char *pagebuf);

//
// 이 함수는 FTL의 역할 중 일부분을 수행하는데 물리적인 저장장치 flash memory에 Flash device driver를 이용하여 데이터를
// 읽고 쓰거나 블록을 소거하는 일을 한다 (동영상 강의를 참조).
// flash memory에 데이터를 읽고 쓰거나 소거하기 위해서 fdevicedriver.c에서 제공하는 인터페이스를
// 호출하면 된다. 이때 해당되는 인터페이스를 호출할 때 연산의 단위를 정확히 사용해야 한다.
// 읽기와 쓰기는 페이지 단위이며 소거는 블록 단위이다.
// 
int main(int argc, char *argv[])
{	
	char sectorbuf[SECTOR_SIZE],sparebuf[SPARE_SIZE];
	char pagebuf[PAGE_SIZE];
	char *blockbuf;
	
	// flash memory 파일 생성: 위에서 선언한 flashfp를 사용하여 flash 파일을 생성한다. 그 이유는 fdevicedriver.c에서 
	//                 flashfp 파일포인터를 extern으로 선언하여 사용하기 때문이다.
	if(strcmp(argv[1],"c")==0){
		flashfp = fopen(argv[2],"w+");
		if(flashfp == NULL){
			fprintf(stderr,"file open is failed.");
			exit(1);
		}
		int blocksize = atoi(argv[3]);
		int size = PAGE_NUM*PAGE_SIZE;

		blockbuf = (char*)malloc(size);
		memset(blockbuf,(char)0xFF,size);
		for(int i = 0; i < blocksize; i++){
			fwrite(blockbuf,size,1,flashfp);
		}
	
		free(blockbuf);
		fclose(flashfp);
	}
	// 페이지 쓰기: pagebuf의 섹터와 스페어에 각각 입력된 데이터를 정확히 저장하고 난 후 해당 인터페이스를 호출한다
	else if(strcmp(argv[1],"w")==0){
		flashfp = fopen(argv[2],"r+");
		if(flashfp == NULL){
			fprintf(stderr,"file open is failed");
			exit(1);
		}//파일이 없으면 error출력
		int ppn = atoi(argv[3]);
		fseek(flashfp,0,SEEK_END);
		int filesize = ftell(flashfp);
		int pagenumber = filesize/PAGE_SIZE;//page의 개수 구하기 
		if(ppn >= pagenumber){
			fprintf(stderr,"lack of pages");
			exit(1);
		}//ppn값보다 페이지가 부족할 때 에러 출력
		memset(sectorbuf,(char)0xFF,SECTOR_SIZE);
		memset(sparebuf,(char)0xFF,SPARE_SIZE);
		memset(pagebuf,(char)0xFF,PAGE_SIZE);
		
		char str1[512];
		strcpy(str1,argv[4]);
		memcpy(sectorbuf,str1,strlen(str1));//sectorbuf값 입력받기
		memcpy(pagebuf,sectorbuf,SECTOR_SIZE);//pagebuf에 sectorbuf값 복사
		
		char str2[16];
		strcpy(str2,argv[5]);
		memcpy(sparebuf,str2,strlen(str2));//sparebuf값 입력받기 
		memcpy(pagebuf+SECTOR_SIZE,sparebuf,SPARE_SIZE);//pagebuf에 sparebuf값 복사 
		
		printf("%s",pagebuf);
		dd_write(ppn,pagebuf);//pagebuf값을 파일에 쓰기 
		fclose(flashfp);
	}
	// 페이지 읽기: pagebuf를 인자로 사용하여 해당 인터페이스를 호출하여 페이지를 읽어 온 후 여기서 섹터 데이터와
	//                  스페어 데이터를 분리해 낸다
	// memset(), memcpy() 등의 함수를 이용하면 편리하다. 물론, 다른 방법으로 해결해도 무방하다.
	else if(strcmp(argv[1],"r")==0){
		flashfp = fopen(argv[2],"r+");
		if(flashfp == NULL){
			fprintf(stderr,"file open is failed");
			exit(1);
		}
		int ppn = atoi(argv[3]);
		dd_read(ppn,pagebuf);//pagebuf에 파일 값 읽기
		
		memset(sectorbuf,0x00,SECTOR_SIZE);//sectorbuf에 있는 쓰레기값을 0x00으로 초기화
		memset(sparebuf,0x00,SPARE_SIZE);//sparebuf에 있는 쓰레기값을 0x00으로 초기화 
		memcpy(sectorbuf,pagebuf,SECTOR_SIZE);//pagebuf의 sector부분 복사 
		memcpy(sparebuf,pagebuf+SECTOR_SIZE,SPARE_SIZE);//pagebuf의 spare부분 복사 
		for(int i = 0; i < SECTOR_SIZE; i++){//0xFF전까지 sectorbuf 출력
			if(sectorbuf[i]!=0xFF){
				printf("%c",sectorbuf[i]);
			}
		}
		printf("\n");
		for(int i = 0; i < SPARE_SIZE; i++){//0xFF전까지 sparebuf 출력
			if(sparebuf[i]!=0xFF){
				printf("%c",sparebuf[i]);
			}
		}
		fclose(flashfp);
	}	
	
	else if(strcmp(argv[1],"e")==0){
		flashfp = fopen(argv[2],"r+");
		if(flashfp == NULL){
			fprintf(stderr,"file open is failed");
			exit(1);
		}
		int pbn = atoi(argv[3]);
		dd_erase(pbn);
		fclose(flashfp);
	}

	return 0;
}
