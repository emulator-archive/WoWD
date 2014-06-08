#include "StdAfx.h"

typedef struct
{
    uint16 opcode;
    uint16 size;
}logonpacket;

LogonCommClientSocket::LogonCommClientSocket(SocketHolder& holder) : BaseSocket(holder, 2048, 2048, 2048)
{
    // do nothing
    last_ping = time(NULL);
    remaining = opcode = 0;
    _id=0;
    gotpong = true;
    pingseq = 0;
    latency = 0;
}

void LogonCommClientSocket::OnReceive(const u16 Size)
{
    while(true)
    {
        if(!remaining)
        {
            if(!HasBytes(4))
                return;     // no header

            // read header
            ReadBytes((u8*)&opcode, 2);
            ReadBytes((u8*)&remaining, 2);
        }

        // do we have a full packet?
        if(!HasBytes(remaining))
            return;

        // create the buffer
        WorldPacket buff(opcode, remaining);
        buff.resize(remaining);
        ReadBytes((u8*)buff.contents(), remaining);

        // handle the packet
        HandlePacket(buff);
        
        remaining = 0;
        opcode = 0;
    }
}

void LogonCommClientSocket::HandlePacket(WorldPacket & recvData)
{
    switch(recvData.GetOpcode())
    {
    case RSMSG_REALM_REGISTERED:
        HandleRegister(recvData);
        break;

    case RSMSG_SESSION_RESULT:
        HandleSessionInfo(recvData);
        break;

    case RSMSG_PONG:
        HandlePong(recvData);
        break;

    default:
        printf("Got unknwon packet from logoncomm: %u\n", recvData.GetOpcode());
        break;
    }
}

void LogonCommClientSocket::HandleRegister(WorldPacket & recvData)
{
    uint32 realmlid;
    uint32 error;
    string realmname;
    recvData >> error >> realmlid >> realmname;

    sLog.outColor(TNORMAL, "\n           >> realm `%s` registered under id ", realmname.c_str());
    sLog.outColor(TGREEN, "%u\n", realmlid);
    
    LogonCommHandler::getSingleton().AdditionAck(_id, realmlid);
}

void LogonCommClientSocket::HandleSessionInfo(WorldPacket & recvData)
{
    uint32 request_id;
    recvData >> request_id;

    // find the socket with this request
    WorldSocket * sock = sLogonCommHandler.GetSocketByRequest(request_id);
    if(sock == 0)       // Expired/Client disconnected
        return;

#ifdef ASYNC_NET
    if(sock->_deleted != 0)
        return;
#endif

    // extract sessionkey / account information (done by WS)
    sock->InformationRetreiveCallback(recvData, request_id);
}

void LogonCommClientSocket::HandlePong(WorldPacket & recvData)
{
    if(latency)
        sLog.outDebug(">> logonserver latency: %ums", getMSTime() - pingtime);
    latency = getMSTime() - pingtime;
    gotpong = true;
    //printf("PONG!\n");
}

void LogonCommClientSocket::SendPing()
{
    pingseq++;
    pingtime = getMSTime();
    WorldPacket data(RCMSG_PING, 4);
    data << pingseq;
    SendPacket(&data);
    //printf("PING...\n");

    last_ping = time(NULL);
    gotpong = false;
}

void LogonCommClientSocket::SendPacket(WorldPacket * data)
{
    SendMutex.Acquire();

    if((SendSize + data->size() + 4) > 2048)
    {
        SendMutex.Release();
        Connected = FALSE;
        return;
    }

    logonpacket header;
    header.opcode = data->GetOpcode();
    header.size   = data->size();

    SendArray((const u8*)&header, 4, TRUE);

    if(data->size() > 0)
        SendArray((const uint8*)data->contents(), data->size(), TRUE);

    SendMutex.Release();
}

void LogonCommClientSocket::Delete()
{
    if(_id != 0)
        sLogonCommHandler.ConnectionDropped(_id);    
    delete (LogonCommClientSocket*)this;
}

LogonCommClientSocket::~LogonCommClientSocket()
{

}