#include "client.h"
#include "../protocol/my_protocol.hpp"

#pragma warning(disable:4996)

Client* Client::instance = nullptr;

int Client::start()
{
    //创建套接字
    servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    //向服务器发起请求
    sockaddr_in sockAddr;
    memset(&sockAddr, 0, sizeof(sockAddr));
    sockAddr.sin_family = PF_INET;
    sockAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    sockAddr.sin_port = htons(1730);
    connect(servSock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));

    //welcome
    DataPacket requestPacket;
    std::string str;
    printMsg((DataPacket*)MyProtocol::recvMessage(servSock));

    bool quit = false;
    while (!quit)
    {
        //根据输入发送请求包
        std::cout << "> ";
        std::getline(std::cin, str);

        //输入解析
        requestPacket.type = parse(str);
        switch (requestPacket.type)
        {
        case    't':
        case    'n':
        case    'l':
            //request
            str = (char*)(&requestPacket);
            MyProtocol::sendMessage(servSock, (char*)(&requestPacket), sizeof(requestPacket) + sizeof(char));

            //respond
            printMsg((DataPacket*)MyProtocol::recvMessage(servSock));
            break;

        case    's'://send msg to another client
            std::cout << "please input the client number" << std::endl;
            std::cin >> requestPacket.destiny;

            std::cout << "please input content you want to send, and end with \';\'" << std::endl;
            std::cin.getline(requestPacket.data, 1024, ';');
            requestPacket.len = strlen(requestPacket.data) + 1;
            MyProtocol::sendMessage(servSock, (char*)(&requestPacket), sizeof(requestPacket) + sizeof(char));

            printMsg((DataPacket*)MyProtocol::recvMessage(servSock));
            break;
        case    'q':
            quit = true;
            break;
        default:
            std::cout << "unknown input!" << std::endl;
            break;
        }
        std::cout << std::endl;
    }

    char type = 'q';
    MyProtocol::sendMessage(servSock, (char*)(&type), sizeof(type) + sizeof(char));
    instance = nullptr;

    closesocket(servSock);
    return 0;
}

char Client::parse(std::string inputStr)
{
    RequestType p = RequestType::NONE_TYPE;
    if (inputStr[0] == 'q' || inputStr[0] == 'n' || inputStr[0] == 't' || inputStr[0] == 'l' || inputStr[0] == 's')
        return inputStr[0];
    else
        return '0';
}

void Client::printMsg(DataPacket* msg)
{
    std::cout << "message from ";
    if (msg->source == -1)
        std::cout << "server";
    else
        std::cout << "client " << msg->source;
    std::cout << ": " << msg->data << std::endl;
}


Client::Client() :servSock(SOCKET())
{
    if (instance == nullptr)
        instance = this;
    else
        throw InstanceExist();
}

Client::~Client()
{
    if (servSock && instance)
    {
        RequestType type = RequestType::QUIT;
        std::string msg = (char*)(&type);
        MyProtocol::sendMessage(servSock, msg.c_str(), msg.size() + sizeof(char));
        instance = nullptr;
    }
}

