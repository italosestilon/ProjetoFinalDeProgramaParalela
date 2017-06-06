#ifndef KSA_THREADPOOL_H
#define KSA_THREADPOOL_H

#include <cstdlib> // EXIT_SUCCESS,..
#include <string>  // std::string,..
#include <new>     // std::nothrow,..

#include "multithread.h"
#include "taskqueue.h"

namespace ksa {

struct thread_pool // simple structure for thread pool
{

struct task { // abstract struct!!
   virtual void execute() = 0;
};

typedef unsigned int uint;

private:

   static void* routine(void*);

   thread_t* thread;
   task_queue queue;

   mutex_t mutex;
   uint count;

   uint pool_size;
   uint active;

   void plus();
   void minus();

public:

   thread_pool(uint pool_size);
   virtual ~thread_pool();

   inline uint inqueue() const { return queue.size(); }
   inline uint inexecu() const { return active; }

   inline uint size() const { return count; }

   inline void push(task* job) { // inserts a new job at the end of the queue
      queue.enqueue(job);
   }

};

}

#endif // KSA_THREADPOOL_H
