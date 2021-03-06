/**
 * This program tests Thread and Mutex classes
 */

#include <iostream>
#include <stdlib.h>
#include "core/common.h"
#include "core/worker-pool.h"

using namespace Epyx;

Mutex *my_mutex;
int my_counter;

class MutexThread : public Thread {
protected:
    virtual void run(){
        int i;

        usleep(100000 * (1 + rand()%20));
        my_mutex->lock();
        i = ++my_counter;
        my_mutex->unlock();
        log::info << "Inc Cnt to " << i << log::endl;

        usleep(100000 * (1 + rand()%20));
        my_mutex->lock();
        i = --my_counter;
        my_mutex->unlock();
        log::info << "Dec Cnt to " << i << log::endl;
    }
};

void test_mutex() {
    const int threadNumber = 10;
    Thread *threads[threadNumber];
    my_mutex = 0;

    try {
        log::info << "Create Mutex" << log::endl;
        my_mutex = new Mutex();
        my_counter = 0;

        for (int i = 0; i < threadNumber; i++) {
            log::info << "Spawn thread " << i << log::endl;
            threads[i] = new MutexThread();
            threads[i]->setName("Mutex", i);
            threads[i]->start();
        }

        for (int i = 0; i < threadNumber; i++) {
            log::info << "Wait for thread " << i << log::endl;
            threads[i]->wait();
            delete threads[i];
            threads[i] = 0;
        }


    } catch (Exception e) {
        std::cerr << e;
    }

    if (my_mutex)
        delete my_mutex;
}

Condition *cond_condition;

class ConditionThread : public Thread {
protected:
    virtual void run(){
        cond_condition->lock();
        cond_condition->wait();
        cond_condition->unlock();
        log::info << "Woken up" << log::endl;

        cond_condition->lock();
        cond_condition->wait();
        cond_condition->unlock();
        log::info << "Woken up AGAIN" << log::endl;
    }
};

class ImpatientConditionThread : public Thread {
protected:
    virtual void run(){
        cond_condition->lock();
        cond_condition->timedWait(500);
        cond_condition->unlock();
        log::info << "I timed out" << log::endl;

        cond_condition->lock();
        cond_condition->timedWait(500);
        cond_condition->unlock();
        log::info << "I'm really too impatient" << log::endl;
    }
};

void test_cond(){
    const int threadNumber = 6;
    Thread *threads[threadNumber];

    try {
        log::info << "Create Condition" << log::endl;
        cond_condition = new Condition();

        for (int i = 0; i < threadNumber; i++) {
            log::info << "Spawn thread " << i << log::endl;
            if(i == 0) {
                threads[i] = new ImpatientConditionThread();
                threads[i]->setName("Impatient", i);
            } else {
                threads[i] = new ConditionThread();
                threads[i]->setName("Condition", i);
            }
            threads[i]->start();
        }

        sleep(2);

        log::info<<"Waking "<< threadNumber - 2 <<" threads"<< log::endl;
        for(int i=0; i < threadNumber - 2; i++){
            cond_condition->lock();
            cond_condition->notify();
            cond_condition->unlock();
        }
        sleep(1);
        log::info<<"Waking every thread"<< log::endl;
        cond_condition->lock();
        cond_condition->notifyAll();
        cond_condition->unlock();
        sleep(1);
        log::info<<"Waking the remaining threads"<< log::endl;
        cond_condition->lock();
        cond_condition->notifyAll();
        cond_condition->unlock();

        for (int i = 0; i < threadNumber; i++) {
            log::info << "[Main] Wait for thread " << i << log::endl;
            threads[i]->wait();
            delete threads[i];
            threads[i] = 0;
        }

    } catch (Exception e) {
        std::cerr << e;
    }
}

class SpammingThread : public Thread {
protected:
    virtual void run(){
        for(int i=0; i<10000; i++){
            log::info<<"Spamming!!!§ " << i << " times " << log::endl;
        }
    }
};

void stress_test_logger(){
    const int threadNumber = 6;
    Thread *threads[threadNumber];

    try {
        for (int i = 0; i < threadNumber; i++) {
            log::info << "Spawn thread " << i << log::endl;
            threads[i] = new SpammingThread();
            threads[i]->setName("Spamming", i);
        }
        for (int i = 0; i < threadNumber; i++) {
            threads[i]->start();
        }
        for (int i = 0; i < threadNumber; i++) {
            log::info << "Wait for thread " << i << log::endl;
            threads[i]->wait();
            delete threads[i];
            threads[i] = 0;
        }

    } catch (Exception e) {
        std::cerr << e;
    }
}

class WaitingWorkerPool: public WorkerPool<int> {
public:
    WaitingWorkerPool():WorkerPool<int>(true){}

    virtual void treat(int *t){
        sleep(*t);
        log::info << "Worker finished" << log::endl;
        // t is deleted by the caller
    }
};

void test_worker_pool(){
    WaitingWorkerPool p;
    p.setName("WaitingWorker");

    log::info << "First wave of sleeps: 6 posts for 6 workers" << log::endl;
    p.setNumWorkers(6);
    for(int i=0; i<6; i++){
        p.post(new int(1));
    }
    sleep(2);

    log::info << "Second wave of sleeps: 6 sleeps and ask the pool have only 3 workers" << log::endl;
    p.setNumWorkers(3);
    for(int i=0; i<6; i++){
        p.post(new int(1));
    }
    sleep(3);

    log::info << "Second wave of sleeps (continued) : 6 sleeps notice how the workers";
    log::info <<" are destroyed only after they received a message" << log::endl;
    for(int i=0; i<6; i++){
        p.post(new int(1));
    }
    sleep(3);

    log::info << "Third wave of sleeps : 6 posts after asking the pool to have 6 workers" << log::endl;
    p.setNumWorkers(6);
    for(int i=0; i<6; i++){
        p.post(new int(1));
    }
    sleep(2);
}

int main(){
    Thread::init();
    log::init(log::CONSOLE | log::LOGFILE, "Test.log");

    test_mutex();
    test_cond();
    stress_test_logger();
    test_worker_pool();

    log::flushAndQuit();
    return 0;
}
