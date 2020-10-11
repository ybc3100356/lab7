#ifndef LAB7_MY_PROTOCOL_H
#define LAB7_MY_PROTOCOL_H
const size_t BUFFERSIZE = 100;

#include <WinSock2.h>
#include <string>

class MyProtocol
{
public:
    static int sendMessage(SOCKET recvSock, const char* message, size_t len)
    {
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

    static std::string recvMessage(SOCKET sendSock)
    {
        int offset = 0;
        int recved;
        char msgSize[sizeof(int)];
        char buffer[BUFFERSIZE];
        std::string msg;

        auto dataleft = sizeof(u_long);//u_long: the return type of ntohl()
        while (dataleft > 0)
        {
            recved = recv(sendSock, msgSize + offset, dataleft, NULL);
            offset += recved;
            dataleft -= recved;
        }

        offset = 0;
        dataleft = ntohl(*(int*)(msgSize));
        while (dataleft > 0)
        {
            //接收数据
            recved = recv(sendSock, buffer + offset, dataleft, NULL);
            //msg += std::string(buffer + offset);
            if (recv == 0)
            {
                break;
            }
            offset += recved;
            dataleft -= recved;
        }

        msg = buffer;
        return msg;
    }
};

#endif // !LAB7_MY_PROTOCOL_H
