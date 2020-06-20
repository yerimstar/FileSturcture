#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "person.h"

int pagenum,recordnum;
char **temparray;
int heapcount = 0;

void readPage(FILE *fp, char *pagebuf, int pagenum)
{
	fseek(fp,pagenum*PAGE_SIZE,SEEK_SET);
	fread(pagebuf,PAGE_SIZE,1,fp);
	return;

}
void writePage(FILE *fp, const char *pagebuf, int pagenum)
{
	fseek(fp,pagenum*PAGE_SIZE,SEEK_SET);
	fwrite(pagebuf,PAGE_SIZE,1,fp);
	return;
}
						
void buildHeap(FILE *inputfp, char **heaparray){
	char temp[RECORD_SIZE];
	for(int i = 1; i <= recordnum; i++){
		strcpy(heaparray[++heapcount],temparray[i]);

		int child = heapcount;
		int parent = child/2;
		while(child > 1 && (atoll(heaparray[parent]) > atoll(heaparray[child]))){
			strcpy(temp,heaparray[parent]);
			strcpy(heaparray[parent],heaparray[child]);
			strcpy(heaparray[child],temp);
			child = parent;
			parent = child/2;
		}
	}

}

void makeSortedFile(FILE *outputfp, char **heaparray)
{
	int num = PAGE_SIZE/RECORD_SIZE;
	char pagebuf[PAGE_SIZE];
	int pagecount = 1;
	int recordcount = 1;
	char temp[RECORD_SIZE];
	char tmp[RECORD_SIZE];

	while(pagenum > pagecount){
		memset(pagebuf,(char)0xFF,PAGE_SIZE);
		for(int i = 0; i < num; i++){	
		memset(temp,(char)0xFF,RECORD_SIZE);
			if(recordnum < recordcount)
				break;
			strcpy(temp,heaparray[1]);
			strcpy(heaparray[1],heaparray[heapcount--]);
			int parent = 1;
			int child = 2;
			while(child <= heapcount){
				if(child < heapcount && atoll(heaparray[child]) > atoll(heaparray[child+1]))
					child++;
				if(atoll(heaparray[parent]) > atoll(heaparray[child])){
					strcpy(tmp,heaparray[parent]);
					strcpy(heaparray[parent],heaparray[child]);
					strcpy(heaparray[child],tmp);
				}
				parent = child;
				child *= 2;
			}
			memcpy(pagebuf+i*RECORD_SIZE,temp,RECORD_SIZE);
			recordcount++;
		}
		writePage(outputfp,pagebuf,pagecount);
		pagecount++;
	}
	
}


int main(int argc, char *argv[])
{
	FILE *inputfp;	
	FILE *outputfp;	
	char pagebuf[PAGE_SIZE];
	char **heaparray;
	int num = PAGE_SIZE/RECORD_SIZE;
	int recordindex = 1;

	if(!strcmp(argv[1],"s")){
		inputfp = fopen(argv[2],"r");
		outputfp = fopen(argv[3],"w+");
	}
	
	if(inputfp == NULL || outputfp == NULL){
		fprintf(stderr,"file open is failed");
		exit(1);
	}

	readPage(inputfp,pagebuf,0);
	strncpy((char*)&pagenum,pagebuf,sizeof(int));
	strncpy((char*)&recordnum,&pagebuf[4],sizeof(int));
	writePage(outputfp,pagebuf,0);

	temparray = malloc(sizeof(char*)*(recordnum+1));
	heaparray = malloc(sizeof(char*)*(recordnum+1));

	for(int i = 0; i <= recordnum; i++){
		temparray[i] = malloc(sizeof(char)*RECORD_SIZE);
		heaparray[i] = malloc(sizeof(char)*RECORD_SIZE);
	}
	
	for(int i = 1; i < pagenum; i++){
		readPage(inputfp,pagebuf,i);
		for(int j = 0; j < num; j++){
			if(recordindex <= recordnum)
				strncpy(temparray[recordindex++],&pagebuf[j*RECORD_SIZE],RECORD_SIZE);
		}
	}//배열에 person.dat내용을 page단위로 읽어서 저장한다.
	

	buildHeap(inputfp,heaparray);
	makeSortedFile(outputfp,heaparray);

	fclose(inputfp);
	fclose(outputfp);//파일 닫기 

	return 1;
}

