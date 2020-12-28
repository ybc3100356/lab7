#include "server.h"
#include "client_list.h"
#include "../protocol/my_protocol.hpp"
#pragma warning(disable : 4996)

Server* Server::instance = nullptr;

int Server::serve()
{
    //create socket
    SOCKET servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    //bind socket
    sockaddr_in sockAddr;
    memset(&sockAddr, 0, sizeof(sockAddr));
    sockAddr.sin_family = PF_INET;
    sockAddr.sin_addr.S_un.S_addr = inet_addr("0.0.0.0");
    sockAddr.sin_port = htons(1730);
    bind(servSock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));

    listen(servSock, ClientList::MAXUSERNUM);

    // get host name
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == SOCKET_ERROR)
    {
        std::cout << "cannot get the local host name" << std::endl;
        return -1;
    }
    name = hostname;
    std::cout << "local host name:" << name << std::endl;

    //accept connection of client
    SOCKADDR clntAddr;
    int nSize = sizeof(SOCKADDR);
    HANDLE hThreadArray[ClientList::MAXUSERNUM] = { 0 };
    while (1)
    {
        SOCKET clntSock = accept(servSock, (SOCKADDR*)&clntAddr, &nSize);
        if (INVALID_SOCKET == clntSock)
        {
            printf("socket Failed\n");
            continue;
        }
        //accept success, add client to client pool and create thread to receive request 
        int clientIndex = ClientList::addClient(clntSock, clntAddr);
        if (clientIndex >= 0)
        {
            if (hThreadArray[clientIndex])
                CloseHandle(hThreadArray[clientIndex]);
            PtrToRespondData pData = new RespondData(clntSock, (size_t)clientIndex, this);
            hThreadArray[clientIndex] = CreateThread(nullptr, 0, requestReceiver, pData, 0, nullptr);
        }
        else// the client pool is full
        {
            char refuseStr[] = "Sorry, there are too many users now! please connect later.";
            MyProtocol::sendPacket(clntSock, DataPacket('q', refuseStr, strlen(refuseStr) + sizeof(char)));
            closesocket(clntSock);
        }
    }
    WaitForMultipleObjects(ClientList::MAXUSERNUM, hThreadArray, TRUE, INFINITE);
    for (size_t i = 0; i < ClientList::MAXUSERNUM; i++)
        CloseHandle(hThreadArray[i]);
    closesocket(servSock);

    instance = nullptr;
    return 0;
}

DWORD WINAPI requestReceiver(LPVOID lpParameter)
{
    PtrToRespondData pData = (PtrToRespondData)lpParameter;
    SOCKET clntSock = pData->clntSock;
    size_t index = pData->index;
    delete pData;

    //send hello message
    char welcomeStr[] = "Hello World!";
    MyProtocol::sendPacket(clntSock, DataPacket('0', welcomeStr, strlen(welcomeStr) + sizeof(char)));

    bool quit = false;
    while (!quit)
    {
        //receive request from client
        DataPacket requestPacket = MyProtocol::recvPacket(clntSock);
        std::cout << "Message from client " << index << ": type[" << requestPacket.type << "]";
        if (requestPacket.len)
            std::cout << ", content[" << requestPacket.data << "]";
        std::cout << std::endl;

        //respond to client
        quit = Server::instance->respond(requestPacket, clntSock, index);
    }

    ClientList::removeClient(index);
    closesocket(clntSock);
    return 0;
}

const std::string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
    return buf;
}

int Server::respond(DataPacket& requestPacket, SOCKET clntSock, size_t index)
{
    std::string timeStr = currentDateTime();
    std::string str;

    switch (requestPacket.type)
    {
    case    't':
        MyProtocol::sendPacket(clntSock, DataPacket('t', timeStr.c_str(), timeStr.size() + sizeof(char)));
        break;
    case    'n':
        MyProtocol::sendPacket(clntSock, DataPacket('n', getName().c_str(), getName().size() + sizeof(char)));
        break;
    case    'l':
        str = ClientList::getClientList();
        MyProtocol::sendPacket(clntSock, DataPacket('l', str.c_str(), str.size() + sizeof(char)));
        break;
    case    's':
        requestPacket.source = index;
        MyProtocol::sendPacket(ClientList::getSock(requestPacket.destiny), std::move(requestPacket));
        break;
    case    'q':
        MyProtocol::sendPacket(clntSock, DataPacket('q'));
        return 1;
    default:
        break;
    }
    return 0;
}

Server::Server()
{
    if (instance == nullptr)
        instance = this;
}

Server::~Server()
{
    instance = nullptr;
}