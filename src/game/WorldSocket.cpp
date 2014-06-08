//////////////////////////////////////////////////
// Copyright (C) 2006 Burlex for WoWd Project
//////////////////////////////////////////////////

// Class WorldSocket - Main network code functions, handles
// reading/writing of all packets.

#include "StdAfx.h"
#include "../shared/AuthCodes.h"

#pragma pack(push, 1)
struct ClientPktHeader
{
    uint16 size;
    uint32 cmd;
};

struct ServerPktHeader
{
    uint16 size;
    uint16 cmd;
};
#pragma pack(pop)

WorldSocket::WorldSocket(SocketHolder &SH) : BaseSocket(SH, WORLDSOCKET_RECVBUF_SIZE, WORLDSOCKET_SENDBUF_SIZE,
                                                        WORLDSOCKET_SENDBUF_SIZE)
{
    mSize = mOpcode = mRemaining = 0;
    _latency = 0;
    mSession = NULL;
    mSeed = rand() % 0xFFFFFFF0 + 10;
    pAuthenticationPacket = NULL;
    mQueued = false;
    mRequestID = 0;
}


WorldSocket::~WorldSocket()
{
    /*if(mSession)
        delete mSession;                    // Closed queue socket*/
    if(mSession)
        mSession->SetSocket(0);

    if(mQueued)
        sWorld.RemoveQueuedSocket(this);    // Remove from queued sockets.
}

void WorldSocket::OnDisconnect()
{
    if(mSession)
        mSession->SetSocket(0);
}

void WorldSocket::Disconnect()
{
    if(mSession)
    {
        mSession->SetSocket(0);
        mSession = 0;
    }
    Connected = FALSE;
    //PostCompletion(IOShutdownRequest);
}

void WorldSocket::OutPacket(uint16 opcode, uint16 len, const void* data)
{
    if(opcode == 0 || !IsConnected())
        return;

    SendMutex.Acquire();

    if((SendSize + len + 4) > WORLDSOCKET_SENDBUF_SIZE)
    {
        SendMutex.Release();
        Connected = FALSE;
        return;
    }

    // Packet logger :)
    sWorldLog.LogPacket(len, opcode, (const uint8*)data, 1);

    // Encrypt the packet
    // First, create the header.
    ServerPktHeader Header;
    Header.cmd = opcode;
    Header.size = SwapBytes(len + 2);
    _crypt.EncryptSend((uint8*)&Header, 4);

    // Pass the header to our send buffer
    SendArray((const u8*)&Header, 4, TRUE);

    // Pass the rest of the packet to our send buffer (if there is any)
    if(len > 0)
    {
        if(data == 0)
        {
            uint8* data_new = new uint8[len];
            memset(data_new, 0, len);
            SendArray((const uint8*)data_new, len, TRUE);
            delete [] data_new;
        }
        else
        {
            SendArray((const uint8*)data, len, TRUE);
        }
    }

    SendMutex.Release();
}

void WorldSocket::OnConnect()
{
    sLog.outString("Connection accepted from %s.", RetreiveClientIP());
    sWorld.mAcceptedConnections++;
    OutPacket(SMSG_AUTH_CHALLENGE, 4, &mSeed);
    _latency = getMSTime();
}

void WorldSocket::Close()
{
    if(mSession)
        mSession->SetSocket(NULL);

    mSession = NULL;

    BaseSocket::Close();
}

void WorldSocket::Delete()
{
    if(!DeleteMutex.AttemptAcquire())
        return;

    Close();
    if(mRequestID != 0)
    {
        sLogonCommHandler.UnauthedSocketClose(mRequestID);
        mRequestID = 0;
    }

    // call correct destructor
    delete ((WorldSocket*)this);
}

