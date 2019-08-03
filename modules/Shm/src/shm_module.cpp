#include "shm_module.h"

//! Constructor
//! Prepear the elements needed for creating the share memory and semaphore
sharedMem::sharedMem(const char *filename){

	//! gerate the shmbuf, readcount and sem file name for later use
	sprintf(global_name, "%s", filename); 		//!< save the filename to global_name variable
	sprintf(shm_name, "%s.shm", filename);		//!< generate name filename.shm for shm buffer
	sprintf(flag_name, "%s_flag.shm", filename); 		//!< generate name filename_rdcnt.shm for reader counter buffer
	sprintf(wt_lock_name, "%s_wlock", filename); 		//!< generate name filename_wlock for writer semaphore
	sprintf(rd_lock_name, "%s_rlock", filename);		//!< generate name filename_rlock for reader semaphore
	sprintf(rdcnt_lock_name, "%s_rclock", filename);	//!< generate name filename_rclock for reader counter semaphore

	shm.size = 0; 		//!< init the share memory size to 0
	shm.timeout_ns = 500000; //!< defaul timeout for semaphore is 500us
	create_link_mutex = 0; 		// init the mutex for create and link
}


//! Create a share memory buffer with shm_size unsigned char
int sharedMem::create(unsigned int shm_size, unsigned int timeout_ns)
{
	//! if create_link_mutex is not equal to 0, means either create() or link() is run for at least once.
	if (create_link_mutex != 0 ){
		if (create_link_mutex == 1){
			fprintf(stderr, "Error: Share memory has already created, ");
			fprintf(stderr, "it can not be created again! You need to ");
			fprintf(stderr, "destroy it before recreation!\n");
		}
		if (create_link_mutex == 2){
			fprintf(stderr, "Error: link() operation is used for ");
			fprintf(stderr, "this memory, create() is mutually exclusive ");
			fprintf(stderr, "to link() for a sharedMem object!\n");
		}
		return -1;
	}

	if (shm_size <= 0){
		fprintf(stderr, "Error: The memory size must be larger than 0.\n");
		return -1;
	}

	//! if create_link_mutex is 0, create() runs.
	if (timeout_ns < 100000)
	{
		fprintf(stderr, "Error: timeout value set is %d ns which is less than 100 us, which may cause problem.\n", timeout_ns);
		return -1;
	}

	int fd_buf, fd_flags; 		//!< file descriptors for share memory buffer and reader counter

	fd_buf = shm_open(shm_name, O_RDWR | O_CREAT | O_EXCL, S_IRWXU);  //!< create a share memory descriptor with RWX access
	fd_flags = shm_open(flag_name, O_RDWR | O_CREAT | O_EXCL, S_IRWXU); //!< create a reader counter descriptor with RWX access

	if (fd_buf < 0 || fd_flags < 0) 	//!< check if file already exist!
	{
		fprintf(stderr, "Error: fail to create the shm, Other program may be using the share memory with the same name.\n");
		fprintf(stderr, "If you know what you are doing, delete all %s*.shm in /dev/shm/\n", global_name);
		return -1;
	}

	if(ftruncate(fd_buf, shm_size) == -1) 	//!< set the share memory buffer to the shm_size
	{
		fprintf(stderr, "ftruncate shmbuf failed --- %s!\n", strerror(errno));
		return -1;
	}

	if(ftruncate(fd_flags, SHM_FLAGS*sizeof(unsigned int)) == -1) 	//!< set the reader counter size to 2 integers
	{
		fprintf(stderr, "ftruncate readcounter failed --- %s!\n", strerror(errno));
		return -1;
	}

	//! map the memory to physical address and set the access right
	shm.buf = (unsigned char *)mmap(NULL, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_buf, 0);
	shm.flags = (unsigned int*)mmap(NULL, SHM_FLAGS*sizeof(unsigned int), PROT_READ | PROT_WRITE, MAP_SHARED, fd_flags, 0);
	shm.flags[0] = 0; //!< init reader counter to 0;
	shm.size = shm_size;

	//! flags[1] is timeout value
	shm.timeout_ns = timeout_ns;
	shm.flags[1] = shm.timeout_ns;		//!< set the timeout value to the global register for other processes

	//! flags[2] = 0, memory still exist, flags[2]= 1, memory is destroyed.
	shm.flags[2] = 0;

	//! semaphore initiallization
	shm.wt_lock = sem_open(wt_lock_name, O_CREAT | O_EXCL, 0666, 1); 		//!< create wt_lock and init it to 1
	shm.rd_lock = sem_open(rd_lock_name, O_CREAT | O_EXCL, 0666, 1); 		//!< create rd_lock and init it to 1
	shm.rdcnt_lock = sem_open(rdcnt_lock_name, O_CREAT | O_EXCL, 0666, 1); 	//!< create rdcnt_lock and init it to 1

	//! if either lock initialization failed
	if (shm.wt_lock == SEM_FAILED || shm.wt_lock == SEM_FAILED || shm.rdcnt_lock == SEM_FAILED)
	{
		fprintf(stderr, "Error: fail to create the locks, Other program may be using the locks with the same name.\n");
		fprintf(stderr, "If you know what you are doing, delete all sem.%s* in /dev/shm/\n", global_name);
		return -1;
	}

	close(fd_buf); 		//!< close the file descriptor after mmap will not after the share memory buffer
	close(fd_flags);	//!< close the file descriptor after mmap will not after the share memory buffer
	create_link_mutex = 1; 		//!< set the create_link_mutex to 1
	return 1;
}

