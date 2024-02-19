#ifndef LOCKER_H
#define LOCKER_H

#include <exception>
#include <pthread.h>
#include <semaphore.h>

//������Ҫ��Linux�����������з�װ��
//�����Ĵ��������ٺ�����װ����Ĺ��������������У�ʵ��RAII����
//�����������ź��������������������� 


/*

sem_init�������ڳ�ʼ��һ��δ�������ź���

sem_destory�������������ź���

sem_wait��������ԭ�Ӳ�����ʽ���ź�����һ,�ź���Ϊ0ʱ,sem_wait����

sem_post������ԭ�Ӳ�����ʽ���ź�����һ,�ź�������0ʱ,���ѵ���sem_post���߳�

*/

//�ź�����ʵ�� 
class sem
{
public:
	//���캯�� 
    sem()
    {
    	//sem_init() ��ʼ��һ����λ�� sem �������ź���
        if (sem_init(&m_sem, 0, 0) != 0)//�ڶ���������ʾ �������̿��Թ�����ź�����0��ʾ�����ڶ��̹߳���
										//������������ʾ������Դ��Ŀ�� 
        {
        	//em_init() �ɹ�ʱ���� 0������ʱ������ -1
            throw std::exception();
        }
    }
    sem(int num)
    {
    	//sem_init�������ڳ�ʼ��һ��δ�������ź���
        if (sem_init(&m_sem, 0, num) != 0)
        {
            throw std::exception();
        }
    }
    //�������� 
    ~sem()
    {
    	//sem_destory�������������ź���
        sem_destroy(&m_sem);
    }
    bool wait()
    {
		//wait���� 
        return sem_wait(&m_sem) == 0;
    }
    bool post()
    {
    	//signal���� 
        return sem_post(&m_sem) == 0;
    }

private:
    sem_t m_sem;
};

//������ 

class locker
{
public:
    locker()
    {
    	//pthread_mutex_init�������ڳ�ʼ��������
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

//��������
 
//����������ʹ�û�����Ҫ�������ʹ��
//�ڲ�����һ�μ����ͽ���
//��װ������ʹ�ø��Ӽ��
 
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
    	
    	//pthread_cond_wait�������ڵȴ�Ŀ����������.�ú�������ʱ��Ҫ���� 
		//mutex����(�����Ļ�����) ,����ִ��ʱ,�Ȱѵ����̷߳�������������
		//�������,Ȼ�󽫻�����mutex����,�������ɹ�����Ϊ0ʱ,���������ٴ�
		//������. Ҳ����˵�����ڲ�����һ�ν����ͼ�������.
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
    	//pthread_cond_broadcast�����Թ㲥�ķ�ʽ�������еȴ�Ŀ�������������߳�
        return pthread_cond_broadcast(&m_cond) == 0;
    }

private:
    //static pthread_mutex_t m_mutex;
    pthread_cond_t m_cond;
};
#endif
