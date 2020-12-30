#ifndef LAB7_MY_PROTOCOL_H
#define LAB7_MY_PROTOCOL_H

#include <memory>
#include <numeric>
#include <string>

#include <WinSock2.h>

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
        int received = 0, offset = 0;
        while (dataleft > 0)
        {
            received = recv(sendSock, buf + offset, dataleft, NULL);
            offset += received;
            dataleft -= received;
        }
    }

public:
    static int sendPacket(SOCKET recvSock, const DataPacket&& packet)
    {
        u_long dataSize = htonl(packet.len);

		// sent the size, type, source, destiny, data
		const size_t lens[] = { sizeof(dataSize), sizeof(packet.type), sizeof(packet.source), sizeof(packet.destiny), packet.len };
		auto tsdu_len = std::accumulate(lens, lens + sizeof(lens)/sizeof(lens[0]), 0U);
		auto tsdu = std::make_unique<char[]>(tsdu_len);
		auto offset = 0, i = 0;
		std::memcpy(&tsdu[offset], &dataSize, lens[i]); offset += lens[i]; i++;
		std::memcpy(&tsdu[offset], &packet.type, lens[i]); offset += lens[i]; i++;
		std::memcpy(&tsdu[offset], &packet.source, lens[i]); offset += lens[i]; i++;
		std::memcpy(&tsdu[offset], &packet.destiny, lens[i]); offset += lens[i]; i++;
		std::memcpy(&tsdu[offset], packet.data, lens[i]); offset += lens[i]; i++;

        // sent assembled TSDU
        offset = sendChar(recvSock, tsdu.get(), tsdu_len);
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
