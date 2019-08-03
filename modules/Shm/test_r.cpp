#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "shm_module.h"


using namespace std;

int main(){
	int size = 64;
	int status = 1;

	sharedMem shm("mytest");

	status = shm.link();
	if (status < 0){
		DEBUG_MSG("link failed\n");
		return 0;
	} 
	
	unsigned char rdata[64];
	for (int i=0; i<64; i++){
		rdata[i] = 0;
	}
	int loop = 0;

	struct timespec t1;
	float tdif = 0.0;

	while (loop < 1000000000){
		t1 = tic();
		status = shm.readData(rdata, 0, 64);

		if (status < 0){ 
			DEBUG_MSG("readData failed!\n");
		}
		usleep(1000);
		loop++;
		t1 = toc(t1, &tdif);
		if (loop%1000 == 0){
			fprintf(stderr, "time = %f us.\n", tdif);
		}
	}

	for (int i=0; i<64; i++){
		printf("%d ", rdata[i]);
	}
	printf("\n");
	

	return 0;

}