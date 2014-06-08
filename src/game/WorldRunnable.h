//
// WorldRunnable.h
//

#ifndef __WORLDRUNNABLE_H
#define __WORLDRUNNABLE_H

#include "../game/WowdThreads.h"

class WorldRunnable : public WowdThread
{
public:
    WorldRunnable();
    void run();
};

#endif
