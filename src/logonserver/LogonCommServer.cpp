#include "LogonStdAfx.h"

typedef struct
{
    uint16 opcode;
    uint16 size;
}logonpacket;

LogonCommServerSocket::LogonCommServerSocket(SocketHolder& holder) : BaseSocket(holder, 2048, 2048, 2048)
{
    // do nothing
    last_ping = time(NULL);
    remaining = opcode = 0;
    my_id = 0;
    sInfoCore.AddServerSocket(this);
    removed = false;
}

LogonCommServerSocket::~LogonCommServerSocket()
{

}

void LogonCommServerSocket::Delete()
{
    // if we're registered -> de-register
    if(!removed)
    {
        if(my_id)
            sInfoCore.RemoveRealm(my_id);

        sInfoCore.RemoveServerSocket(this);
    }
    delete (LogonCommServerSocket*)this;
}

void LogonCommServerSocket::OnReceive(const u16 Size)
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

void LogonCommServerSocket::HandlePacket(WorldPacket & recvData)
{
    switch(recvData.GetOpcode())
    {
    case RCMSG_REGISTER_REALM:
        HandleRegister(recvData);
        break;

    case RCMSG_REQUEST_SESSION:
        HandleSessionRequest(recvData);
        break;

    case RCMSG_PING:
        HandlePing(recvData);
        break;

    case RCMSG_SQL_EXECUTE:
        HandleSQLExecute(recvData);
        break;

    default:
        printf("Got unknwon packet from logoncomm: %u\n", recvData.GetOpcode());
        break;
    }
}

void LogonCommServerSocket::HandleRegister(WorldPacket & recvData)
{
    Realm realm;

    recvData >> realm.Name >> realm.Address;
    recvData >> realm.Colour >> realm.Icon >> realm.TimeZone >> realm.Population;

    my_id = sInfoCore.GenerateRealmID();
    sLog.outString("Registering realm `%s` under ID %u.", realm.Name.c_str(), my_id);

    // Add to the main realm list
    sInfoCore.AddRealm(my_id, &realm);

    // Send back response packet.
    WorldPacket data(RSMSG_REALM_REGISTERED, 4);
    data << uint32(0);      // Error
    data << my_id;          // Realm ID
    data << realm.Name;
    SendPacket(&data);
}

void LogonCommServerSocket::HandleSessionRequest(WorldPacket & recvData)
{
    uint32 request_id;
    string account_name;
    recvData >> request_id;
    recvData >> account_name;

    // get sessionkey!
    uint32 error = 0;
    Account * acct = sAccountMgr.GetAccount(account_name);
    BigNumber * sessionkey = acct ? sInfoCore.GetSessionKey(acct->AccountId) : 0;
    
    if(sessionkey == 0 || acct == 0)
        error = 1;          // Unauthorized user.

    // build response packet
    WorldPacket data(RSMSG_SESSION_RESULT, 150);
    data << request_id;
    data << error;
    if(!error)
    {
        // Append account information.
        data << acct->AccountId;
        data << acct->Username;
        data << acct->GMFlags;
        data << acct->AccountFlags;
        data.append(sessionkey->AsByteArray(), 40);
    }
    
    SendPacket(&data);

    printf("Sessionkey sent for account %s, err %u\n", account_name.c_str(), error);
}

void LogonCommServerSocket::HandlePing(WorldPacket & recvData)
{
    uint32 seq;
    recvData >> seq;

    WorldPacket data(RSMSG_PONG, 4);
    data << seq;
    SendPacket(&data);
    last_ping = time(NULL);
}

void LogonCommServerSocket::SendPacket(WorldPacket * data)
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

void LogonCommServerSocket::HandleSQLExecute(WorldPacket & recvData)
{
    string Query;
    recvData >> Query;
    sLogonSQL->Execute(Query.c_str());
}
