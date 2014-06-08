/************************************************************************/
/* Copyright (C) 2006 Burlex                                            */
/************************************************************************/

/* * Class BaseSocket
   * All listening sockets or client sockets in the server will be based off this class.
   * Basically acts as a wrapper to keep code constant, easier to maintain, and simply
   * looks cleaner. :)
*/

#ifndef __BASESOCKET_H
#define __BASESOCKET_H

#include "SocketDefines.h"
#include "Common.h"
#include "../../game/WowdThreads.h"
#include "../../game/ThreadMgr.h"

#ifdef ASYNC_NET
    #include "../AsyncNet/AsyncNet.h"
    #include "../AsyncNet/AsyncDefines.h"
#endif

class SocketHolder;

class WOWD_SERVER_DECL BaseSocket
{
public:
    BaseSocket(SocketHolder& socketHolder, u32 RecvBufferSize, u32 SendBufferSize, u32 BufferSize);
    virtual ~BaseSocket();

    const char* RetreiveClientIP(void);
    u16 RetreiveClientPort(void);
#ifdef ASYNC_NET
    inline BOOL IsConnected() { return (Connected && _deleted == 0); }
#else
    inline BOOL IsConnected() { return Connected; }
#endif
    
    BOOL Create( SOCKET Socket_, struct sockaddr_in* ClientAddress );
    BOOL Connect(const char* RemoteAddress, u16 Port);

    virtual bool UpdateRead();
    virtual bool UpdateWrite();
    virtual void Delete();

    BOOL CheckConnection();
    inline SOCKET GetSocket() { return Socket; }
    virtual void OnDisconnect();
    virtual void Disconnect()
    {
        Connected = FALSE;
        OnDisconnect();
    }

    inline void UseReadBuffer(uint32 n) { RecvSize += n; }
    inline void UseSendBuffer(uint32 n) { EraseSendBytes(n); }

    virtual void OnReceive(const u16 Size) {}
    virtual void OnConnect() {}

#ifdef ASYNC_NET
    inline void SetCP(HANDLE cp) { _cp = cp; }
    bool PostCompletion(IOType type);
    void PostDeletionCompletion();

    inline void IncWriteLock() { InterlockedIncrement(&_writeLock); }
    inline void DecWriteLock() { InterlockedDecrement(&_writeLock); }
    inline bool HasWriteLock() { return _writeLock; }
    inline BOOL HasSend() { return SendSize; }
    int _deleted;

#endif

protected:
    
    BOOL SendArray(const u8* DataArray, const u16 Size, BOOL ManualLock);
    
    
    void EraseReceiveBytes(u32 Size);
    void EraseSendBytes(u32 Size);
    
    inline BOOL HasBytes(u32 Size)
    {
        return (RecvSize >= Size ? TRUE : FALSE);
    }

    // YOU NEED TO LOCK MANUALLY! ;)
    inline void ReadBytes(u8* DestBuffer, u32 Size)
    {
        ASSERT(RecvSize >= Size && Size <= receiveBufferSize);
        memcpy(DestBuffer, ReceiveBuffer, Size);
        EraseReceiveBytes(Size);
    }

    virtual void Close();

    u8 * ReceiveBuffer;
    u8 * SendBuffer;
    uint32 receiveBufferSize;
    uint32 sendBufferSize;

    u32 SendSize;
    u32 RecvSize;

    Mutex SendMutex;
    Mutex RecvMutex;
    Mutex DeleteMutex;

    BOOL Connected;
    BOOL State;

    struct sockaddr_in ConnectedPeer;

private:
    
    BOOL SetOptions();

    SOCKET Socket;
    u32 SocketBufferSize;

    SocketHolder& _socketHolder;

#ifdef ASYNC_NET
    HANDLE _cp;
    long _writeLock;

    Mutex _postMutex;
#endif
};

#endif


