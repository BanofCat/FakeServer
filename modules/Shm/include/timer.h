/*!
	This module defines function for ticking time

	Company: PI Electronics (H.K.) Ltd
	Author : Terence
	Date: 20 Oct 2016
*/
#ifndef __TIMER_H__
#define __TIMER_H__

#define TIMER_VERSION 20161020

#include "stdio.h"
#include "stdlib.h"
#include <time.h>

extern struct timespec t_on;
extern struct timespec t_off;
extern float ts_diff;
extern struct timespec t_loc;

#define NSEC_PER_SEC 1000000000

//! tick current time
struct timespec tic();
// tick cuurent time and calculate the time difference with ts
/*!
	input:
		ts: time variable
		tdif: time difference between current time and ts
*/
struct timespec toc(struct timespec ts, float *tdif);

void tsnorm(struct timespec *ts);

void semtimeout(struct timespec *ts, unsigned int timeout_ns);

char getlocaltime(void);

#endif
