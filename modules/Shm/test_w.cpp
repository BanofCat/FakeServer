#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "shm_module.h"


using namespace std;

int main(){
	int size = 64;
	int status;

	sharedMem shm("mytest");

	status = shm.create(size, 1000000);

	if (status < -1){
		fprintf(stderr, "create failed!\n");
	}

	unsigned char data[64], rdata[64];

	for (int i=0; i<64; i++){
		data[i] = i+1;
		rdata[i] = 0;
	}

	int loop = 0;
	while (loop < 10000){
		status = shm.writeData(data, 0, 64);
		if (status < -1){
			fprintf(stderr, "write failed!\n");
		}

		// shm.readData(rdata, 0, 64);
		usleep(1000);
		loop++;
	}
	shm.readData(rdata, 0, 64);
	for (int i=0; i<64; i++){
		printf("%d ", rdata[i]);
	}
	printf("\n");

	status = shm.flushData();
		if (status < -1){
			fprintf(stderr, "flush failed!\n");
	}

	shm.readData(rdata, 0, 64);
	for (int i=0; i<64; i++){
		printf("%d ", rdata[i]);
	}
	printf("\n");
	
	shm.destroy();
	return 0;

}