void WorldSocket::_HandleAuthSession(WorldPacket* recvPacket)
{
    std::string account;
    uint32 unk2;
    _latency = getMSTime() - _latency;

    try
    {
        *recvPacket >> mClientBuild;
        *recvPacket >> unk2;
        *recvPacket >> account;
        *recvPacket >> mClientSeed;
    }
    catch(ByteBuffer::error &)
    {
        sLog.outDetail("Incomplete copy of AUTH_SESSION recieved.");
        return;
    }

    // Send out a request for this account.
    mRequestID = sLogonCommHandler.ClientConnected(account, this);
    
    if(mRequestID == 0xFFFFFFFF)
    {
        Disconnect();
        return;
    }

    // Set the authentication packet 
    pAuthenticationPacket = recvPacket;
}

void WorldSocket::InformationRetreiveCallback(WorldPacket & recvData, uint32 requestid)
{
#ifdef ASYNC_NET
    if(_deleted != 0) return;
#endif
    if(requestid != mRequestID)
        return;

    uint32 error;
    recvData >> error;

    // This socket no longer needs to be in the pending map.
    sLogonCommHandler.UnauthedSocketClose(mRequestID);

    if(error != 0)
    {
        // something happened wrong @ the logon server
        OutPacket(SMSG_AUTH_RESPONSE, 1, "\x0D");
        return;
    }

    // Extract account information from the packet.
    string AccountName;
    uint32 AccountID;
    string GMFlags;
    uint32 AccountFlags;
    
    recvData >> AccountID >> AccountName >> GMFlags >> AccountFlags;
    sLog.outColor(TNORMAL, " >> got information packet from logon: `%s` ID %u (request %u)", AccountName.c_str(), AccountID, mRequestID);
    sLog.outColor(TNORMAL, "\n");

    mRequestID = 0;
    // Pull the session key.
    uint8 K[40];
    recvData.read(K, 40);
    
    BigNumber BNK;
    BNK.SetBinary(K, 40);
    
    // Initialize crypto.
    _crypt.SetKey(K, 40);
    _crypt.Init();

    //checking if player is already connected
    WorldSession *session = sWorld.FindSession( AccountID );
    if( session)
    {
        // AUTH_FAILED = 0x0D
        OutPacket(SMSG_AUTH_RESPONSE, 1, "\x0D");
        session->Disconnect();
        return;
    }

    Sha1Hash sha;

    uint8 digest[20];
    pAuthenticationPacket->read(digest, 20);

    uint32 t = 0;
    sha.UpdateData(AccountName);
    sha.UpdateData((uint8 *)&t, 4);
    sha.UpdateData((uint8 *)&mClientSeed, 4);
    sha.UpdateData((uint8 *)&mSeed, 4);
    sha.UpdateBigNumbers(&BNK, NULL);
    sha.Finalize();

    if (memcmp(sha.GetDigest(), digest, 20))
    {
        // AUTH_UNKNOWN_ACCOUNT = 21
        OutPacket(SMSG_AUTH_RESPONSE, 1, "\x15");
        return;
    }

    // Allocate session
    mSession = new WorldSession(AccountID, AccountName, this);
    ASSERT(mSession);
    
    // Set session properties
    mSession->SetClientBuild(mClientBuild);
    mSession->LoadSecurity(GMFlags);
    mSession->SetAccountFlags(AccountFlags);
    mSession->m_lastPing = time(NULL);

    for(uint32 i = 0; i < 8; ++i)
        mSession->SetAccountData(i, NULL, true, 0);

    char authtext[150];
    sprintf(authtext, "> %s authenticated from %s:%u [%ums]", AccountName.c_str(), RetreiveClientIP(), RetreiveClientPort(), _latency);
    if(sScriptMgr.HasPlugins())
    {
        WorldPacket data(PSMSG_MESSAGE, 150);
        data << authtext;
        PostPluginMessage(data);
    }
    printf(authtext);
    printf("\n");

    // Check for queue.
    if( (sWorld.GetNonGmSessionCount() < sWorld.GetPlayerLimit()) || mSession->HasPermissions() ) {
        Authenticate();
    } else {
        // Queued, sucker.
        uint32 Position = sWorld.AddQueuedSocket(this);
        mQueued = true;
        sLog.outString("> %s added to queue in position %u", AccountName.c_str(), Position);

        // Send packet so we know what we're doing
        UpdateQueuePosition(Position);
    }
}

