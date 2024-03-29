/*!
	This module defines function for ticking time

	Company: PI Electronic (H.K.) Ltd
	Author : Terence
	Date: 13 Jan 2015
*/
#include "timer.h"

struct timespec t_on ;
struct timespec t_off ;
float ts_diff = 0;
struct timespec t_loc;

//! tick current time
struct timespec tic(){
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return ts;
}

// tick cuurent time and calculate the time difference with ts
/*!
	input:
		ts: time variable
		tdif: time difference between current time and ts
*/
struct timespec toc(struct timespec ts, float *tdif){

	struct timespec te;
	clock_gettime(CLOCK_MONOTONIC, &te);
	*tdif = ((float)te.tv_sec-(float)ts.tv_sec)+((float)te.tv_nsec-(float)ts.tv_nsec)/1E9;
	*tdif = *tdif*1000000;	//!< make it to unit micro second (us)
 	//printf("Elapsed time is %.7f us!\n", *tdif);
 	return te;

}

//!	normalize the time from 1E9 ns to 1s
void tsnorm(struct timespec *ts){
	while (ts->tv_nsec >=NSEC_PER_SEC){
		ts->tv_nsec -= NSEC_PER_SEC;
		ts->tv_sec++;
	}
}

void semtimeout(struct timespec *ts,unsigned int timeout_ns){

    clock_gettime(CLOCK_REALTIME, ts);		//!< sem_timewait needs CLOCK_REALTIME can not use CLOCK_MONOTIONIC
    ts->tv_nsec+=timeout_ns;
    tsnorm(ts);

}

char getlocaltime(void){

	time_t timep;
	struct tm *p;

	time(&timep);
	p = localtime(&timep);
	fprintf(stderr,"[%d %d:%d:%d",p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec );

	return ']';
}