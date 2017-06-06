#include "../headers/threadpool.h"

namespace ksa {

thread_pool::thread_pool(uint size) // constructing it..
{
   thread = new (std::nothrow) thread_t[size];

   if (!(bool)thread) { // null pointer
      throw std::string("Thread Pool - Failed to allocate memory!!");
   }

   pool_size = size; // \(°-º)/ oOoOoOoo
   active = 0;
   count = 0;

   for (uint i = 0; i < size; ++i) { // creating it..
      if (!(bool)thread[i].create(thread_pool::routine, this)) {
         thread[i].id = i;
         ++count;
      }
      else { // failed!!
         thread[i].id = -1;
      }
   }
}

thread_pool::~thread_pool() // destroying it..
{
   if ((bool)thread) { // it has been allocated memory space

      for (uint i = 0; i < pool_size; ++i) {
         if (thread[i].id != -1) {
            thread[i].detach();
         }
      };
      delete[] thread;
   }
}

void thread_pool::plus() // pokemon plusle
{
   mutex.lock();
   ++active;
   mutex.unlock();
}

void thread_pool::minus() // pokemon minun
{
   mutex.lock();
   --active;
   mutex.unlock();
}

void* thread_pool::routine(void* who) // new thread begins execution!!
{
   thread_pool& master = *((thread_pool*)who);

   while (true) { // infinite loop?! no, this is a fallacy!!

      task* job = (task*)master.queue.dequeue();

      if((bool)job) { // you got a job!! congratulations!!
         master.plus();
         job->execute();
         master.minus();
      }
   }

   return EXIT_SUCCESS;
}

}
