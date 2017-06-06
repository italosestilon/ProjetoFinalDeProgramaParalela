#ifndef KSA_MULTITHREAD_H
#define KSA_MULTITHREAD_H

#include <pthread.h> // pthread_t, pthread_cond_t, pthread_mutex_t,..

namespace ksa {

struct mutex_t // simple structure for muteces
{

/* DESCRIPTION *****************************************************************
 *
 * A mutex is a MUTual EXclusion device, and is useful for protecting shared
 * data structures from concurrent modifications, and implementing critical
 * sections and monitors.
 *
 * A mutex has two possible states: unlocked (not owned by any thread), and
 * locked (owned by one thread). A mutex can never be owned by two different
 * threads simultaneously. A thread attempting to lock a mutex that is already
 * locked by another thread is suspended until the owning thread unlocks the
 * mutex first.
 *
 * ************************************************************************** */

friend struct condition_t;

private:

   pthread_mutex_t mutual_exclusion;

public:

   mutex_t() { ::pthread_mutex_init(&mutual_exclusion, NULL); } // initializes..
   virtual ~mutex_t() { ::pthread_mutex_destroy(&mutual_exclusion); }

   inline int trylock() { // does not block if the mutex is already locked ..
      return ::pthread_mutex_trylock(&mutual_exclusion); // by another thread
   }

   inline int lock() { // locks the mutex
      return ::pthread_mutex_lock(&mutual_exclusion);
   }

   inline int unlock() { // unlocks the given mutex
      return ::pthread_mutex_unlock(&mutual_exclusion);
   }

};

struct condition_t // simple structure for condition variable
{

/* DESCRIPTION *****************************************************************
 *
 * A condition (short for "condition variable") is a synchronization device that
 * allows threads to suspend execution and relinquish the processors until some
 * predicate on shared data is satisfied. The basic operations on conditions
 * are: signal the condition (when the predicate becomes true), and wait for the
 * condition, suspending the thread execution until another thread signals the
 * condition.
 *
 * A condition variable must always be associated with a mutex, to avoid the
 * race condition where a thread prepares to wait on a condition variable and
 * another thread signals the condition just before the first thread actually
 * waits on it.
 *
 * ************************************************************************** */

private:

   pthread_cond_t condition;

public:

   condition_t() { ::pthread_cond_init(&condition, NULL); } // initializes..
   virtual ~condition_t() { ::pthread_cond_destroy(&condition); }

   inline int wait(mutex_t& mutex) { // atomically unlocks and waits
      return ::pthread_cond_wait(&condition, &mutex.mutual_exclusion);
   }

   inline int signal() { // restarts one of the threads that are waiting..
      return ::pthread_cond_signal(&condition); // on the condition variable
   }

   inline int broadcast() { // restarts all the threads that are waiting..
      return ::pthread_cond_broadcast(&condition); // on the condition variable
   }

};

struct thread_t // simple structure for threads
{

typedef void *(*function)(void*);

private:

   pthread_t thread;

public:

   inline int create(function routine, void* param) { // starts a new thread
      return ::pthread_create(&thread, NULL, routine, param);
   }

   int id; //

   inline int detach() { // marks the thread identified by thread as detached
      return ::pthread_detach(thread);
   }

   //inline int join() { // suspends execution of the calling thread until..
   //   return ::pthread_join(thread, NULL); // the target thread terminates
   //}

};

}

#endif // KSA_MULTITHREAD_H
