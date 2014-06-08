#ifndef _THREADING_STARTER_H
#define _THREADING_STARTER_H

class WOWD_SERVER_DECL ThreadBase
{
public:
    virtual ~ThreadBase() {}
    virtual void run() = 0;
};

#ifdef WIN32

#include <process.h>

static unsigned int __stdcall thread_func(void * args)
{
    ThreadBase * ptr = (ThreadBase*)args;
    ptr->run();

    // delete the thread to call the destructor, our threads remove themselves here.
    delete ptr;

    return 0;
}

// Use _beginthreadex to start the thread (MT runtime lib needed)
inline void launch_thread(ThreadBase * thread)
{
    _beginthreadex(0, 0, &thread_func, (void*)thread, 0, 0);
}

#else

static void * thread_func(void * args)
{
    ThreadBase * ptr = (ThreadBase*)args;
    ptr->run();

    // delete the thread to call the destructor, our threads remove themselves here.
    delete ptr;
}

// Use pthread_create to start the thread
inline void launch_thread(ThreadBase * thread)
{
    pthread_t t;
    pthread_create(&t, 0, thread_func, (void*)thread);
}

#endif

#endif

