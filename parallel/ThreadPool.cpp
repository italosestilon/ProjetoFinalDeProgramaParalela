//
// Created by estilon on 06/06/17.
//

#include <random>
#include <iostream>
#include "ThreadPool.h"

bool task::operator< (const task  &c) const {
    if(level == c.level){
        return (-1)*COUNT(U, W) < (-1)*COUNT(c.U, c.W);
    }
    return level*(-1) < (-1)*c.level;
}

ThreadPool::ThreadPool(int thread_number, function<void (task)> problem) {
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
            
                            if (this->pq.empty()){
                            	cout << "thread " << i << " parando" << endl;
                                return;
                            }

               
                        }

                        g = this->getTask();
                        
                        if(g){
	                        this->problem(*g);
	                        delete g;
	                        //cout << "t " << i << " enfileirando " << calls.size() << " elementos" << endl;
                    	}                        
                    }
                })
          );
    }
}

ThreadPool::~ThreadPool() {
    this->condition.notify_all();
    cout << "tentando parar as threads" << endl;
    for(std::thread &worker: thread_handles){
        worker.join();
    }
}