//! overload the create method with default timeout setting, timeout = 500us
int sharedMem::create(unsigned int shm_size)
{
	return create(shm_size, shm.timeout_ns);
}

//! link to a existing share memory buffer
int sharedMem::link()
{
	//! if create_link_mutex is not equal to 0, means either create() or link() is run for at least once.
	if (create_link_mutex != 0 ){
		if (create_link_mutex == 2){
			fprintf(stderr, "Error: SharedMem has already linked to a memory, ");
			fprintf(stderr, "can not be linked again! You can unlink the ");
			fprintf(stderr, "current memory and relink to another one!\n");
		}
		if (create_link_mutex == 1){
			fprintf(stderr, "Error: create() operation is used for ");
			fprintf(stderr, "this memory, link() is mutually exclusive to ");
			fprintf(stderr, "create() for a sharedMem object!\n");
		}
		return -1;
	}

	//! if create_link_mutex is 0, link() runs.
	struct stat shmstat;	//!< status variable for checking the file descriptor's status
	int fd_buf, fd_flags;	//!< file descriptors for share memory buffer and reader counter
	fd_buf = shm_open(shm_name, O_RDONLY, 0);		//!< open the file descriptor of share memory buffer with READ ONLY access
	fd_flags = shm_open(flag_name, O_RDWR, 0);		//!< open the file descriptor of reader counter with RW access

	if (fd_buf < 0 || fd_flags < 0)		//!< check if file exist!
	{
		fprintf(stderr, "Error: file %s or %s does not exist, please create them first!\n", shm_name, flag_name);
		return -1;
	}

	if(fstat(fd_buf, &(shmstat)) == -1) 	//!< get the file descriptor status
	{
		fprintf(stderr, "get shared memory stat error\n");
		return -1;
	}

	shm.size = shmstat.st_size;	//!< shmstat.st_size is in the unit of bytes
	// printf("shm.size = %d\n", shm.size);
	shm.buf = (unsigned char *)mmap(NULL, shm.size, PROT_READ, MAP_SHARED, fd_buf, 0);	//!< map the variable the physical address
	shm.flags = (unsigned int *)mmap(NULL, SHM_FLAGS*sizeof(unsigned int), PROT_READ | PROT_WRITE, MAP_SHARED, fd_flags, 0);
	shm.timeout_ns = shm.flags[1];		//!< get the timeout value from the register
	fprintf(stderr, "time out for lock is set to %dns\n", shm.timeout_ns);

	// semaphore initiallization
	shm.wt_lock = sem_open(wt_lock_name, 0); 		//!< link to wt_lock
	if (shm.wt_lock == SEM_FAILED)
	{
		fprintf(stderr, "open wt_lock fail!! %s\n", strerror(errno));
		return -1;
	}
	shm.rd_lock = sem_open(rd_lock_name, 0); 		//!< link to rd_lock
	if (shm.rd_lock == SEM_FAILED)
	{
		fprintf(stderr, "open rd_lock fail!!\n");
		return -1;
	}
	shm.rdcnt_lock = sem_open(rdcnt_lock_name, 0); 		//!< link to rdcnt_lock
	if (shm.rdcnt_lock == SEM_FAILED)
	{
		fprintf(stderr, "open rdcnt lock fail!!\n");
		return -1;
	}

	close(fd_buf); 		//!< close the file descriptor after mmap will not after the share memory buffer
	close(fd_flags);	//!< close the file descriptor after mmap will not after the share memory buffer
	create_link_mutex = 2; 		//!< set the create_link_mutex to 2
	return shm.size;		//!< return the size of the share memory buffer in the unit of byte
}

