/*!
	This module defines function for single writer multiple reader share memory operation

	Company: PI Electronics (H.K.) Ltd
	Author : Terence
	Date: 20 Oct 2016
*/

#ifndef __SHM_MODULE_H__
#define __SHM_MODULE_H__

#define SHM_VERSION 20161027

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <semaphore.h>
#include <errno.h>
#include <string.h>
#include "timer.h"

#define SEM_DEBUG
#define SHM_FLAGS 3

//!< define the message macro
#define DEBUG_MSG(msg, ...) (fprintf(stderr, "\033[31m" "In file %s at line %d: " msg "\033[0m", __FILE__, __LINE__, ##__VA_ARGS__))

//!< struct of share memory
struct SHM{
	unsigned char *buf;		//!< share memory buffer 
	unsigned int *flags;	//!< flags[0] = read counter; flags[1] = timeout value in ns; flags[2] = memory_destoryed
	unsigned int size;		//!< size of share memory buffer

	unsigned int timeout_ns;	//!< timeout limit in nano second
	//!< lock variable
	sem_t *wt_lock;			//!< writer lock
	sem_t *rd_lock;			//!< reader lock
	sem_t *rdcnt_lock;		//!< reader counter lock

};	

class sharedMem{
	public:
		sharedMem(const char *filename);	//!< constructor
		int create(unsigned int shm_size, unsigned int timeout_ns);			//!< create the shm and corresponding semaphore with timeout setting
		int create(unsigned int shm_size); 	//!< create the shm and corresponding semaphore with default timeout 500us
		int link();							//!< link to the existing shm and return the size of shm
		int writeData(unsigned char *wbuf, int start, int length);	//!< write data into shm
		int flushData(); 	//! flush the shm to zeros
		int readData(unsigned char *rbuf, int start, int length);	//!< read data from shm
		int destroy();						//!< destroy the shm and its file descriptor in /dev/shm
		int destroy(const char *filepath);			//!< destroy the shm and its file descriptor in /dev/shm and save to the file path
		int unlink();						//!< unlink the shm
		~sharedMem();						//!< destructor
	
	private:
		char global_name[256];		//!< a string that all the file descriptors in /dev/shm will contain.
		char shm_name[256];			//!< share memnory buffer name in /dev/shm
		char flag_name[256];		//!< reader counter name in /dev/shm 
		char wt_lock_name[256];		//!< writer lock name in /dev/shm
		char rd_lock_name[256];		//!< reader lock name in /dev/shm
		char rdcnt_lock_name[256];	//!< reader counter lock name in /dev/shm

		struct SHM shm;				//!< share memory 
		int create_link_mutex; //!< 0 = not assiged, 1 = create, 2 = link
};

#endif
