#ifndef _THREADING_MUTEX_H
#define _THREADING_MUTEX_H

#ifdef WIN32
#include <windows.h>

class WOWD_SERVER_DECL Mutex
{
public:
	Mutex()
	{
		InitializeCriticalSection(&cs);
	}

	~Mutex()
	{
		DeleteCriticalSection(&cs);
	}

	void Acquire()
	{
		EnterCriticalSection(&cs);
	}

	void Release()
	{
		LeaveCriticalSection(&cs);
	}

	bool AttemptAcquire()
	{
		return TryEnterCriticalSection(&cs);
	}

protected:
	CRITICAL_SECTION cs;
};

/*******
 * this type of mutex will prevent re-entry from the same thread
 * its needed for the event system.
*/

class WOWD_SERVER_DECL RMutex
{
public:
	RMutex()
	{
		InitializeCriticalSection(&cs);
		locked = false;
	}

	~RMutex()
	{
		DeleteCriticalSection(&cs);
	}

	void Acquire()
	{
		EnterCriticalSection(&cs);
		locked = true;
	}

	void Release()
	{
		LeaveCriticalSection(&cs);
		locked = false;
	}

	bool AttemptAcquire()
	{
		if(TryEnterCriticalSection(&cs))
		{
			if(locked == false)
			{
				locked = true;
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}

protected:
	CRITICAL_SECTION cs;
	bool locked;
};

#else

/***********************************
  pthread_mutex implementation
  **********************************/

class Mutex
{
public:
    Mutex()
    {
        pthread_mutex_init(&mutex, NULL);
    }

    ~Mutex()
    {
        pthread_mutex_destroy(&mutex);
    }

    inline void Acquire()
    {
        pthread_mutex_lock(&mutex);
    }

    inline void Release()
    {
        pthread_mutex_unlock(&mutex);
    }

    inline bool AttemptAcquire()
    {
        return (pthread_mutex_trylock(&mutex) == 0);
    }

protected:
    pthread_mutex_t mutex;
};

#define RMutex Mutex

#endif

#endif

