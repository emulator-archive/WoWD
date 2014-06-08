#include "../shared/Singleton.h"

extern bool mrunning;

class LogonServer;
class LogonServer : public Singleton< LogonServer >
{
public:
    void Run();
    void Stop();
    uint32 max_build;
    uint32 min_build;
private:
    bool m_stopEvent;
};
