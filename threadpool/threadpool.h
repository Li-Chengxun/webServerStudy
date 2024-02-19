#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <list>
#include <cstdio>
#include <exception>
#include <pthread.h>
#include "../lock/locker.h"
#include "../CGImysql/sql_connection_pool.h"


//线程池类的实现
template<typename T>
class threadpool
{
private:
    int m_thread_number; //线程池最大的线程数
    int m_max_requestes; //请求队列最大数量，即最大请求数
    pthread_t pthread_t *m_threads;//线程数组，用来存放线程
    locker m_queuelocker;//互斥访问请求队列；
    std::list<T *> m_workqueue; //请求队列
public:
    //thread_number是线程池中线程的数量
    //max_requests是请求队列中最多允许的、等待处理的请求的数量
    //connPool是数据库连接池指针
    threadpool(int actor_model, connection_pool *connPool, int thread_number = 8, int max_request = 10000);
    ~threadpool();

    //向请求队列中插入任务请求 
    bool append(T *request, int state);
    bool append_p(T *request);
};

//线程池创建与回收
template <typename T>
threadpool<T>::threadpool( int actor_model, connection_pool *connPool, int thread_number, int max_requests) : m_actor_model(actor_model),m_thread_number(thread_number), m_max_requests(max_requests), m_threads(NULL),m_connPool(connPool)
{
	if(thread_number <= 0 || max_requestes <= 0) {
        throw std::exception();
    }

    m_threads = new pthread_t[m_thread_number];

    if(!m_threads) {
        throw std:expection();
    }else {
        //循环创建主线程
        for(int i = 0;i < thread_number;i++) {
            //循环创建线程，并将工作线程按要求进行运行
            if (pthread_create(m_threads + i, NULL, worker, this) != 0)
            {
                delete[] m_threads;
                throw std::exception();
            }
            //将线程进行分离后，不用单独对工作线程进行回收
            if (pthread_detach(m_threads[i]))
            {
                delete[] m_threads;
                throw std::exception();
            }
        }
    }
    
}

template <typename T>
threadpool<T>::~threadpool()
{
    delete[] m_threads;
}
