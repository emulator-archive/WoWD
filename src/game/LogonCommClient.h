#ifndef __LOGON_COMM_CLIENT_H
#define __LOGON_COMM_CLIENT_H

#include "../logonserver/LogonOpcodes.h"

class LogonCommClientSocket : public BaseSocket
{
    uint16 remaining;
    uint16 opcode;
public:
    LogonCommClientSocket(SocketHolder& holder);
    ~LogonCommClientSocket();
    
    void OnReceive(const u16 Size);
    void SendPacket(WorldPacket * data);
    void HandlePacket(WorldPacket & recvData);
    void SendPing();

    void HandleRegister(WorldPacket & recvData);
    void HandlePong(WorldPacket & recvData);
    void HandleSessionInfo(WorldPacket & recvData);

    void Delete();
    uint32 last_ping;

    uint32 pingtime;
    uint32 latency;
    uint32 pingseq;
    uint32 _id;
    bool gotpong;
};

#endif

