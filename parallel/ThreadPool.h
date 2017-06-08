//
// Created by estilon on 06/06/17.
//

#include <vector>
#include <queue>
#include <pthread.h>
#include <functional>
#include <thread>
#include "bitmap.h"
#include <condition_variable>

#ifndef POOLTHREAD_THREADPOOL_H
#define POOLTHREAD_THREADPOOL_H


class task{
public:
    word * U;
    int W, level, * nncnt, * set;

    bool operator<(const task &) const;
};

using namespace std;

class ThreadPool {

private:
    priority_queue < pair< int, task>, vector< pair < int, task> > > pq;
    vector<thread> thread_handles;
    condition_variable condition;
    vector<bool> busy;
    function<vector<task>(task)> problem;
    int threadNumber;

    mutex mutex_queue;

    bool stop = false;

public:

    ThreadPool(int thread_number, std::function<vector<task>(task)> problem);
    ~ThreadPool();
    void run();
    task* getTask();
    void enqueue(task call);
};


#endif //POOLTHREAD_THREADPOOL_H
