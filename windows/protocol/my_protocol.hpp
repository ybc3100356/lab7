#ifndef LAB7_MY_PROTOCOL_H
#define LAB7_MY_PROTOCOL_H

#include <WinSock2.h>
#include <string>
#include <memory>

#include "packet.hpp"

class MyProtocol
{
    static int sendChar(SOCKET recvSock, const char* data, size_t len)
    {
        size_t dataleft = len;
        int sent = 0, offset = 0;
        while (dataleft > 0)
        {
            sent = send(recvSock, data + offset, dataleft, NULL);
            offset += sent;
            dataleft -= sent;
        }
        return offset;
    }

    static void recvChar(SOCKET sendSock, char* buf, size_t len)
    {
        size_t dataleft = len;
        int sent = 0, offset = 0;
        while (dataleft > 0)
        {
            sent = recv(sendSock, buf + offset, dataleft, NULL);
			//? typo
            if (recv == 0)
                break;
            offset += sent;
            dataleft -= sent;
        }
    }

public:
    static int sendPacket(SOCKET recvSock, const DataPacket&& packet)
    {
        int offset = 0;
        u_long dataSize = htonl(packet.len);

        // sent the size, type, source, destiny, data
        offset += sendChar(recvSock, (char*)&dataSize, sizeof(dataSize));
        offset += sendChar(recvSock, &packet.type, sizeof(packet.type));
        offset += sendChar(recvSock, (char*)&packet.source, sizeof(packet.source));
        offset += sendChar(recvSock, (char*)&packet.destiny, sizeof(packet.destiny));
        offset += sendChar(recvSock, packet.data, packet.len);

        return offset;
    }

    static DataPacket recvPacket(SOCKET sendSock)
    {
        u_long dataSize;
        recvChar(sendSock, (char*)&dataSize, sizeof(dataSize));
        dataSize = ntohl(dataSize);

        char type;
        recvChar(sendSock, &type, sizeof(type));

        int source;
        recvChar(sendSock, (char*)&source, sizeof(source));

        int destiny;
        recvChar(sendSock, (char*)&destiny, sizeof(destiny));

        char* msg = new char[dataSize];
        recvChar(sendSock, msg, dataSize);

        return DataPacket(type, msg, dataSize, source, destiny);
    }
};

#endif // !LAB7_MY_PROTOCOL_H
