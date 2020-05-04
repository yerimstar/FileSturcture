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

typedef struct{
	int ppn;
}SectorMapEntry;
typedef struct{
	SectorMapEntry entry[DATAPAGES_PER_DEVICE];
}SectorMap;

SectorMap sectormap;
int freeblock;
char freepage[PAGES_PER_BLOCK*BLOCKS_PER_DEVICE];

//prototypes
void ftl_open();
void ftl_read(int lsn, char *sectorbuf);
void ftl_write(int lsn, char *sectorbuf);
void ftl_print();
int get_freepage();
int use_freeblock();
void update(int cur_ppn,int temp_ppn);
//
// flash memory를 처음 사용할 때 필요한 초기화 작업, 예를 들면 address mapping table에 대한
// 초기화 등의 작업을 수행한다. 따라서, 첫 번째 ftl_write() 또는 ftl_read()가 호출되기 전에
// file system에 의해 반드시 먼저 호출이 되어야 한다.
//
void ftl_open()
{
	//
	// address mapping table 초기화
	// free block's pbn 초기화
   	// address mapping table에서 lbn 수는 DATABLKS_PER_DEVICE 동일
	for(int i = 0; i < DATAPAGES_PER_DEVICE; i++){
		sectormap.entry[i].ppn = -1;
	}

	for(int i = 0; i < BLOCKS_PER_DEVICE*PAGES_PER_BLOCK; i++){
		freepage[i] = -1;
	}
	freeblock = DATABLKS_PER_DEVICE;
	return;
}

//
// 이 함수를 호출하기 전에 이미 sectorbuf가 가리키는 곳에 512B의 메모리가 할당되어 있어야 한다.
// 즉, 이 함수에서 메모리를 할당받으면 안된다.
//

void ftl_read(int lsn, char *sectorbuf)
{
	char pagebuf[PAGE_SIZE];
	int ppn = sectormap.entry[lsn].ppn;
	if(ppn == -1){
		printf("아직 매핑되어있는 값이 없습니다.\n");
		return;
	}
	else{
		memset(sectorbuf,(char)0xFF,SECTOR_SIZE);
		memset(pagebuf,(char)0xFF,PAGE_SIZE);
	
		dd_read(ppn,pagebuf);
		memcpy(sectorbuf,pagebuf,SECTOR_SIZE);
		return;
	}
}

void ftl_write(int lsn, char *sectorbuf)
{
	char tempbuf[PAGE_SIZE];
	char sparebuf[SPARE_SIZE];
	char buf[PAGE_SIZE];
	int garbage_lpn,garbage_pbn,garbage_ppn,freeblock_ppn;
	int ppn = sectormap.entry[lsn].ppn;
	memset(sparebuf,(char)0xFF,SPARE_SIZE);
	memset(tempbuf,(char)0xFF,PAGE_SIZE);
	memset(buf,(char)0xFF,PAGE_SIZE);

	dd_read(ppn,tempbuf);
	if(tempbuf[SECTOR_SIZE] == lsn){
		freepage[sectormap.entry[lsn].ppn] = 1;
	}//이미 저장되어있는 값이 있다면 그 값의 freepage값을 1로 바꿔준다. 

	if(use_freeblock()){//freeblock을 사용해야 하는지 검사해준다.
			ppn = sectormap.entry[lsn].ppn;
			sectormap.entry[lsn].ppn = get_freepage();
			freepage[sectormap.entry[lsn].ppn] = 0;
	}
	
	else{
		for(int i = 0; i < PAGES_PER_BLOCK*BLOCKS_PER_DEVICE; i++){
			if(i/PAGES_PER_BLOCK == freeblock)
				continue;
			if(freepage[i] == 1){
				garbage_pbn = i/PAGES_PER_BLOCK;
				break;
			}
		}
		for(int j = 0; j < PAGES_PER_BLOCK; j++){
			freeblock_ppn = freeblock*PAGES_PER_BLOCK+j;
			garbage_ppn = garbage_pbn*PAGES_PER_BLOCK+j;
			if(freepage[garbage_ppn] == 0){
				dd_read(garbage_ppn,buf);
				garbage_lpn = buf[SECTOR_SIZE];
				update(freeblock_ppn,garbage_ppn);
				freepage[freeblock_ppn] = 0;
				sectormap.entry[garbage_lpn].ppn = freeblock_ppn;
			}
			freepage[garbage_ppn] = -1;
		}	
		dd_erase(garbage_pbn);
		freeblock = garbage_pbn;

		sectormap.entry[lsn].ppn = get_freepage();
		freepage[sectormap.entry[lsn].ppn] = 0;

	}//가용 메모리에 더 이상 쓸 자리가 없다 -> freeblock 사용
		
	memset(tempbuf,(char)0xFF,PAGE_SIZE);
	ppn = sectormap.entry[lsn].ppn;
	memcpy(tempbuf,sectorbuf,strlen(sectorbuf));
	tempbuf[SECTOR_SIZE] = lsn;
	dd_write(ppn,tempbuf);
}
void update(int cur_ppn,int temp_ppn){//prev_ppn위치에 있던 값을 cur_ppn 즉 freeblock 으로 데이터값을 옮겨준다. 

	char curbuf[PAGE_SIZE];
	char prevbuf[PAGE_SIZE];
	memset(prevbuf,(char)0xFF,PAGE_SIZE);
	memset(curbuf,(char)0xFF,PAGE_SIZE);
	
	dd_read(temp_ppn,prevbuf);
	memcpy(curbuf,prevbuf,PAGE_SIZE);
	dd_write(cur_ppn,curbuf);
	
}
int use_freeblock(){//가용메모리에 쓸 자리가 있는지 검사하는 함수 
	for(int i = 0; i < PAGES_PER_BLOCK*BLOCKS_PER_DEVICE; i++){
		if(i/PAGES_PER_BLOCK == freeblock)
			continue;
		if(freepage[i] == -1)
			return 1;
	}
	return 0;
}
int get_freepage(){
	for(int j = 0; j < PAGES_PER_BLOCK*BLOCKS_PER_DEVICE; j++){
		if(j/PAGES_PER_BLOCK == freeblock)
			continue;
		if(freepage[j] == -1)
			return j;
	}

}

void ftl_print()
{
	printf("lpn ppn\n");
	for(int i = 0; i < DATAPAGES_PER_DEVICE; i++){
		printf("%d %d\n", i, sectormap.entry[i].ppn);
	}
	printf("freeblock's pbn = %d\n",freeblock);
	return;
}