//! write data into the share memory buffer
//! int start: starting index
//! int length: lenght of the memory segment
//! eg. if start = 2, length = 10, date is at index of [2, 3, 4, ..., 11]
int sharedMem::writeData(unsigned char *wbuf, int start, int length)
{
	#ifdef SEM_DEBUG
	int val;	//!< variable for checking semaphore value in debug mode
	#endif

	struct timespec ts;		//!< time variable

	if (create_link_mutex == 2) 	//!< check the access right, only create can do the write operation
	{
	    fprintf(stderr, "Error: Write operation denied. You can only read the memory since ");
	    fprintf(stderr, "you are not the creator of this memory!\n");
	    return -1;
	}

	if( (start+length) > shm.size )		//!< check the input size is valid
	{
		fprintf(stderr, "Error: WriteData size exceeded the ");
		fprintf(stderr, "share memory max size!\n");
		return -1;
	}

	semtimeout(&ts,shm.timeout_ns);	//!< set the time for timeout

	struct timespec t1;
	float tdif;

	t1 = tic();
	if (sem_timedwait(shm.rd_lock, &ts) == -1) 		//!< block the readers
	{
		fprintf(stderr,"%c%s\n",getlocaltime(), strerror(errno) );
		#ifdef SEM_DEBUG
			sem_getvalue(shm.rd_lock, &val);
			DEBUG_MSG("rd_lock timeout from write!!--%d\n",val);
		#endif
		return -1;
	}
	t1 = toc(t1, &tdif);
	//fprintf(stderr, "time for get lock = %fus\n", tdif);


	semtimeout(&ts,shm.timeout_ns);
	if (sem_timedwait(shm.wt_lock, &ts) == -1)        //!< writer get the write lock
	{
		fprintf(stderr,"%c%s\n",getlocaltime(),strerror(errno) );
		#ifdef SEM_DEBUG
			sem_getvalue(shm.wt_lock, &val);
			DEBUG_MSG("wt_lock timeout from write!!--%d\n",val);
		#endif

		sem_post(shm.rd_lock); 	//!< if get lock fails,release the lock which we got and then return

		return -1;
	}

	// Start writing
	int kk = 0;
	for (int i=start; i<start+length; i++){
		shm.buf[i] = wbuf[kk];
		kk++;
	}
	sem_post(shm.wt_lock);		//!< Writer release the write lock
	sem_post(shm.rd_lock);      //!< release the readers
	// fprintf(stderr, "reader counter %d.\n", shm.flags[0]);

	return 1;
}

