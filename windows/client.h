#ifndef LAB7_CLIENT_H
#define LAB7_CLIENT_H

#include <WinSock2.h>

class ClientList
{
public:
    class Client
    {
        SOCKET clntSock;
        bool connected;
        SOCKADDR clntAddr;

    public:
        SOCKET &getSock() { return clntSock; }
        void setSock(SOCKET _clntSock) { clntSock = _clntSock; }

        SOCKADDR &getAddr() { return clntAddr; }
        void setAddr(SOCKADDR _clntAddr) { clntAddr = _clntAddr; }

        bool isConnected() { return connected; }
        void setConnected(bool _connected) { connected = _connected; }
        Client() : connected(false) {}
        Client(SOCKET _clntSock, SOCKADDR _clntAddr) : clntSock(_clntSock), clntAddr(_clntAddr){};
        ~Client(){};
    };

    static const size_t MAXUSERNUM = 2;
    static Client clientPool[MAXUSERNUM];

    static int addClient(SOCKET clntSock, SOCKADDR clntAddr);
    static int removeClient(size_t index);
};

#endif