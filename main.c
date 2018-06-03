/*************************************************************************
	> File Name: main.c
	> Author: 
	> Mail: 
	> Created Time: Mon 07 May 2018 06:17:31 PM CST
 ************************************************************************/

#include "threadpool.h"

void *route(void *arg)
{
    int id = *(int*)arg;
    free(arg);
    printf("%#X thread ruing %d\n", pthread_self(), id);
    sleep(1);
}

int main( void )
{
    threadpool_t pool;
    threadpool_init(&pool, 3);
    int i = 0;
    for (i=0; i<10; i++) {
         int *p = malloc(sizeof(int));
         *p = i;
         threadpool_add(&pool, route, (void*)p);
    }

    threadpool_destroy(&pool);
    //destroy还未定义，所以等15秒
    return 0;
}