int sharedMem::flushData(){

	struct timespec t1;
	float tdif = 0.0;

	if (create_link_mutex == 0){
		fprintf(stderr, "Error: No memory exist, can not flush!\n");
	    return -1;
	}

	if (create_link_mutex == 2) 	//!< check the access right, only create can do the write operation
	{
	    fprintf(stderr, "Error: flush operation denied. You can only read the memory since ");
	    fprintf(stderr, "you are not the creator of this memory!\n");
	    return -1;
	}

	t1 = tic();
	if (sem_wait(shm.rd_lock) == -1) 		//!< block the readers
	{
		fprintf(stderr,"%c%s\n",getlocaltime(), strerror(errno) );
		return -1;
	}
	
	if (sem_wait(shm.wt_lock) == -1)        //!< writer get the write lock
	{
		fprintf(stderr,"%c%s\n",getlocaltime(),strerror(errno) );
		sem_post(shm.rd_lock); 	//!< if get lock fails,release the lock which we got and then return 
		
		return -1;
	}

	// Start setting zeros 
	memset(shm.buf, 0, sizeof(char)*shm.size);
	
	sem_post(shm.wt_lock);		//!< Writer release the write lock
	sem_post(shm.rd_lock);      //!< release the readers
	// fprintf(stderr, "reader counter %d.\n", shm.flags[0]);
	toc(t1, &tdif);
	fprintf(stderr, "Use %fus to flush the memory!\n", tdif);
	return 1;

}

//! read data from the share memory buffer
//! int start: starting index
//! int length: lenght of the memory segment
//! eg. if start = 2, length = 10, date is at index of [2, 3, 4, ..., 11]
int sharedMem::readData(unsigned char *rbuf, int start, int length)
{
	#ifdef SEM_DEBUG
	int val;	//!< variable for checking semaphore value in debug mode
	#endif

	struct timespec ts; 	//!< time variable


	if (shm.flags[2] == 1){
		fprintf(stderr, "The memory is nolonger exist, it may be destoryed by the creater already!\n");
		fprintf(stderr, "unlink() runs automatically!\n");
		unlink();
		exit(1);
	}

	if( (start+length) > shm.size )		//!< check the input size is valid
	{
		fprintf(stderr, "Error: ReadData size exceeded the ");
		fprintf(stderr, "share memory max size!\n");
		return -1;
	}

	semtimeout(&ts, shm.timeout_ns);
	if( sem_timedwait(shm.rd_lock, &ts) == -1 ) 		//!< reader get the read lock
	{
		fprintf(stderr,"%c%s\n", getlocaltime(), strerror(errno) );
		#ifdef SEM_DEBUG
			sem_getvalue(shm.rd_lock, &val);
			DEBUG_MSG("rd_lock timeout from read!!--%d\n", val);
		#endif
		return -1;
	}

	if ( sem_wait(shm.rdcnt_lock) == -1 )				//!< reader get the reader counter lock
	{
		fprintf(stderr,"%c%s\n", getlocaltime(), strerror(errno) );
		#ifdef SEM_DEBUG
			sem_getvalue(shm.rdcnt_lock, &val);
			DEBUG_MSG("rdcnt_lock timeout from read!!--%d\n", val);
		#endif

		sem_post(shm.rd_lock); 		//!< if get lock fail,release the lock which we got and then return
		return -1;
	}

	(shm.flags[0])++; //!< increase the reader counter
	// fprintf(stderr, "reader counter = %d.\n", shm.flags[0]);

	if (shm.flags[0] == 1)		//!< if this is the first reader
	{
		semtimeout(&ts, shm.timeout_ns);
		if (sem_timedwait(shm.wt_lock, &ts) == -1)  //!< block the writer
		{
			fprintf(stderr,"%c%s\n", getlocaltime(), strerror(errno) );
			#ifdef SEM_DEBUG
				sem_getvalue(shm.wt_lock, &val);
				DEBUG_MSG("wt_lock timeout from read--%d!!\n", val);
			#endif

			shm.flags[0] = 0;               //!< release the rd_lock, no more reader

			//! if get lock fails, release the lock we got and then return
			sem_post(shm.rdcnt_lock);
			sem_post(shm.rd_lock);

			return -1;
		}
	}

	sem_post(shm.rdcnt_lock);		//!< Release the reader counter lock
	sem_post(shm.rd_lock);			//!< Release the read lock


	// Start reading
	//! copy the value in the buffer
	int kk = 0;
	for (int i=start; i<start+length; i++){
		rbuf[kk] = shm.buf[i];
		kk++;
	}

   if ( sem_wait(shm.rdcnt_lock) == -1 )	//!< get the reader counter lock
	{
		printf("%s\n",strerror(errno) );
		#ifdef SEM_DEBUG
			sem_getvalue(shm.rdcnt_lock, &val);
			DEBUG_MSG("rdcnt_lock timeout from read!!--%d\n", val);
		#endif
		return -1;
	}

	(shm.flags[0])--;				//!< decrease the reader counter
	if (shm.flags[0] == 0)			//!< if no more reader
	{
		sem_post(shm.wt_lock); 		//!< release the writer
	}

	sem_post(shm.rdcnt_lock);		//!< release the read lock


	return 1;
}