void WorldSocket::Authenticate()
{
    ASSERT(pAuthenticationPacket);
    mQueued = false;

    sWorld.AddSession(mSession);
    sWorld.AddGlobalSession(mSession);
    if(mSession->HasFlag(ACCOUNT_FLAG_XPACK_01))
        OutPacket(SMSG_AUTH_RESPONSE, 11, "\x0C\x30\x78\x00\x00\x00\x00\x00\x00\x00\x01");
    else
        OutPacket(SMSG_AUTH_RESPONSE, 11, "\x0C\x30\x78\x00\x00\x00\x00\x00\x00\x00\x00");

    if(mSession->HasFlag(ACCOUNT_FLAG_XTEND_INFO))
        sWorld.AddExtendedSession(mSession);
    if(mSession->HasPermissions())
        sWorld.gmList.insert(mSession);
    
    sAddonMgr.SendAddonInfoPacket(pAuthenticationPacket, pAuthenticationPacket->rpos(), mSession);
    mSession->_latency = _latency;

    delete pAuthenticationPacket;
    pAuthenticationPacket = 0;

    sLog.outDetail("> Account %u authentication complete, adding session.", mSession->GetAccountId());
}

void WorldSocket::UpdateQueuePosition(uint32 Position)
{
    WorldPacket QueuePacket(SMSG_AUTH_RESPONSE, 15);
    QueuePacket << uint8(0x1B) << uint8(0x2C) << uint8(0x73) << uint8(0) << uint8(0);
    QueuePacket << uint32(0) << uint8(0);
    QueuePacket << Position;
    SendPacket(&QueuePacket);
}

void WorldSocket::_HandlePing(WorldPacket* recvPacket)
{
    uint32 ping;
    if(recvPacket->size() < 4)
    {
        sLog.outString("Socket closed due to incomplete ping packet.");
        Connected = FALSE;
        return;
    }

    *recvPacket >> ping;
    if(mSession)
        mSession->m_lastPing = time(NULL);

    if(recvPacket->size() >= 8 && mSession)
    {
        *recvPacket >> _latency;
        if(!_latency)
            _latency = mSession->_latency;
        else
            mSession->_latency = _latency;
        //sLog.outDetail("Got ping packet with latency of %u and seq of %u", mSession->_latency, ping);
    }
    OutPacket(SMSG_PONG, 4, &ping);
}

