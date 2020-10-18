#ifndef LAB7_MY_PROTOCOL_H
#define LAB7_MY_PROTOCOL_H

#include <WinSock2.h>
#include <string>
#include <memory>

#include "packet.hpp"

class MyProtocol
{
public:
    static int sendMessage(SOCKET recvSock, const DataPacket& packet, size_t len)
    {
        char* message = (char*)&packet;
        int sent = 0, offset = 0;
        auto msgSize = htonl(len);
        auto dataleft = sizeof(msgSize);

        while (dataleft > 0)// sent the length of msg
        {
            sent = send(recvSock, ((char*)&msgSize) + offset, dataleft, NULL);
            offset += sent;
            dataleft -= sent;
        }

        offset = 0;
        dataleft = len;
        while (dataleft > 0)
        {
            sent = send(recvSock, message + offset, dataleft, NULL);
            offset += sent;
            dataleft -= sent;
        }
        return offset;
    }

    static std::unique_ptr<DataPacket> recvMessage(SOCKET sendSock)
    {
        int offset = 0, recved;
        char msgSize[sizeof(int)];

        auto dataleft = sizeof(u_long);//u_long: the return type of ntohl()
        while (dataleft > 0)
        {
            recved = recv(sendSock, msgSize + offset, dataleft, NULL);
            offset += recved;
            dataleft -= recved;
        }

        offset = 0;
        dataleft = ntohl(*(int*)(msgSize));
        char *msg = new char[dataleft];
        while (dataleft > 0)
        {
            //��������
            recved = recv(sendSock, msg + offset, dataleft, NULL);
            if (recv == 0)
            {
                break;
            }
            offset += recved;
            dataleft -= recved;
        }

        return std::make_unique<DataPacket>(*(DataPacket*)msg);
    }
};

#endif // !LAB7_MY_PROTOCOL_H
