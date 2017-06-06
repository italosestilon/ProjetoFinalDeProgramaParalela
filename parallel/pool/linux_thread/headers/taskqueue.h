#ifndef KSA_TASK_QUEUE_H
#define KSA_TASK_QUEUE_H

#include <queue> // std::queue,..

#include "multithread.h"

namespace ksa {

struct task_queue // simple structure for task queue
{

typedef unsigned int uint;

private:

   std::queue<void*> queue;
   condition_t condition;
   mutex_t mutex;
   uint count;

public:

   task_queue() { count = 0; } // initializes..
   virtual ~task_queue() {}

   inline uint size() const { return count; }

   void enqueue(void* const job);
   void* dequeue();

};

}

#endif // KSA_TASK_QUEUE_H
