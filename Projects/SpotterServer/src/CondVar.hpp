/*
 * CondVar.hpp
 *
 *  Created on: Dec 28, 2013
 *      Author: rob
 */

#ifndef CONDVAR_HPP_
#define CONDVAR_HPP_

#include "Mutex.hpp"

namespace fambogie {

class CondVar
{
    pthread_cond_t  cond;
    Mutex&          lock;

  public:
    // just initialize to defaults
    CondVar(Mutex& mutex) : lock(mutex) { pthread_cond_init(&cond, NULL); }
    virtual ~CondVar() { pthread_cond_destroy(&cond); }

    int wait() { return  pthread_cond_wait(&cond, &(lock.mutex)); }
    int wait(const timespec * timeout) { return  pthread_cond_timedwait(&cond, &(lock.mutex), timeout); }
    int signal() { return pthread_cond_signal(&cond); }
    int broadcast() { return pthread_cond_broadcast(&cond); }

  private:
    CondVar();
};

} /* namespace fambogie */

#endif /* CONDVAR_HPP_ */
