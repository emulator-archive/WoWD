/************************************************************************/
/* Copyright (C) 2006 Burlex                                            */
/************************************************************************/

// Class WorldSocket - Main network code functions, handles
// reading/writing of all packets.

#ifndef __WORLDSOCKET_H
#define __WORLDSOCKET_H

#define WORLDSOCKET_SENDBUF_SIZE 65535
#define WORLDSOCKET_RECVBUF_SIZE 32768

class WorldPacket;
class SocketHandler;
class WorldSession;

class WorldSocket : public BaseSocket
{
public:
    WorldSocket(SocketHolder &SH);
    ~WorldSocket();

    // vs8 fix - send null on empty buffer
    inline void SendPacket(WorldPacket* packet) { if(!packet) return; OutPacket(packet->GetOpcode(), packet->size(), (packet->size() ? (const void*)packet->contents() : NULL)); }
    void __fastcall OutPacket(uint16 opcode, uint16 len, const void* data);
   
    inline uint32 GetLatency() { return _latency; }

    void Close();
    void Delete();
    void Disconnect();

    void Authenticate();
    void InformationRetreiveCallback(WorldPacket & recvData, uint32 requestid);

    void __fastcall UpdateQueuePosition(uint32 Position);

    void OnReceive(const u16 Size);
    void OnConnect();
    void OnDisconnect();
    inline void SetSession(WorldSession * session) { mSession = session; }
    
protected:
    
    void _HandleAuthSession(WorldPacket* recvPacket);
    void _HandlePing(WorldPacket* recvPacket);

private:

    uint32 mOpcode;
    uint32 mRemaining;
    uint32 mSize;
    uint32 mSeed;
    uint32 mClientSeed;
    uint32 mClientBuild;
    uint32 mRequestID;

    WorldSession *mSession;
    WorldPacket * pAuthenticationPacket;

    WowCrypt _crypt;
    uint32 _latency;
    bool mQueued;
};

inline void FastGUIDPack(ByteBuffer & buf, const uint64 & oldguid)
{
    // hehe speed freaks
    uint8 guidmask = 0;
    uint8 guidfields[8] = {0,0,0,0,0,0,0,0};
    
    int j = 1;
    uint8 * test = (uint8*)&oldguid;

    if (*test) //7*8
    {
        guidfields[j] = *test;
        guidmask |= 1;
        j++;
    }
    if (*(test+1)) //6*8
    {
        guidfields[j] = *(test+1);
        guidmask |= 2;
        j++;
    }
    if (*(test+2)) //5*8
    {
        guidfields[j] = *(test+2);
        guidmask |= 4;
        j++;
    }
    if (*(test+3)) //4*8
    {
        guidfields[j] = *(test+3);
        guidmask |= 8;
        j++;
    }
    if (*(test+4)) //3*8
    {
        guidfields[j] = *(test+4);
        guidmask |= 16;
        j++;
    }
    if (*(test+5))//2*8
    {
        guidfields[j] = *(test+5);
        guidmask |= 32;
        j++;
    }
    if (*(test+6))//1*8
    {
        guidfields[j] = *(test+6);
        guidmask |= 64;
        j++;
    }
    if (*(test+7)) //0*8
    {
        guidfields[j] = *(test+7);
        guidmask |= 128;
        j++;
    }
    guidfields[0] = guidmask;

    buf.append(guidfields,j);
}

#endif
