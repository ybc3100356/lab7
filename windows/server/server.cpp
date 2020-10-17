#include "server.h"
#include "client_list.h"
#include "../protocol/my_protocol.hpp"
#pragma warning(disable : 4996)

int Server::serve()
{
    //创建套接字
    SOCKET servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    //绑定套接字
    sockaddr_in sockAddr;
    memset(&sockAddr, 0, sizeof(sockAddr));
    sockAddr.sin_family = PF_INET;
    //sockAddr.sin_addr.S_un.S_addr = inet_addr("192.168.1.159");
    //sockAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    sockAddr.sin_addr.S_un.S_addr = inet_addr("0.0.0.0");
    sockAddr.sin_port = htons(1730);
    bind(servSock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));

    //监听
    listen(servSock, ClientList::MAXUSERNUM);

    //接受客户端请求
    SOCKADDR clntAddr;
    int nSize = sizeof(SOCKADDR);
    HANDLE hThreadArray[ClientList::MAXUSERNUM];
    while (1)
    {
        SOCKET clntSock = accept(servSock, (SOCKADDR*)&clntAddr, &nSize);
        //收到客户端的连接，创建子线程处理消息
        if (INVALID_SOCKET == clntSock)
        {
            printf("socket Failed\n");
            continue;
        }
        int clientIndex = ClientList::addClient(clntSock, clntAddr);
        if (clientIndex >= 0)
        {
            PMYDATA pData = new MYDATA(clntSock, (size_t)clientIndex, this);
            hThreadArray[clientIndex] = CreateThread(NULL, 0, clientFunc, pData, 0, NULL);
        }
        else
        {
            const char* welcomeStr = "Sorry, there are too many users now! please connect later.";
            DataPacket welcomePacket('0', welcomeStr, strlen(welcomeStr) + sizeof(char));
            MyProtocol::sendMessage(clntSock, (char*)(&welcomePacket), sizeof(DataPacket));
            closesocket(clntSock);
        }
    }
    WaitForMultipleObjects(ClientList::MAXUSERNUM, hThreadArray, TRUE, INFINITE);
    for (size_t i = 0; i < ClientList::MAXUSERNUM; i++)
    {
        CloseHandle(hThreadArray[i]);
    }
    // CloseHandle(hThreadArray[i]);
    closesocket(servSock);
    return 0;
}

DWORD WINAPI clientFunc(LPVOID lpParameter)
{
    PMYDATA pData = (PMYDATA)lpParameter;
    SOCKET clntSock = pData->clntSock;
    Server* server = pData->server;

    //向客户端发送数据
    const char* welcomeStr = "Hello World!";
    DataPacket welcomePacket('0', welcomeStr, strlen(welcomeStr) + sizeof(char));
    MyProtocol::sendMessage(clntSock, (char*)(&welcomePacket), sizeof(DataPacket));

    while (1)
    {
        //TODO: quit when client quit without informing server

        //接受客户端数据
        DataPacket* requestPacket = (DataPacket*)MyProtocol::recvMessage(clntSock);

        //输出收到的数据
        std::cout << "Message from client: type[" << requestPacket->type
            << "], content[" << requestPacket->data << "]" << std::endl;

        //根据请求作出回应
        if (server->respond(requestPacket, clntSock, pData->index) == 1)
            break;//return value == 1: quit
        delete (char*)requestPacket;
    }

    ClientList::removeClient(pData->index);
    closesocket(clntSock);
    return 0;
}

int Server::respond(DataPacket* requestPacket, SOCKET clntSock, size_t index)
{
    time_t _tm = time(NULL);
    struct tm* curtime = localtime(&_tm);
    const char* timeStr = asctime(curtime);
    std::string str;

    DataPacket respondPacket('r');
    switch (requestPacket->type)
    {
    case    't':
        memcpy(respondPacket.data, timeStr, strlen(timeStr) + sizeof(char));
        respondPacket.len = strlen(timeStr) + sizeof(char);
        MyProtocol::sendMessage(clntSock, (char*)(&respondPacket), sizeof(DataPacket));
        break;
    case    'n':
        memcpy(respondPacket.data, getName().c_str(), getName().size() + sizeof(char));
        respondPacket.len = getName().size() + sizeof(char);
        MyProtocol::sendMessage(clntSock, (char*)(&respondPacket), sizeof(DataPacket));
        break;
    case    'l':
        str = ClientList::getClientList();
        memcpy(respondPacket.data, str.c_str(), str.size() + sizeof(char));
        respondPacket.len = str.size() + sizeof(char);
        MyProtocol::sendMessage(clntSock, (char*)(&respondPacket), sizeof(DataPacket));
        break;
    case    's':
        respondPacket.source = index;
        MyProtocol::sendMessage(ClientList::getSock(requestPacket->destiny), (char*)requestPacket, sizeof(DataPacket));
        break;
    case    'q':
        return 1;
    default:
        break;
    }
    return 0;
}