//
// Created by estilon on 06/06/17.
//

#include <random>
#include <iostream>
#include "ThreadPool.h"

bool task::operator< (const task  &c) const {
    if(level == c.level){
        return COUNT(U, W) < COUNT(c.U, c.W);
    }
    return level < c.level;
}

ThreadPool::ThreadPool(int thread_number, function<vector<task>(task)> problem) {
    //cout << "fila esta " << pq.empty() << endl;
    this->stop = false;
    this->threadNumber = thread_number;
    this->problem = problem;
}

void ThreadPool::enqueue(task call) {
	{
	    std::unique_lock<std::mutex> lock(mutex_queue);
	    //cout << "colocando " << call.a << " na fila" << "\n";
	    pq.push(make_pair(call.level, call));
	}

    this->condition.notify_one();
}

task* ThreadPool::getTask() {
    task *c = new task;
    //cout << "tentando tirar da fila "  <<  " " <<  endl;
    //cout << "tamanho da fila " << pq.size() << endl;
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
        busy.push_back(true);
        thread_handles.push_back(thread(
                [this, i]
                {
                    cout << "criando thread " << i << endl;
                    for(;;)
                    {
                        //cout << "thread " << i << " ficando desocupada" << endl;
                        this->busy[i] = false;
                        task* g = nullptr;
                        {
                            unique_lock<mutex> lock(this->mutex_queue);
                            //cout << "thread " << i << " ficando ocupada" << endl;
                            this->condition.wait(lock,
                                                 [this, i] { 
                                                    //cout << "avaliando a thread " << i << endl;
                                                    return this->stop || !this->pq.empty(); });
                            if (this->stop && this->pq.empty()){
                            	cout << "thread " << i << " parando" << endl;
                                return;
                            }

               
                        }

                        {
                        	unique_lock<mutex> lock(this->mutex_queue);
                        	this->busyThreads++;
                        }

                        g = this->getTask();
                        vector<task> calls;
                        if(g){
	                        calls = this->problem(*g);
	                        delete g;
	                        //cout << "t " << i << " enfileirando " << calls.size() << " elementos" << endl;
	                        for(task c : calls){
	                            this->enqueue(c);
	                        }
                    	}

                        {
                        	unique_lock<mutex> lock(this->mutex_queue);
                        	this->busyThreads--;
                    	}

                        if(calls.empty()){
                        	//cout << "possivelmente parar " << endl;
                        	std::unique_lock<std::mutex> lock(mutex_queue);
                          	//cout << "ocupadas " << this->busyThreads << " tamanho da fila " <<  pq.size() << endl;
                            if(this->busyThreads==0) {
                                if(pq.empty()){
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

ThreadPool::~ThreadPool() {
    
    for(std::thread &worker: thread_handles){
        worker.join();
    }
}