int sharedMem::destroy(const char *filepath)
{

	if (create_link_mutex != 1) 	//!< check the access right
	{
		fprintf(stderr, "Error: You can not destroy this memory since you are not the creator! Try unlink()\n");
		return -1;
	}

	shm.flags[2] = 1;	//!< memory is going to be destroy, set the flags to inform all readers

	//! log the shm and sem content to the file path before destroy them
	char outshell[512], filedir[256];
	//! create the directory for logging
	sprintf(filedir, "mkdir -p %s/%s_log", filepath, global_name);
	system(filedir);
	//! copy the data from /dev/shm/ to the log directory
	sprintf(outshell, "cp /dev/shm/*%s* %s/%s_log/", global_name, filepath, global_name);
	system(outshell);

	//destroy the sem
	sem_close(shm.wt_lock);
	sem_close(shm.rd_lock);
	sem_close(shm.rdcnt_lock);


	sem_unlink(wt_lock_name);
	sem_unlink(rd_lock_name);
	sem_unlink(rdcnt_lock_name);


	// destroy share memory
	munmap(shm.buf, shm.size);
	shm_unlink(shm_name);
	munmap(shm.flags, SHM_FLAGS*sizeof(unsigned int));
	shm_unlink(flag_name);

	fprintf(stderr,"Finsih destroying the shared memory and the semaphore!\n");

	shm.size = 0;
	shm.timeout_ns = 500000;
	create_link_mutex = 0;

	return 1;
}

//! destroy the share memory and delete all the files under /dev/shm create by this object, only creater can do this
int sharedMem::destroy()
{
	return destroy(".");	//!< default log file in working directory
}

//! unlink the share memory and semaphore but will not delete any files in /dev/shm, only linker can do this
int sharedMem::unlink()
{
	if (create_link_mutex != 2) 	//!< check the access right
	{
		fprintf(stderr, "Error: You can not unlink this memory since you are not the linker! Try destroy()\n");
		return -1;
	}

	//! close the sem without destroy it
	sem_close(shm.wt_lock);
	sem_close(shm.rd_lock);
	sem_close(shm.rdcnt_lock);

	//! unmap the share memory buffer
	munmap(shm.buf, shm.size);
	munmap(shm.flags, SHM_FLAGS*sizeof(unsigned int));

	fprintf(stderr,"Finish unlinking the shared memory and the semaphore!\n");

	shm.size = 0;
	create_link_mutex = 0;

	return 1;
}

//! Destructor
sharedMem::~sharedMem()
{
	if (create_link_mutex != 0){
		if (create_link_mutex == 1){
			destroy();
		}

		else
			unlink();
	}

	fprintf(stderr, "Destructor performs, delete object at the end of program!\n");
}
