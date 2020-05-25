#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "person.h"
//필요한 경우 헤더 파일과 함수를 추가할 수 있음

// 과제 설명서대로 구현하는 방식은 각자 다를 수 있지만 약간의 제약을 둡니다.
// 레코드 파일이 페이지 단위로 저장 관리되기 때문에 사용자 프로그램에서 레코드 파일로부터 데이터를 읽고 쓸 때도
// 페이지 단위를 사용합니다. 따라서 아래의 두 함수가 필요합니다.
// 1. readPage(): 주어진 페이지 번호의 페이지 데이터를 프로그램 상으로 읽어와서 pagebuf에 저장한다
// 2. writePage(): 프로그램 상의 pagebuf의 데이터를 주어진 페이지 번호에 저장한다
// 레코드 파일에서 기존의 레코드를 읽거나 새로운 레코드를 쓰거나 삭제 레코드를 수정할 때나
// 모든 I/O는 위의 두 함수를 먼저 호출해야 합니다. 즉 페이지 단위로 읽거나 써야 합니다.

//
// 페이지 번호에 해당하는 페이지를 주어진 페이지 버퍼에 읽어서 저장한다. 페이지 버퍼는 반드시 페이지 크기와 일치해야 한다.
//
typedef struct headerpage{
	int entire_pagenum;
	int entire_recordnum;
	int pagenum;
	int recordnum;
}header;

void readPage(FILE *fp, char *pagebuf, int pagenum)
{	
	fseek(fp,PAGE_SIZE*pagenum,SEEK_SET);
	fread(pagebuf,PAGE_SIZE,1,fp);
	return;
}

//
// 페이지 버퍼의 데이터를 주어진 페이지 번호에 해당하는 위치에 저장한다. 페이지 버퍼는 반드시 페이지 크기와 일치해야 한다.
//
void writePage(FILE *fp, const char *pagebuf, int pagenum)
{
	fseek(fp,PAGE_SIZE*pagenum,SEEK_SET);
	fwrite(pagebuf,PAGE_SIZE,1,fp);
	return;
}

//
// 새로운 레코드를 저장할 때 터미널로부터 입력받은 정보를 Person 구조체에 먼저 저장하고, pack() 함수를 사용하여
// 레코드 파일에 저장할 레코드 형태를 recordbuf에 만든다. 그런 후 이 레코드를 저장할 페이지를 readPage()를 통해 프로그램 상에
// 읽어 온 후 pagebuf에 recordbuf에 저장되어 있는 레코드를 저장한다. 그 다음 writePage() 호출하여 pagebuf를 해당 페이지 번호에
// 저장한다. pack() 함수에서 readPage()와 writePage()를 호출하는 것이 아니라 pack()을 호출하는 측에서 pack() 함수 호출 후
// readPage()와 writePage()를 차례로 호출하여 레코드 쓰기를 완성한다는 의미이다.
// 
void pack(char *recordbuf, const Person *p)
{
	sprintf(recordbuf,"%s#%s#%s#%s#%s#%s#",p->sn,p->name,p->age,p->addr,p->phone,p->email);
}

// 
// 아래의 unpack() 함수는 recordbuf에 저장되어 있는 레코드를 구조체로 변환할 때 사용한다. 이 함수가 언제 호출되는지는
// 위에서 설명한 pack()의 시나리오를 참조하면 된다.
//
void unpack(const char *recordbuf, Person *p)
{
	char *field;
	char record[RECORD_SIZE];
	strcpy(record,recordbuf);
	
	if(recordbuf[0] == '*'){
		memset(p,(char)0xFF,sizeof(Person));
		return;
	}
	field = strtok(record,"#");
	strcpy(p->sn,field);
	field = strtok(NULL,"#");
	strcpy(p->name,field);
	field = strtok(NULL,"#");
	strcpy(p->age,field);
	field = strtok(NULL,"#");
	strcpy(p->addr,field);
	field = strtok(NULL,"#");
	strcpy(p->phone,field);
	field = strtok(NULL,"#");
	strcpy(p->email,field);
	
}

