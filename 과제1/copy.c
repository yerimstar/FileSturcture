#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
	FILE *ofp = fopen(argv[1],"r");
	FILE *cfp = fopen(argv[2],"w+");
	char buffer[100] = {0,};
	int size;

	if(argc != 3){
		printf("Usage %s <filename> \n",argv[0]);
		exit(1);
	}
	
	if(ofp == NULL){
		fprintf(stderr,"File open is failed");
		exit(1);
	}

	if(cfp == NULL){
		fprintf(stderr,"File open is failed");
		exit(1);
	}

	while(fread(buffer,1,100,ofp) > 0)
		fwrite(buffer,1,100,cfp);

	fclose(ofp);
	fclose(cfp);
	exit(0);
}

