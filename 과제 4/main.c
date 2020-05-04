#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "sectormap.h"

extern FILE *flashfp;

void ftl_open();
void ftl_read(int lsn, char *sectorbuf);
void ftl_write(int lsn, char *sectorbuf);
void ftl_print();

void initialize_flash_memory();

int main(int argc, char *argv[])
{
	// 아래 세 개의 변수는 테스트할 때 필요하면 사용하기 바람
	FILE *workloadfp;
	char sectorbuf[SECTOR_SIZE];
	int lpn;

	// 가상 flash memory의 파일명은 'flashmemory'을 가정함
	flashfp = fopen("flashmemory", "w+");	

	if(flashfp == NULL)
	{
		printf("file open error\n");
		exit(1);
	}

	initialize_flash_memory();

	// ftl_write() 및 ftl_read() 테스트를 위한 코드를 자유자재로 만드세요
	ftl_open();

	ftl_write(1, "abcde");
	ftl_write(2, "nct127");
	ftl_write(2, "nctdream");
	ftl_write(5, "ridin");
	ftl_read(3,sectorbuf);
	ftl_write(7, "hello");
	ftl_write(4, "jeno smile");
	ftl_write(0, "I love jisung");
	ftl_write(6, "fullsun");
/*	ftl_write(8, "chewing gum");
	ftl_write(9, "my first and last");
	ftl_write(10, "we young");
	ftl_write(11,"go");
	ftl_write(12,"we go up");
	ftl_write(13,"boom");
	ftl_write(14,"ridin");
	ftl_write(15,"quiet down");
	*/

	ftl_write(7, "renjun");
	ftl_write(3, "music star chenle");
	ftl_write(1, "nana the rabit");
	
	ftl_print();
	
//	for(int i  = 0; i < DATAPAGES_PER_DEVICE; i++){
//		ftl_read(i,sectorbuf);
//		printf("%s\n",sectorbuf);
//	}
	fclose(flashfp);

	return 0;
}

//
// initialize flash memory where each byte are set to 'OxFF'
// 
void initialize_flash_memory()
{
    char *blockbuf;
	int i;

	blockbuf = (char *)malloc(BLOCK_SIZE);
	memset(blockbuf, 0xFF, BLOCK_SIZE);

	for(i = 0; i < BLOCKS_PER_DEVICE; i++)
	{
		fwrite(blockbuf, BLOCK_SIZE, 1, flashfp);
	}

	free(blockbuf);

	return;
}