//
// 새로운 레코드를 저장하는 기능을 수행하며, 터미널로부터 입력받은 필드값을 구조체에 저장한 후 아래의 insert() 함수를 호출한다.
//
void insert(FILE *fp, const Person *p)
{
	header h;
	char recordbuf[RECORD_SIZE];
	char pagebuf[PAGE_SIZE];
	char headerbuf[PAGE_SIZE];
	char buf[8];
	int maxrecord = PAGE_SIZE/RECORD_SIZE;
	memset(recordbuf,(char)0xFF,RECORD_SIZE);
	memset(headerbuf,(char)0xFF,PAGE_SIZE);
	memset(buf,(char)0xFF,8);

	pack(recordbuf,p);
	readPage(fp,headerbuf,0);
	memcpy(&h,headerbuf,sizeof(header));
	
	if((h.recordnum != -1) && (h.pagenum != -1)){
		memset(pagebuf,(char)0xFF,PAGE_SIZE);
		readPage(fp,pagebuf,h.pagenum);
		memcpy(buf,pagebuf+RECORD_SIZE*h.recordnum+1,sizeof(int)*2);
		memset(pagebuf+RECORD_SIZE*h.recordnum,(char)0xFF,RECORD_SIZE);
		memcpy(pagebuf+RECORD_SIZE*h.recordnum,recordbuf,RECORD_SIZE);
		writePage(fp,pagebuf,h.pagenum);
		memcpy(headerbuf+sizeof(int)*2,buf,sizeof(int)*2);
		writePage(fp,headerbuf,0);
		
	}// 삭제 record가 있을 때
	else{
		if((maxrecord*(h.entire_pagenum-1)) > h.entire_recordnum){
			memset(pagebuf,(char)0xFF,PAGE_SIZE);
			int lastpage_recordnum = (h.entire_recordnum-((h.entire_pagenum-2)*maxrecord));
			readPage(fp,pagebuf,h.entire_pagenum-1);
			memcpy(pagebuf+lastpage_recordnum*RECORD_SIZE,recordbuf,RECORD_SIZE);
			writePage(fp,pagebuf,h.entire_pagenum-1);
			h.entire_recordnum += 1;
			memcpy(headerbuf,&h,sizeof(h));
			writePage(fp,headerbuf,0);
		}
		else if((maxrecord*(h.entire_pagenum-1)) == h.entire_recordnum){
			memset(pagebuf,(char)0xFF,PAGE_SIZE);
			memcpy(pagebuf,recordbuf,RECORD_SIZE);
			writePage(fp,pagebuf,h.entire_pagenum);
			h.entire_pagenum++;
			h.entire_recordnum++;
			memcpy(headerbuf,&h,sizeof(h));
			writePage(fp,headerbuf,0);
		}
		
	}// 삭제 record가 없을 때
}

//
// 주민번호와 일치하는 레코드를 찾아서 삭제하는 기능을 수행한다.
//
void delete(FILE *fp, const char *sn)
{
	Person p;
	header h;
	
	char pagebuf[PAGE_SIZE];
	char headerbuf[PAGE_SIZE];
	char recordbuf[RECORD_SIZE];
	char tempbuf[RECORD_SIZE];
	memset(pagebuf,(char)0xFF,PAGE_SIZE);
	memset(headerbuf,(char)0xFF,PAGE_SIZE);
	memset(recordbuf,(char)0xFF,RECORD_SIZE);
	memset(tempbuf,(char)0xFF,RECORD_SIZE);

	char delete_mark = '*';
	int maxrecord = PAGE_SIZE/RECORD_SIZE;
	
	readPage(fp,headerbuf,0);
	memcpy(&h,headerbuf,sizeof(header));
	
	for(int i = 1; i < h.entire_pagenum; i++){
		readPage(fp,pagebuf,i);
		for(int j = 0; j < maxrecord; j++){
			memcpy(recordbuf,pagebuf+RECORD_SIZE*j,RECORD_SIZE);
			memcpy(tempbuf,recordbuf,RECORD_SIZE);
			unpack(recordbuf,&p);
			if(strcmp(p.sn,sn) == 0){
				memcpy(tempbuf,&delete_mark,sizeof(char));
				memcpy(tempbuf+sizeof(char),&h.pagenum,sizeof(int));
				memcpy(tempbuf+sizeof(char)+sizeof(int),&h.recordnum,sizeof(int));
				
				memset(pagebuf,(char)0xFF,PAGE_SIZE);
				readPage(fp,pagebuf,i);		
				memcpy(pagebuf+RECORD_SIZE*j,tempbuf,RECORD_SIZE);
				writePage(fp,pagebuf,i);
			
				h.pagenum = i;
				h.recordnum = j;
				memcpy(headerbuf,&h,sizeof(h));
				writePage(fp,headerbuf,0);
			
				return;
			}
		}
	}
	
}	

void first_header(FILE *fp){
	header h;
	char headerbuf[PAGE_SIZE];
	memset(headerbuf,(char)0xFF,PAGE_SIZE);

	h.entire_pagenum = 1;
	h.entire_recordnum = 0;
	h.pagenum = -1;
	h.recordnum = -1;

	memcpy(headerbuf,&h,sizeof(h));
	writePage(fp,headerbuf,0);
}
int main(int argc, char *argv[])
{
	FILE *fp;  // 레코드 파일의 파일 포인터
	Person p;

	if(!strcmp(argv[1],"i")){
		fp = fopen(argv[2],"r+");
		if(fp  == NULL){
			fp = fopen(argv[2],"w+");
			first_header(fp);
		}
		strcpy(p.sn,argv[3]);
		strcpy(p.name,argv[4]);
		strcpy(p.age,argv[5]);
		strcpy(p.addr,argv[6]);
		strcpy(p.phone,argv[7]);
		strcpy(p.email,argv[8]);
		insert(fp,&p);
	}
	else if(!strcmp(argv[1],"d")){
		fp = fopen(argv[2],"r+");
		delete(fp,argv[3]);
	}
	return 1;
}
