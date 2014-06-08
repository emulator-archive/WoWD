/************************************************************************/
/* Copyright (C) 2006 Burlex                                            */
/************************************************************************/

/* * Class BaseSocket
   * All listening sockets or client sockets in the server will be based off this class.
   * Basically acts as a wrapper to keep code constant, easier to maintain, and simply
   * looks cleaner. :)
*/

#include "BaseSocket.h"
#include "SocketHolder.h"

BaseSocket::BaseSocket(SocketHolder &SocketHolder, u32 RecvBufferSize, u32 SendBufferSize, u32 BufferSize) : _socketHolder(SocketHolder)
{
    // Nullify Pointers
    ReceiveBuffer = SendBuffer = INVALID_PTR;
    RecvSize = SendSize = 0;

    // Allocate Buffers
    if(RecvBufferSize > 0) {
        ASSERT(ReceiveBuffer = new u8[RecvBufferSize]);
    }

    if(SendBufferSize > 0) {
        ASSERT(SendBuffer = new u8[SendBufferSize]);
    }

    SocketBufferSize = BufferSize;        
    Connected = FALSE;
    State = TRUE;
    Socket = INVALID_SOCKET;

    sendBufferSize = SendBufferSize;
    receiveBufferSize = RecvBufferSize;

#ifdef ASYNC_NET
    _cp = 0;
    _writeLock = 0;
    _deleted = 0;
#endif
}

BaseSocket::~BaseSocket()
{
    // Deallocate Pointers
    if(ReceiveBuffer != INVALID_PTR)
        delete [] ReceiveBuffer;

    if(SendBuffer != INVALID_PTR)
        delete [] SendBuffer;

#ifndef ASYNC_NET
    _socketHolder.RemoveSocket(this);
#else
    _deleted = 1;
#endif
}

// Checks if the connection to the opposite peer is still active
BOOL BaseSocket::CheckConnection()
{
    if( Socket == INVALID_SOCKET )
        return FALSE;

    char CheckBuffer[1];
    int FirstTest = recv(Socket, CheckBuffer, 1, MSG_PEEK);
    
    if(FirstTest == 0)
        return FALSE;

    if(FirstTest == -1)
    {
        int LastError = GetLastSocketError;
        switch(LastError)
        {
        case WSAECONNRESET:
        case WSAENOTCONN:
        case WSAENETRESET:
        case WSAENOTSOCK:
        case WSAESHUTDOWN:
        case WSAEINVAL:
        case WSAECONNABORTED:
            return FALSE;
            break;
        }
    }

    return TRUE;    
}

// Copies a just-accepted socket into this class, and adds us to our socket holder.
BOOL BaseSocket::Create(SOCKET Socket_, sockaddr_in *ClientAddress)
{
    ASSERT(Socket_ != INVALID_SOCKET);

    // Copy client's address into our local variable.
    memcpy(&ConnectedPeer, ClientAddress, sizeof(*ClientAddress));

    // Set our socket
    Socket = Socket_;
    Connected = TRUE;

    SetOptions();   
    return TRUE;
}

// Sets options on socket.
BOOL BaseSocket::SetOptions()
{
    // Set the appropriate buffers on our socket.
    int OptionReturn = setsockopt(Socket, SOL_SOCKET, SO_SNDBUF, (const char*)&SocketBufferSize, 4);
    if(OptionReturn == 0) setsockopt(Socket, SOL_SOCKET, SO_RCVBUF, (const char*)&SocketBufferSize, 4);

    u32 Val = 0x1;

    // Turn off internal buffering algorithms.
    if(OptionReturn == 0) OptionReturn = setsockopt(Socket, 0x6, TCP_NODELAY, (const char*)&Val, 4);

#ifndef ASYNC_NET
    // Set non blocking mode on the socket.
    if(OptionReturn == 0) OptionReturn = ioctlsocket(Socket, FIONBIO, &Val);
#endif

    if(OptionReturn != 0)
    {
        printf("[BaseSocket] One or more errors occured while attempting to configure options on socket %u.\n", Socket);
        return FALSE;
    }

#ifndef ASYNC_NET
    // Call virtual onconnect before adding to socket holder, in case we're destroyed by another thread :p
    OnConnect();
    _socketHolder.AddSocket(this);
#endif
    return TRUE;
}

