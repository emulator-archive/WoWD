/************************************************************************/
/* Copyright (C) 2007 Burlex for WoWd Project                           */
/************************************************************************/
#include "../shared/CallBack.h"

template<class Type>
class PeriodicFunctionCaller : public ThreadBase
{
public:
    template<class T>
        PeriodicFunctionCaller(T * callback, void (T::*method)(), uint32 Interval)
    {
        cb = new CallbackP0<T>(callback, method);
        interval = Interval;
        running = true;
    }

    ~PeriodicFunctionCaller()
    {
        delete cb;
    }

    void run()
    {
        uint32 start = getMSTime();
        uint32 end;
        uint32 etime;
        // initial rest
        Sleep(interval);

        while(running && mrunning)
        {
            start = getMSTime();
            
            // execute the callback
            cb->execute();

            end = getMSTime();
            etime = end - start;
            if(etime < interval)
                Sleep(interval - etime);
        }
    }

    template<class T>
        void kill()
    {
        running = false;
    }

private:
    CallbackBase * cb;
    uint32 interval;
    bool running;
};

#define SpawnPeriodicCallThread(otype, ptr, method, interval) \
    launch_thread(new PeriodicFunctionCaller<otype>(ptr, method, interval));
