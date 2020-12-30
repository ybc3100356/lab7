#include <memory>
#include "client.h"
#include "../protocol/my_protocol.hpp"
#include "msg_queue.hpp"

#pragma warning(disable:4996)

Client* Client::instance = nullptr;
msg_queue<std::unique_ptr<DataPacket>> mq;

int Client::start()
{
    //create socket
    servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    bool quit = false;
    HANDLE hThread = nullptr;
	std::cout << menu;
    while (!quit)
    {
        //input
        std::string inputStr;
        int destiny;
        char type;
        if (connected) std::cout << "(connected)";
        std::cout << "< ";
        std::getline(std::cin, inputStr);

        //input parse
        switch (type = parse(inputStr))
        {
        case 'c'://connect
            if (!connected)
            {
                connectToServer();
                if (connected)
                {
                    PtrToRespondData pData = new RespondData(servSock);
                    hThread = CreateThread(NULL, 0, respondReceiver, pData, 0, NULL);
                }
            }
            else
                std::cout << "already connect to server!";
            break;
        case 't'://time
        case 'n'://name
        case 'l'://client list
            if (connected)
            {
                MyProtocol::sendPacket(servSock, DataPacket(type));
            }
            else
                std::cout << "please connect to server!";
            break;
        case 's'://send msg to another client
            if (connected)
            {
                if (clientList.empty())
                {
                    std::cout << "never get client before, now refreshing...";
                    MyProtocol::sendPacket(servSock, DataPacket('l'));
                }
                std::cout << "please input the client number: ";
                (std::cin >> destiny).get();
                std::cout << "please input content you want to send below, ending with LF" << std::endl;
                std::getline(std::cin, inputStr);
                MyProtocol::sendPacket(servSock, DataPacket('s', inputStr.c_str(), inputStr.size() + sizeof(char), -1, destiny));
            }
            else
                std::cout << "please connect to server!";
            break;
        case 'q'://quit
            quit = true;
            if (connected)
            {
                MyProtocol::sendPacket(servSock, DataPacket('q'));
                if (hThread)
                {
                    WaitForSingleObject(hThread, INFINITE);
                    CloseHandle(hThread);
                }
                closesocket(servSock);
                connected = false;
            }
            std::cout << "bye!";
            break;
        default:
            std::cout << "unknown input:" << inputStr << std::endl;
			std::cout << menu;
            break;
        }
        std::cout << std::endl;
    }

    instance = nullptr;
    return 0;
}


void Client::connectToServer()
{
    sockaddr_in sockAddr = { 0 };

    std::cout << "please input the server IP address" << std::endl;
    std::string IPaddr;
    (std::cin >> IPaddr).get();
    sockAddr.sin_addr.S_un.S_addr = inet_addr(IPaddr.c_str());
    if (sockAddr.sin_addr.S_un.S_addr == INADDR_NONE || sockAddr.sin_addr.S_un.S_addr == INADDR_ANY) {
        std::cout << "invalid IP addr!" << std::endl;
        return;
    }

    sockAddr.sin_family = PF_INET;
    sockAddr.sin_port = htons(1730);
    connect(servSock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));

    if (servSock)
    {
        //welcome
        DataPacket welcomePacket = MyProtocol::recvPacket(servSock);
        printMsg(welcomePacket);
        if (welcomePacket.type == 'q')//disconnect
        {
            closesocket(servSock);
            servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        }
        else
        {
            connected = true;
            return;
        }
    }

    std::cout << "failed to connect to server! please try again!" << std::endl;
}

DWORD WINAPI respondReceiver(LPVOID lpParameter)
{
    PtrToRespondData pData = (PtrToRespondData)lpParameter;
    SOCKET servSock = pData->servSock;
    delete pData;

    bool quit = false;
    while (!quit)
    {
        DataPacket respondPacket = MyProtocol::recvPacket(servSock);
        //TODO Msg from server when no data
        if (respondPacket.data)
        {
            Client::instance->printMsg(respondPacket);
        }
        switch (respondPacket.type)
        {
        case 'c':
        case 't':
        case 'n':
        case 's':
            break;
        case 'q':
            quit = true;
            break;
        case 'l':
            Client::instance->refreshClientList(respondPacket);
            break;
        }
    }
    return 0;
}

char Client::parse(std::string inputStr)
{
    if (inputStr[0] == 'c' || inputStr[0] == 'q' || inputStr[0] == 'n' || inputStr[0] == 't' || inputStr[0] == 'l' || inputStr[0] == 's')
        return inputStr[0];
    else
        return '0';
}

void Client::printMsg(const DataPacket& msg)
{
    /*TODO
    (connected)< bla(interrupted by incoming msg)
    > message from server: S E R V E R
    (connected)< bla
    */
    std::cout << "\n> message from ";
    if (msg.source == -1)
        std::cout << "server";
    else
        std::cout << "client " << msg.source;
    std::cout << ": " << msg.data << std::endl;
}

void Client::refreshClientList(DataPacket& clientListPacket)
{
    clientList.clear();
    char* p = std::strtok(clientListPacket.data, ",");
    while (p) {
        clientList.push_back(std::stoi(p + 7));//num
        p = strtok(nullptr, ",");//addr
        p = strtok(nullptr, ",");//port
        p = strtok(nullptr, ",");//next num
    }
    delete[] p;
}

Client::Client() :servSock(SOCKET()), connected(false)
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
        MyProtocol::sendPacket(servSock, DataPacket('q'));
        instance = nullptr;
    }
}