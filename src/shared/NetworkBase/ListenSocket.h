/************************************************************************/
/* Copyright (C) 2006 Burlex                                            */
/************************************************************************/

/* *
   * Class ListenSocket
   * This class handles the listening/accepting of connections to the server.
   * It is only needed in a server socket, not in a client. It will request
   * Holders from the load balancer and add the sockets to the holders as
   * connections are accepted.
*/

#ifndef _LISTENSOCKET_H
#define _LISTENSOCKET_H

#include "SocketDefines.h"
#include "SocketHolder.h"

#include <string>

template<class T>
class ListenSocket
{
public:
    ListenSocket(SocketLoadBalancer& Load_Balancer) : LoadBalancer(Load_Balancer) { }
    ~ListenSocket()
    {
        closesocket(Socket);
    }

    BOOL CreateListenSocket(std::string host, u16 port)
    {
        Socket = socket(AF_INET, SOCK_STREAM, 0);
        if(Socket == INVALID_SOCKET)
        {
            printf("[ListenSocket]: Failed to create socket.\n");
            return FALSE;
        }

        // Create listen address
        ServerAddress.sin_family = AF_INET;
        ServerAddress.sin_addr.s_addr = htonl(INADDR_ANY);
        if(!(host == "0.0.0.0" || host.length() < 7))
        {
            struct hostent * hostname = gethostbyname(host.c_str());
            if(hostname == NULL)
            {
                printf("[ListenSocket]: Could not translate host for: %s\n", host.c_str());                
            } else {
                memcpy(&ServerAddress.sin_addr.s_addr, hostname->h_addr_list[0], hostname->h_length);
            }
        }

        ServerAddress.sin_port = SwapBytes(port);
        // Attempt to bind to this address
        int Res = bind(Socket, (const sockaddr*)&ServerAddress, sizeof(ServerAddress));

        if(Res != 0) {
            printf("[ListenSocket]: Could not bind to the specified interface/port. (%s:%u)\n",
                inet_ntoa(ServerAddress.sin_addr), port);
            int err = GetLastSocketError;
            return FALSE;
        }

        Res = listen(Socket, 5);
        if(Res != 0) {
            printf("[ListenSocket]: Could not listen on port %u\n", port);
            int err = GetLastSocketError;
            return FALSE;
        }

        u32 Option = 1;
        if(setsockopt(Socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&Option, 4))
            printf("[ListenSocket]: Could not set SO_REUSERADDR.\n");

        if(ioctlsocket(Socket, FIONBIO, &Option) == SOCKET_ERROR)
            printf("[ListenSocket]: Could not disable blocking mode on accept socket.\n");

        //printf("[BaseSocket] Listening on %s:%u\n", inet_ntoa(ServerAddress.sin_addr), port);
        //printf("[Listen Socket] Awaiting connections on port %u\n\n", port);
        ClientAddressSize = sizeof(ClientAddress);
        return TRUE;
    }
    void Update()
    {
        Accept_Socket = accept(Socket, (struct sockaddr*)&ClientAddress, (socklen_t*)&ClientAddressSize);
        if(Accept_Socket != INVALID_SOCKET)
        {
            // Woot, we got a connection :)
            SocketHolder & Holder = LoadBalancer.DynamicLoadBalance();
            T *Socket = new T(Holder);
            Socket->Create(Accept_Socket, &ClientAddress);
        } else {
            LastError = GetLastSocketError;
            if(LastError != WSAEWOULDBLOCK)
            {
                printf("[ListenSocket] Encountered error %u.\n", LastError);
            }
        }
    }
    void CloseListener()
    {
        shutdown(Socket, SD_BOTH);
        closesocket(Socket);
    }

    inline SOCKET GetSocket() { return Socket; }

protected:
    SOCKET Socket;
    SOCKET Accept_Socket;
    SocketLoadBalancer &LoadBalancer;
    struct sockaddr_in ServerAddress;
    struct sockaddr_in ClientAddress;
    int ClientAddressSize;
    int LastError;
};

//class ListenSocketThread : public WowdThread

void InitializeWinsock();
void CleanupWinsock();

#endif