BOOL BaseSocket::Connect(const char* RemoteAddress, u16 Port)
{
    // WSAStartup SHOULD already be called!

    struct hostent* ClientInfo = gethostbyname(RemoteAddress);
    if(!ClientInfo)
    {
        throw "Could not resolve client address.";
        return FALSE;
    }

#ifdef WIN32
    Socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
#else
    Socket = socket(AF_INET, SOCK_STREAM, 0);
#endif
    if(Socket == INVALID_SOCKET)
    {
        throw "Socket could not be created.";
        return FALSE;
    }

    struct sockaddr_in ClientAddress;
    ClientAddress.sin_family = ClientInfo->h_addrtype;
    memcpy(&ClientAddress.sin_addr.s_addr, ClientInfo->h_addr_list[0], ClientInfo->h_length);
    ClientAddress.sin_port = ntohs(Port);

    // Turn on blocking
    u32 block = 0;
    int ret = ioctlsocket(Socket, FIONBIO, &block);
    // Attempt to connect.
    int Valid = connect(Socket, (const sockaddr*)&ClientAddress, sizeof(ClientAddress));
    memcpy(&ConnectedPeer, &ClientAddress, sizeof(ClientAddress));

    if(Valid == -1)
    {
        int err = WSAGetLastError();
        throw "Connection failed to remote host.";
        Close();
        return FALSE;
    }

    Connected = TRUE;
    SetOptions();

#ifdef ASYNC_NET
    if (_cp)
    {
        if (CreateIoCompletionPort((HANDLE)Socket, _cp, (DWORD)this, 0) == NULL)
        {
                printf("Could not assign socket to completion port\n");
                Close();
                return FALSE;
        }

        PostCompletion(IOAccept); 
    }
#else
    OnConnect();
#endif
    return TRUE;
}

void BaseSocket::Close()
{
    if(CheckConnection())
    {
        // Disconnect socket.
        shutdown(Socket, SD_BOTH);
    }
    closesocket(Socket);
    Socket = -1;
}

void BaseSocket::Delete()
{
    if(!DeleteMutex.AttemptAcquire())
        return;

    if(Socket != INVALID_SOCKET)
    {
        Close();
    }

    // Suicide xD
    delete this;
}

bool BaseSocket::UpdateRead()
{
    // First we will handle receiving.
#ifndef ASYNC_NET
    State = CheckConnection();

    if(Socket == INVALID_SOCKET || !Connected || !State)
    {
        Delete();
        return false;
    }

    u32 ReadLength = 0;
    bool ret = ioctlsocket(Socket, FIONREAD, &ReadLength);

    if(ret)
        ReadLength = 0;

    if(ReadLength)
    {

        if(RecvSize + ReadLength > receiveBufferSize)
        {
            Disconnect();
            return false;
        }

#endif

        // Woohoo! We got data!
        RecvMutex.Acquire();

#ifndef ASYNC_NET
        int RLength = recv(Socket, (char*)ReceiveBuffer + RecvSize, ReadLength, 0);

        if(RLength == SOCKET_ERROR)
        {
            RecvMutex.Release();
            Disconnect();
            return false;
        }

        RecvSize += RLength;
        OnReceive(RLength);
#else
        uint32 RLength = 0;
        uint32 flags = 0;
        //flags = MSG_PARTIAL; // Not too sure about that one...should try it out

        WSABUF buf;
        buf.len = this->receiveBufferSize - RecvSize;
        buf.buf = (char *)(ReceiveBuffer + RecvSize);

        OverLapped *ol = new OverLapped(IOReadCompleted);
        memset(ol, 0, sizeof(OVERLAPPED));

        int r = WSARecv(Socket, &buf, 1, &RLength, &flags, &ol->m_ol, NULL);
        
        if(r == SOCKET_ERROR)
        {
            int err = WSAGetLastError();
            if (err != WSA_IO_PENDING)
            {
                RecvMutex.Release();
                Disconnect();
                PostCompletion(IOShutdownRequest);
                return false;
            }
        }
#endif

        RecvMutex.Release();
#ifndef ASYNC_NET
    }
#endif

    return true;
}

