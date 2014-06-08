#ifndef __LOGON_COMM_SERVER_H
#define __LOGON_COMM_SERVER_H

class LogonCommServerSocket : public BaseSocket
{
    uint16 remaining;
    uint16 opcode;
public:
    LogonCommServerSocket(SocketHolder& holder);
    ~LogonCommServerSocket();

    void OnReceive(const u16 Size);
    void SendPacket(WorldPacket * data);
    void HandlePacket(WorldPacket & recvData);
    void Delete();

    void HandleRegister(WorldPacket & recvData);
    void HandlePing(WorldPacket & recvData);
    void HandleSessionRequest(WorldPacket & recvData);
    void HandleSQLExecute(WorldPacket & recvData);

    uint32 last_ping;
    uint32 my_id;
    bool removed;
};

#endif
