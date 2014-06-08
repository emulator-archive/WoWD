#include "Common.h"

using namespace std;

vector<string> StrSplit(const string &src, const string &sep)
{
    vector<string> r;
    string s;
    for (string::const_iterator i = src.begin(); i != src.end(); i++) {
        if (sep.find(*i) != string::npos) {
            if (s.length()) r.push_back(s);
            s = "";
        } else {
            s += *i;
        }
    }
    if (s.length()) r.push_back(s);
    return r;
}

void SetThreadName(const char* format, ...)
{
    // This isn't supported on nix?
    va_list ap;
    va_start(ap, format);

#ifdef WIN32

    char thread_name[200];
    vsprintf(thread_name, format, ap);

    THREADNAME_INFO info;
    info.dwType = 0x1000;
    info.dwThreadID = GetCurrentThreadId();
    info.dwFlags = 0;
    info.szName = thread_name;

    __try
    {
        RaiseException(0x406D1388, 0, sizeof(info)/sizeof(DWORD), (DWORD*)&info);
    }
    __except(EXCEPTION_CONTINUE_EXECUTION)
    {

    }

#endif

    va_end(ap);
}