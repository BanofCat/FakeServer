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
	} 
	status = shm.link();
	if (status < 0){
		DEBUG_MSG("link failed\n");
	} 

	status = shm.create(64);
	if (status < 0){
		DEBUG_MSG("create failed\n");
	} 

	unsigned char rdata[64];
	for (int i=0; i<64; i++){
		rdata[i] = 0;
	}

	status = shm.writeData(rdata, 0, 64);
	if (status < 0){ 
		DEBUG_MSG("after writeData()!\n");
	}

	status = shm.destroy();
	if (status < 0){ 
		DEBUG_MSG("destroy failed!\n");
	}

	status = shm.unlink();
	if (status < 0){ 
		DEBUG_MSG("unlink failed!\n");
	}

	status = shm.readData(rdata, 0, 64);
	if (status < 0){ 
		DEBUG_MSG("readData failed!\n");
	}

	status = shm.create(64);
	if (status < 0){
		DEBUG_MSG("create failed\n");
	}


	status = shm.link();
	if (status < 0){
		DEBUG_MSG("link failed\n");
	}

	status = shm.readData(rdata, 0, 64);
	if (status < 0){ 
		DEBUG_MSG("readData failed!\n");
	}


	for (int i=0; i<64; i++){
		printf("%d ", rdata[i]);
	}
	printf("\n");
	

	return 0;

}