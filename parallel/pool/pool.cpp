#include <iostream>
#include <cstdlib>
#include <ctime>
#include <unistd.h>

#define F(i) i/100 << (i/10)%10 << i%10 << "s"

#include "thread"

struct job : public ksa::thread_pool::task // simple structure for tests
{
   void execute() {
	  // your code!!
      sleep(10+rand()%10);
   }
};

int main() 
{
   ksa::thread_pool pool(10);
   job j;
   int i;

   srand(time(0));

   for (i = 1; i <= 60; ++i) {
      pool.push(&j);
      system("clear");
      std::cout << "------------- Pool -------- " << F(i) << std::endl;
      std::cout << "pool size: " << pool.size()    << std::endl;
      std::cout << "executing: " << pool.inexecu() << std::endl;
      std::cout << "#in queue: " << pool.inqueue() << std::endl;
      std::cout << "--------------------------------" << std::endl;
      sleep(1);
   }

   while (pool.inexecu() > 0 || pool.inqueue() > 0) {
      system("clear");
      std::cout << "------------- Pool -------- " << F(i) << std::endl;
      std::cout << "pool size: " << pool.size()    << std::endl;
      std::cout << "executing: " << pool.inexecu() << std::endl;
      std::cout << "#in queue: " << pool.inqueue() << std::endl;
      std::cout << "--------------------------------" << std::endl;
      sleep(1); ++i;
   }
   
   system("clear");
   std::cout << "------------- Pool -------- " << F(i) << std::endl;
   std::cout << "pool size: " << pool.size()    << std::endl;
   std::cout << "executing: " << pool.inexecu() << std::endl;
   std::cout << "#in queue: " << pool.inqueue() << std::endl;
   std::cout << "--------------------------------" << std::endl;

   return EXIT_SUCCESS;
   
}
