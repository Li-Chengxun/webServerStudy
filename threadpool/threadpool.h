#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <list>
#include <cstdio>
#include <exception>
#include <pthread.h>
#include "../lock/locker.h"
#include "../CGImysql/sql_connection_pool.h"


//�̳߳����ʵ��
template<typename T>
class threadpool
{
private:
    int m_thread_number; //�̳߳������߳���
    int m_max_requestes; //���������������������������
    pthread_t pthread_t *m_threads;//�߳����飬��������߳�
    locker m_queuelocker;//�������������У�
    std::list<T *> m_workqueue; //�������
public:
    //thread_number���̳߳����̵߳�����
    //max_requests������������������ġ��ȴ���������������
    //connPool�����ݿ����ӳ�ָ��
    threadpool(int actor_model, connection_pool *connPool, int thread_number = 8, int max_request = 10000);
    ~threadpool();

    //����������в����������� 
    bool append(T *request, int state);
    bool append_p(T *request);
};

//�̳߳ش��������
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
        //ѭ���������߳�
        for(int i = 0;i < thread_number;i++) {
            //ѭ�������̣߳����������̰߳�Ҫ���������
            if (pthread_create(m_threads + i, NULL, worker, this) != 0)
            {
                delete[] m_threads;
                throw std::exception();
            }
            //���߳̽��з���󣬲��õ����Թ����߳̽��л���
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
