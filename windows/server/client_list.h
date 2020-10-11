#ifndef LAB7_CLIENT_LIST_H
#define LAB7_CLIENT_LIST_H

#include <WinSock2.h>
#include <string>

class ClientList
{
    class Client
    {
        SOCKET clntSock;
        bool connected;
        SOCKADDR clntAddr;

    public:
        SOCKET getSock() { return clntSock; }
        void setSock(SOCKET _clntSock) { clntSock = _clntSock; }

        SOCKADDR& getAddr() { return clntAddr; }
        void setAddr(SOCKADDR _clntAddr) { clntAddr = _clntAddr; }

        bool isConnected() { return connected; }
        void setConnected(bool _connected) { connected = _connected; }
        Client() : connected(false) {}
        Client(SOCKET _clntSock, SOCKADDR _clntAddr) : clntSock(_clntSock), clntAddr(_clntAddr), connected(false) {};
        ~Client() {};
    };

public:
    static const size_t MAXUSERNUM = 2;
private:
    static Client clientPool[MAXUSERNUM];

public:
    static int addClient(SOCKET clntSock, SOCKADDR clntAddr);
    static int removeClient(size_t index);

};

#endif