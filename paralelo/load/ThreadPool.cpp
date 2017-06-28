#include <random>
#include <iostream>
#include "ThreadPool.h"

bool task::operator< (const task  &c) const {
    if(level == c.level){
        return COUNT(U, W) < COUNT(c.U, c.W);
    }
    return level < c.level;
}

ThreadPool::ThreadPool(int thread_number, function<void (task)> problem) {
    this->stop = false;
    this->threadNumber = thread_number;
    this->problem = problem;
}

void ThreadPool::enqueue(task call) {
    {
        std::unique_lock<std::mutex> lock(mutex_queue);
        pq.push(make_pair(call.level, call));
    }

    this->condition.notify_one();
}

task* ThreadPool::getTask() {
    task *c = new task;
    {
        std::unique_lock<std::mutex> lock(mutex_queue);
        if(!pq.empty()) {
            pair<int, task> p = pq.top();
            pq.pop();
            *c = p.second;
        }else{
            delete c;
            c = nullptr;
        }
    }

    return c;
}


void ThreadPool::run() {
    for(int i = 0; i < this->threadNumber; i++){
        thread_handles.push_back(thread(
	        [this, i]
	        {
	            for(;;)
	            {
	                task* g = nullptr;
	                {
	                    unique_lock<mutex> lock(this->mutex_queue);
	                    this->condition.wait(lock,
	                                         [this, i] {
	                                            return this->stop || !this->pq.empty(); });
	                    if (this->stop && this->pq.empty()){
	                        return;
	                    }

		                this->busyThreads++;

	                }

	                g = this->getTask();

	                if(g){
	                    this->problem(*g);
	                    delete g;
	                }

					{
						std::unique_lock<std::mutex> lock(mutex_queue);
						this->busyThreads--;
		                if(pq.empty()){

		                    if(this->busyThreads==0) {

						        this->stop = true;
						       	this->condition.notify_all();
		                    }

		                }
					}
                }
            })
        );
    }
}

int ThreadPool::nBusy(){

    int nbusy;
    {
        unique_lock<mutex> lock(this->mutex_queue);
        nbusy = this->busyThreads;
    }

    return nbusy;
}

ThreadPool::~ThreadPool() {
    for(std::thread &worker: thread_handles){
        worker.join();
    }
}
