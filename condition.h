/*************************************************************************
	> File Name: condition.h
	> Author: 
	> Mail: 
	> Created Time: Mon 07 May 2018 04:58:08 PM CST
 ************************************************************************/

#ifndef _CONDITION_H
#define _CONDITION_H
#include <pthread.h>
#include <unistd.h>
#include <time.h> 
#include <errno.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct condition {
	pthread_mutex_t pmutex;
	pthread_cond_t pcond;
} condition_t;

int condition_init(condition_t *cond);
int condition_lock(condition_t *cond);
int condition_unlock(condition_t *cond);
int condition_wait(condition_t *cond);
int condition_timedwait(condition_t *cond, const struct timespec*abstime);
int condition_signal(condition_t *cond);
int condition_boardcast(condition_t *cond);
int condition_destroy(condition_t *cond);
#endif
