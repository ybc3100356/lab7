#include "client.h"
#include "../protocol/my_protocol.hpp"

#pragma warning(disable:4996)

Client* Client::instance = nullptr;

int Client::start()
{
    //create socket
    servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    bool quit = false;
    HANDLE hThread = nullptr;
    while (!quit)
    {
        //input
        DataPacket requestPacket;
        std::string inputStr;
        if (connected) std::cout << "(connected)";
        std::cout << "< ";
        std::getline(std::cin, inputStr);

        //input parse
        requestPacket.type = parse(inputStr);
        switch (requestPacket.type)
        {
        case 'c'://connect
            if (!connected)
            {
                connectToServer();
                if (connected)
                    hThread = CreateThread(NULL, 0, respondReceiver, &RespondData(servSock), 0, NULL);
            }
            else
                std::cout << "already connect to server!";
            break;
        case 't'://time
        case 'n'://name
        case 'l'://client list
            if (connected)
                MyProtocol::sendMessage(servSock, requestPacket, sizeof(DataPacket));
            else 
                std::cout << "please connect to server!";
            break;
        case 's'://send msg to another client
            if (connected)
            {
                if (clientList.empty())
                {
                    std::cout << "never get client before, now refreshing...";
                    MyProtocol::sendMessage(servSock, DataPacket('l'), sizeof(DataPacket));
                }
                std::cout << "please input the client number";
                std::cin >> requestPacket.destiny;
                std::cout << "please input content you want to send, and end with \';\'";
                std::cin.getline(requestPacket.data, 1024, ';');

                requestPacket.len = strlen(requestPacket.data) + 1;
                MyProtocol::sendMessage(servSock, requestPacket, sizeof(DataPacket));
            }
            else
                std::cout << "please connect to server!";
            break;
        case 'q'://quit
            quit = true;
            if (connected)
            {
                MyProtocol::sendMessage(servSock, DataPacket('q'), sizeof(DataPacket));
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
            std::cout << "unknown input!";
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
    std::cin >> IPaddr;
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
        std::unique_ptr<DataPacket> welcomePacket = MyProtocol::recvMessage(servSock);
        printMsg(*welcomePacket);
        if (welcomePacket->type == 'q')//disconnect
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
    std::unique_ptr<DataPacket> respondPacket;
    PtrToRespondData pData = (PtrToRespondData)lpParameter;
    SOCKET servSock = pData->servSock;

    bool quit = false;
    while (!quit)
    {
        respondPacket = MyProtocol::recvMessage(servSock);
        Client::instance->printMsg(*respondPacket);
        switch (respondPacket->type)
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
            Client::instance->refreshClientList(*respondPacket);
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
        p = strtok(nullptr, ",");//num
    }
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
        MyProtocol::sendMessage(servSock, DataPacket('q'), sizeof(DataPacket));
        instance = nullptr;
    }
}