bool BaseSocket::UpdateWrite()
{
    // First we will handle receiving.
#ifndef ASYNC_NET
    State = CheckConnection();

    if(!State || Socket == INVALID_SOCKET || !Connected)
    {
        Delete();
        return false;
    }

#endif

    if(SendSize)
    {
        // We got outgoing data pending.
        SendMutex.Acquire();

#ifndef ASYNC_NET
        // Attempt to push all of our stored data outwards.
        int WriteLength = send(Socket, (const char*)SendBuffer, SendSize, 0);

        if(WriteLength > 0)
            EraseSendBytes(WriteLength);
#else
        uint32 WLength = 0;
        uint32 flags = 0;
        //flags = MSG_PARTIAL; // Not too sure about that one...should try it out

        WSABUF buf;
        buf.len = SendSize;
        buf.buf = (char *)SendBuffer;

        OverLapped *ol = new OverLapped(IOWriteCompleted);
        memset(ol, 0, sizeof(OVERLAPPED));

        int r = WSASend(Socket, &buf, 1, &WLength, flags, &ol->m_ol, NULL);
        
        if(r == SOCKET_ERROR)
        {
            int err = WSAGetLastError();
            if (err != WSA_IO_PENDING)
            {
                SendMutex.Release();
                DecWriteLock();
                Disconnect();
                PostCompletion(IOShutdownRequest);
                return false;
            }
        }
#endif

        SendMutex.Release();

    }

#ifdef ASYNC_NET

    else
    {
        PostCompletion(IOWriteCompleted);
    }

#endif
    return true;
}

void BaseSocket::EraseReceiveBytes(u32 Size)
{
    ASSERT(RecvSize >= Size);
    RecvSize -= Size;

    ASSERT(RecvSize + Size <= receiveBufferSize);
    if(RecvSize > 0)
        memmove(ReceiveBuffer, ReceiveBuffer + Size, RecvSize);
}

void BaseSocket::EraseSendBytes(u32 Size)
{
    ASSERT(SendSize >= Size);

#ifdef ASYNC_NET
    SendMutex.Acquire();
#endif

    SendSize -= Size;

    ASSERT(SendSize + Size <= sendBufferSize);
    if(SendSize > 0)
        memmove(SendBuffer, SendBuffer + Size, SendSize);

#ifdef ASYNC_NET
    SendMutex.Release();
#endif

}

BOOL BaseSocket::SendArray(const u8* DataArray, const u16 Size, BOOL ManualLock)
{
#ifdef ASYNC_NET
    if(_deleted) return FALSE;
#endif

    if(ManualLock == FALSE)
        SendMutex.Acquire();

    ASSERT(SendSize < sendBufferSize);
    ASSERT(Size + SendSize <= sendBufferSize);

    memcpy(&SendBuffer[SendSize], DataArray, Size);
    SendSize += Size;

    if(ManualLock == FALSE)
        SendMutex.Release();

#ifdef ASYNC_NET

    if (!_writeLock)
    {
        IncWriteLock();
        PostCompletion(IOWriteRequest);
    }

#endif

    return TRUE;
}

const char* BaseSocket::RetreiveClientIP()
{
    return (const char*)inet_ntoa(ConnectedPeer.sin_addr);
}

u16 BaseSocket::RetreiveClientPort()
{
    return SwapBytes(ConnectedPeer.sin_port);
}

void BaseSocket::OnDisconnect()
{

}

#ifdef ASYNC_NET

bool BaseSocket::PostCompletion(IOType type)
{
    if (_deleted != 0)
        return false;

    _postMutex.Acquire();
    if (type == IOShutdownRequest)
    {
        _deleted = 1;
        OnDisconnect();
    }

    OverLapped *ol = new OverLapped(type);
    memset(ol, 0, sizeof(OVERLAPPED));
    if (!PostQueuedCompletionStatus(_cp, 0, (DWORD)this, &ol->m_ol))  
    {
        _postMutex.Release();
        delete ol;
        return false;
    }
    else
    {
        _postMutex.Release();
        return true;
    }
}

void BaseSocket::PostDeletionCompletion()
{
    // Cancel any pending I/O operations.
    CancelIo((HANDLE)Socket);

    // Push for deletion in 10 seconds.
    sDeadSocketCollector.QueueSocketDeletion(this);
}

#endif
