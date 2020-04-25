// 주의사항
// 1. sectormap.h에 정의되어 있는 상수 변수를 우선적으로 사용해야 함
// 2. sectormap.h에 정의되어 있지 않을 경우 본인이 이 파일에서 만들어서 사용하면 됨
// 3. 필요한 data structure가 필요하면 이 파일에서 정의해서 쓰기 바람(sectormap.h에 추가하면 안됨)

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include "sectormap.h"
// 필요한 경우 헤더 파일을 추가하시오.
FILE *flashfp;

int dd_read(int ppn, char *pagebuf);
int dd_write(int ppn, char *pagebuf);
int dd_erase(int pbn);

void ftl_open();
void ftl_read(int lsn, char *sectorbuf);
void ftl_write(int lsn, char *sectorbuf);
void ftl_print();

int FTLtable[DATAPAGES_PER_DEVICE];
//
// flash memory를 처음 사용할 때 필요한 초기화 작업, 예를 들면 address mapping table에 대한
// 초기화 등의 작업을 수행한다. 따라서, 첫 번째 ftl_write() 또는 ftl_read()가 호출되기 전에
// file system에 의해 반드시 먼저 호출이 되어야 한다.
//
int main(){
	char blockbuf[BLOCK_SIZE];
	char sectorbuf[SECTOR_SIZE];
	
	flashfp = fopen("flashmemory","w");
	if(flashfp == NULL){
		fprintf(stderr,"file open is failed");
		exit(1);
	}
	memset(blockbuf,(char)0xFF,BLOCK_SIZE);
	for(int i = 0; i < BLOCKS_PER_DEVICE; i++)
		fwrite(blockbuf,BLOCK_SIZE,1,flashfp); //flashmemory 생성 및 초기화
	ftl_open();
	//test
	ftl_write(0,"hello");
	ftl_write(1,"hello");
	ftl_write(2,"hello");
	ftl_write(3,"hello");
	ftl_read(2,sectorbuf);
	ftl_print();
	fclose(flashfp);
}
void ftl_open()
{
	// address mapping table 초기화
	// free block's pbn 초기화
    // address mapping table에서 lbn 수는 DATABLKS_PER_DEVICE 동일
	int freeblock = DATABLKS_PER_DEVICE;
	for(int i = 0; i < DATAPAGES_PER_DEVICE; i++){
		FTLtable[i] = -1;
	}
	return;
}

//
// 이 함수를 호출하기 전에 이미 sectorbuf가 가리키는 곳에 512B의 메모리가 할당되어 있어야 한다.
// 즉, 이 함수에서 메모리를 할당받으면 안된다.
//
void ftl_read(int lsn, char *sectorbuf)
{
	char pagebuf[PAGE_SIZE];
	char printsector[SECTOR_SIZE];
	int ppn;
	char c = 0xFF;

	for(int i = 0; i < DATAPAGES_PER_DEVICE; i++){
		if(i == lsn){
			ppn = FTLtable[i];
		}
	}
	dd_read(ppn,pagebuf);
	memset(sectorbuf,0x00,SECTOR_SIZE);
	memcpy(sectorbuf,pagebuf,SECTOR_SIZE);
	
	int j = 0;
	for(int i = 0; i < SECTOR_SIZE; i++){
		if(sectorbuf[i] != c){
			printsector[j++] = sectorbuf[i];
		}
	}
	if(strlen(printsector)>0){
		printf("%s\n",printsector);
	}
	return;
}


void ftl_write(int lsn, char *sectorbuf)
{
	char pagebuf[PAGE_SIZE];
	int ppn;

	for(int i = 0; i < DATAPAGES_PER_DEVICE; i++){
		if(i == lsn){
			 ppn = FTLtable[i];	 
		}
	}
	if(ppn == -1){
		for(int i = 0; i < DATAPAGES_PER_DEVICE; i++){
			if(i == lsn){
				FTLtable[i] = rand() % DATAPAGES_PER_DEVICE;
					for(int j = 0; j < i; j++){
						if(FTLtable[i] == FTLtable[j])
							i--;
					}
			}
			ppn = FTLtable[i];
		}
		memset(pagebuf,0xFF,PAGE_SIZE);
		memcpy(pagebuf,sectorbuf,strlen(sectorbuf));
		//memcpy(pagebuf+SECTOR_SIZE,temp,strlen(temp));
		dd_write(ppn,pagebuf);
	}
	else{
		
	}

	return;
}

void ftl_print()
{	
	printf("lpn  ppn\n");
	for(int i = 0; i < DATAPAGES_PER_DEVICE; i++){
		printf(" %d    %d\n",i,FTLtable[i]);
	}
	//printf("free block's pbn = %d",freeblock);

	return;
}
