#ifndef LOCKER_H
#define LOCKER_H

#include <exception>
#include <pthread.h>
#include <semaphore.h>

//类中主要是Linux下三种锁进行封装，
//将锁的创建于销毁函数封装在类的构造与析构函数中，实现RAII机制
//三种锁即，信号量，互斥量，条件变量 


/*

sem_init函数用于初始化一个未命名的信号量

sem_destory函数用于销毁信号量

sem_wait函数将以原子操作方式将信号量减一,信号量为0时,sem_wait阻塞

sem_post函数以原子操作方式将信号量加一,信号量大于0时,唤醒调用sem_post的线程

*/

//信号量的实现 
class sem
{
public:
	//构造函数 
    sem()
    {
    	//sem_init() 初始化一个定位在 sem 的匿名信号量
        if (sem_init(&m_sem, 0, 0) != 0)//第二个参数表示 几个进程可以共享该信号量，0表示进程内多线程共享。
										//第三个参数表示可用资源数目。 
        {
        	//em_init() 成功时返回 0；错误时，返回 -1
            throw std::exception();
        }
    }
    sem(int num)
    {
    	//sem_init函数用于初始化一个未命名的信号量
        if (sem_init(&m_sem, 0, num) != 0)
        {
            throw std::exception();
        }
    }
    //析构函数 
    ~sem()
    {
    	//sem_destory函数用于销毁信号量
        sem_destroy(&m_sem);
    }
    bool wait()
    {
		//wait操作 
        return sem_wait(&m_sem) == 0;
    }
    bool post()
    {
    	//signal操作 
        return sem_post(&m_sem) == 0;
    }

private:
    sem_t m_sem;
};

//互斥量 

class locker
{
public:
    locker()
    {
    	//pthread_mutex_init函数用于初始化互斥锁
        if (pthread_mutex_init(&m_mutex, NULL) != 0)
        {
            throw std::exception();
        }
    }
    ~locker()
    {
        pthread_mutex_destroy(&m_mutex);
    }
    bool lock()
    {
        return pthread_mutex_lock(&m_mutex) == 0;
    }
    bool unlock()
    {
        return pthread_mutex_unlock(&m_mutex) == 0;
    }
    pthread_mutex_t *get()
    {
        return &m_mutex;
    }

private:
    pthread_mutex_t m_mutex;
};

//条件变量
 
//条件变量的使用机制需要配合锁来使用
//内部会有一次加锁和解锁
//封装起来会使得更加简洁
 
class cond
{
public:
    cond()
    {
        if (pthread_cond_init(&m_cond, NULL) != 0)
        {
            //pthread_mutex_destroy(&m_mutex);
            throw std::exception();
        }
    }
    ~cond()
    {
        pthread_cond_destroy(&m_cond);
    }
    
    bool wait(pthread_mutex_t *m_mutex)
    {
    	
    	//pthread_cond_wait函数用于等待目标条件变量.该函数调用时需要传入 
		//mutex参数(加锁的互斥锁) ,函数执行时,先把调用线程放入条件变量的
		//请求队列,然后将互斥锁mutex解锁,当函数成功返回为0时,互斥锁会再次
		//被锁上. 也就是说函数内部会有一次解锁和加锁操作.
        int ret = 0;
        //pthread_mutex_lock(&m_mutex);
        ret = pthread_cond_wait(&m_cond, m_mutex);
        //pthread_mutex_unlock(&m_mutex);
        return ret == 0;
    }
    bool timewait(pthread_mutex_t *m_mutex, struct timespec t)
    {
        int ret = 0;
        //pthread_mutex_lock(&m_mutex);
        ret = pthread_cond_timedwait(&m_cond, m_mutex, &t);
        //pthread_mutex_unlock(&m_mutex);
        return ret == 0;
    }
    bool signal()
    {
        return pthread_cond_signal(&m_cond) == 0;
    }
    bool broadcast()
    {
    	//pthread_cond_broadcast函数以广播的方式唤醒所有等待目标条件变量的线程
        return pthread_cond_broadcast(&m_cond) == 0;
    }

private:
    //static pthread_mutex_t m_mutex;
    pthread_cond_t m_cond;
};
#endif
