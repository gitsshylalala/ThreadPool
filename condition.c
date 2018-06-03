/*************************************************************************
	> File Name: condition.c
	> Author: 
	> Mail: 
	> Created Time: 2018年05月07日 星期一 16时58分44秒
 ************************************************************************/

#include "condition.h"

/*typedef struct condition {
	pthread_mutex_t pmutex;
	pthread_cond_t pcond;
} condition_t;*/

int condition_init(condition_t *cond){
    pthread_mutex_init(&cond->pmutex,NULL);
    pthread_cond_init(&cond->pcond,NULL);
}

int condition_lock(condition_t *cond){
    pthread_mutex_lock(&cond->pmutex);
}

int condition_unlock(condition_t *cond){
    pthread_mutex_unlock(&cond->pmutex);
}

int condition_wait(condition_t *cond){
    pthread_cond_wait(&cond->pcond,&cond->pmutex);//mutex to unlock
}

int condition_timedwait(condition_t *cond, const struct timespec*abstime){
    pthread_cond_timedwait(&cond->pcond,&cond->pmutex,abstime);
}

int condition_signal(condition_t *cond){
    pthread_cond_signal(&cond->pcond);
}

int condition_broadcast(condition_t *cond){
    pthread_cond_broadcast(&cond->pcond);
}

int condition_destroy(condition_t *cond){
    pthread_mutex_destroy(&cond->pmutex);
    pthread_cond_destroy(&cond->pcond);
}