void WorldLog::LogPacket(uint32 len, uint16 opcode, const uint8* data, uint8 direction)
{
    if(bEnabled)
    {
        mutex.Acquire();
        unsigned int line = 1;
        unsigned int countpos = 0;
        uint16 lenght = 0;
		unsigned int count = 0;
        if(len)
        {
            lenght = len - 1;
        }

        fprintf(fout, "{%s} Packet: (0x%04X) %s PacketSize = %u\n", (direction ? "SERVER" : "CLIENT"), opcode,
            LookupName(opcode, g_worldOpcodeNames), lenght);
        fprintf(fout, "|------------------------------------------------|----------------|\n");
        fprintf(fout, "|00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |0123456789ABCDEF|\n");
        fprintf(fout, "|------------------------------------------------|----------------|\n");

        if(lenght > 0)
        {
            fprintf(fout, "|");
            for (count = 0 ; count < lenght ; count++)
            {
                if (countpos == 16)
                {
                    countpos = 0;

                    fprintf(fout,"|");

                    for (unsigned int a = count-16; a < count;a++)
                    {
                        if ((data[a] < 32) || (data[a] > 126))
                            fprintf(fout,".");
                        else
                            fprintf(fout,"%c",data[a]);
                    }

                    fprintf(fout,"|\n");

                    line++;
                    fprintf(fout,"|");
                }

                fprintf(fout,"%02X ",data[count]);

                //FIX TO PARSE PACKETS WITH LENGHT < OR = TO 16 BYTES.
                if (count+1 == lenght && lenght <= 16)
                {
                    for (unsigned int b = countpos+1; b < 16;b++)
                        fprintf(fout,"   ");

                    fprintf(fout,"|");

                    for (unsigned int a = 0; a < lenght;a++)
                    {
                        if ((data[a] < 32) || (data[a] > 126))
                            fprintf(fout,".");
                        else
                            fprintf(fout,"%c",data[a]);
                    }

                    for (unsigned int c = count; c < 15;c++)
                    fprintf(fout," ");

                    fprintf(fout,"|\n");
                }

                //FIX TO PARSE THE LAST LINE OF THE PACKETS WHEN THE LENGHT IS > 16 AND ITS IN THE LAST LINE.
                if (count+1 == lenght && lenght > 16)
                {
                    for (unsigned int b = countpos+1; b < 16;b++)
                        fprintf(fout,"   ");

                    fprintf(fout,"|");

                    unsigned short print = 0;

                    for (unsigned int a = line * 16 - 16; a < lenght;a++)
                    {
                        if ((data[a] < 32) || (data[a] > 126))
                            fprintf(fout,".");
                        else
                            fprintf(fout,"%c",data[a]);

                        print++;
                    }

                    for (unsigned int c = print; c < 16;c++)
                    fprintf(fout," ");

                    fprintf(fout,"|\n");
                }

                countpos++;
            }
        }
        fprintf(fout, "-------------------------------------------------------------------\n\n");
        fflush(fout);
        mutex.Release();
    }
}

void WorldSocket::OnReceive(const u16 Size)
{
    if(Size)
    {
        uint32 BytesLeft = Size;

        while(TRUE)
        {
            if(BytesLeft == 0)
                return;

            // Check for the header if we don't have any bytes to wait for.
            if(mRemaining == 0)
            {
                if(!HasBytes(6))
                {
                    // No header in the packet, let's wait.
                    return;
                }

                // Copy from packet buffer into header local var
                ClientPktHeader Header;
                ReadBytes((uint8*)&Header, 6);
                
                // Decrypt the header
                _crypt.DecryptRecv((uint8*)&Header, 6);

                mRemaining = mSize = ntohs(Header.size) - 4;
                mOpcode = Header.cmd;
                BytesLeft -= 6;
            }

            WorldPacket * Packet;

            if(mRemaining > 0)
            {
                if(!HasBytes(mRemaining))
                {
                    // We have a fragmented packet. Wait for the complete one before proceeding.
                    return;
                }

                if(mSize > 100000)   // This is getting way too big after this..
                {
                    Disconnect();
                    return;
                }
            }

            Packet = new WorldPacket(mOpcode, mSize);
            Packet->resize(mSize);

            if(mRemaining > 0)
            {
                // Copy from packet buffer into our actual buffer.
                ReadBytes((u8*)Packet->contents(), mRemaining);
                BytesLeft -= mRemaining;
            }

            sWorldLog.LogPacket(mSize, mOpcode, mSize ? Packet->contents() : NULL, 0);
            mRemaining = mSize = mOpcode = 0;
            
            // Check for packets that we handle
            switch(Packet->GetOpcode())
            {
            case CMSG_PING:
                {
                    _HandlePing(Packet);
                    delete Packet;
                }break;
            case CMSG_AUTH_SESSION:
                {
                    _HandleAuthSession(Packet);
                }break;
            default:
                {
                    if(mSession) mSession->QueuePacket(Packet);
                    else delete Packet;
                }break;
            }
        }
    }
}
