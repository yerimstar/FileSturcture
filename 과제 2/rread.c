//Random read
#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/stat.h>

#define SUFFLE_NUM	10000
void GenRecordSequence(int *list, int n);
void swap(int *a, int *b);

int main(int argc, char **argv){
	int *read_order_list;
	struct stat file_info;
	struct timeval start,end;
	stat(argv[1],&file_info);
	int num_of_records = file_info.st_size/100;
	int times1,times2;
	FILE *fp = fopen(argv[1],"r");
	char buffer[100];

	read_order_list = (int*)calloc(num_of_records,sizeof(int));
	for(int i = 0; i < num_of_records; i++)
		read_order_list[i] = i;
	GenRecordSequence(read_order_list,num_of_records);

	gettimeofday(&start,NULL);
	for(int i = 0; i < num_of_records; i++){
		fseek(fp,read_order_list[i]*100,SEEK_SET);
		fread(buffer,1,100,fp);
	}
	gettimeofday(&end,NULL);
	free(read_order_list);
	fclose(fp);
	
	times2 = (end.tv_sec-start.tv_sec)*1000000;
	times1 = end.tv_usec-start.tv_usec;
	printf("# records : %d timecost : %d us\n",num_of_records,times1+times2);
	return 0;
	}

void GenRecordSequence(int *list, int n)
{
	int i, j, k;

	srand((unsigned int)time(0));

	for(i=0; i<n; i++)
	{
		list[i] = i;
	}
	
	for(i=0; i<SUFFLE_NUM; i++)
	{
		j = rand() % n;
		k = rand() % n;
		swap(&list[j], &list[k]);
	}

	return;
}

void swap(int *a, int *b)
{
	int tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;

	return;
}
