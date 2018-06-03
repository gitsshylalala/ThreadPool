/*************************************************************************
	> File Name: threadpool.c
	> Author: 
	> Mail: 
	> Created Time: 2018年05月07日 星期一 17时00分09秒
 ************************************************************************/

#include "threadpool.h"

void *myroute(void *arg)
{
    //用一个指针去接收线程池指针
    threadpool_t *mypool = (threadpool_t*)arg;
    int timeout = 0;//带上超时机制

    //需要个死循环来等信号到来
    while(1){
        condition_lock(&mypool->cond);

        timeout = 0;
        mypool->idle++;
        //将wait放入一个循环防止假唤醒
        while(mypool->first == NULL && mypool->quit == 0) //无任务且在运行
        {
            struct timespec ts;
            clock_gettime(CLOCK_REALTIME,&ts);//取出当前时间放入结构体
            ts.tv_sec += 2;//绝对时间加两秒
            int r = condition_timedwait(&mypool->cond,&ts);//线程出错时返回值体现
            if(r = ETIMEDOUT) //出错
            {           
                timeout = 1;
                break;
            }
        }
            
        mypool->idle--;

        if(mypool->first != NULL)
        {
            task_t *task = mypool->first;
            mypool -> first = task -> next;
            //防止线程执行任务太久，先解锁
            condition_unlock(&mypool->cond);
            (task->pfun)(task->arg);
            condition_lock(&mypool->cond);
            //malloc勿忘free
            free(task);
        }

        //要保证任务为空。
        if(mypool -> first == NULL && timeout == 1)
        {
            condition_unlock(&mypool->cond);
            printf("%#x thread timeout\n",pthread_self());
            break;
        }

        //要保证任务为空。
        //退出循环时一定要解锁
        //退出处理
        if(mypool -> first == NULL && mypool->quit == 1) //退出了
        {
            printf("%#x thread quit\n",pthread_self());
            mypool->counter--;
            if(mypool->counter == 0)
                condition_signal(&mypool->cond);
            condition_unlock(&mypool->cond);
            break;
        }

        condition_unlock(&mypool->cond);
    }
}

// 初始化
void threadpool_init(threadpool_t *pool, int max)
{
    assert(pool);

    //cond
    condition_init(&pool->cond);
    //task's first and tail
    pool->first      = NULL;
    pool->tail       = NULL;
    //max
    pool->max_thread = max;
    //idle
    pool->idle       = 0;
    //counter
    pool->counter    = 0;
    pool->quit       = 0;
}

// 往线程池添加任务
void threadpool_add(threadpool_t *pool, void*(*pf)(void*),void *arg)
{
    assert(pool);

    // 生成任务节点
    task_t *Onetask = (task_t*)malloc(sizeof(task_t));
    Onetask->pfun = pf;
    Onetask->arg  = arg;
    Onetask->next = NULL;

    // 放入任务队列
    //放入时可能会因为别的线程运行影响主线程，所以上锁
    condition_lock(&pool->cond);

    if(pool->first == NULL)
        pool->first = Onetask;
    else
        pool->tail->next = Onetask;
    pool->tail = Onetask;

    //情况1 有空闲节点
    if(pool->idle > 0)
        condition_signal(&pool->cond);
    //情况2 线程未达到上限
    else if (pool -> counter < pool -> max_thread)
    {
        pthread_t tid;
        pthread_create(&tid,NULL,myroute,(void*)pool);
        pool->counter++;
    }
    //情况3 没有空闲节点且达到上限，那等下次再在执行任务。

    condition_unlock(&pool->cond);
}

// 销毁线程池
void threadpool_destroy(threadpool_t *pool)
{
    if(pool->quit) // 已退出
        return;

    condition_lock(&pool->cond);

    pool->quit = 1;//置1表示退出
    //销毁时要广播
    //如果无线程则无销毁必要
    if(pool->counter > 0){
        //如果无空闲线程无广播必要
        if(pool->idle > 0)
            pthread_cond_broadcast(&(pool->cond.pcond));
    }

    while(pool->counter > 0)
        condition_wait(&pool->cond);

    condition_unlock(&pool->cond);
    condition_destroy(&pool->cond);//最后记得销毁
}
