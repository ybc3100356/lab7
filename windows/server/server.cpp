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
            hThreadArray[clientIndex] = CreateThread(nullptr, 0, requestReceiver, &RespondData(clntSock, (size_t)clientIndex, this), 0, nullptr);
        }
        else// the client pool is full
        {
            std::string refuseStr("Sorry, there are too many users now! please connect later.");
            MyProtocol::sendMessage(clntSock, DataPacket('q', refuseStr.c_str(), refuseStr.size() + sizeof(char)), sizeof(DataPacket));
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

    //send hello message
    std::string welcomeStr("Hello World!");
    MyProtocol::sendMessage(clntSock, DataPacket('0', welcomeStr.c_str(), welcomeStr.size() + sizeof(char)), sizeof(DataPacket));

    bool quit = false;
    while (!quit)
    {
        //receive request from client
        std::unique_ptr<DataPacket> requestPacket = MyProtocol::recvMessage(clntSock);
        std::cout << "Message from client " << pData->index << ": type[" << requestPacket->type
            << "], content[" << requestPacket->data << "]" << std::endl;

        //respond to client
        quit = Server::instance->respond(*requestPacket, clntSock, pData->index);
    }

    ClientList::removeClient(pData->index);
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

    DataPacket respondPacket('r');
    switch (requestPacket.type)
    {
    case    't':
        MyProtocol::sendMessage(clntSock, DataPacket('t', timeStr.c_str(), timeStr.size() + sizeof(char)), sizeof(DataPacket));
        break;
    case    'n':
        MyProtocol::sendMessage(clntSock, DataPacket('n', getName().c_str(), getName().size() + sizeof(char)), sizeof(DataPacket));
        break;
    case    'l':
        str = ClientList::getClientList();
        MyProtocol::sendMessage(clntSock, DataPacket('l', str.c_str(), str.size() + sizeof(char)), sizeof(DataPacket));
        break;
    case    's':
        requestPacket.source = index;
        MyProtocol::sendMessage(ClientList::getSock(requestPacket.destiny), requestPacket, sizeof(DataPacket));
        break;
    case    'q':
        MyProtocol::sendMessage(clntSock, DataPacket('q'), sizeof(DataPacket));
        return 1;
    default:
        break;
    }
    return 0;
}

Server::Server(std::string _name) : name(_name)
{
    if (instance == nullptr)
        instance = this;
}

Server::~Server()
{
    instance = nullptr;
}