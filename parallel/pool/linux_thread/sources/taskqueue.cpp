#include "../headers/taskqueue.h"

namespace ksa {

void task_queue::enqueue(void* const job) // inserts a new job at the end
{
   mutex.lock(); // are you crazy??
   queue.push(job);
   ++count;
   condition.signal();
   mutex.unlock();
}

void* task_queue::dequeue() // removes the next job in the queue
{
   mutex.lock(); // yeah, you're!!
   while (!(bool)count) {
      condition.wait(mutex);
   }
   void* job = queue.front();
   queue.pop();
   --count;
   mutex.unlock();
   return job;
}

}
