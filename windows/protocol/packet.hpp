#ifndef LAB7_PACKET_H
#define LAB7_PACKET_H

#include <string>

class DataPacket {
    static const size_t MESSAGE_SIZE = 1024;
public:
    char type;
    size_t len;
    char data[MESSAGE_SIZE] = { 0 };
    int source;   // for send messgae to another client
    int destiny;  // for send messgae to another client
    DataPacket(char _type = '0', const char* _msg = nullptr, size_t _len = 0, int _source = -1, int _destiny = -1) :type(_type), len(_len), source(_source), destiny(_destiny)
    {
        memcpy(data, _msg, len);
    }

    DataPacket(const DataPacket& p) : type(p.type), len(p.len), source(p.source), destiny(p.destiny)
    {
        memcpy(data, p.data, len);
    }

    ~DataPacket() {}

    DataPacket& operator=(const DataPacket& rhs)
    {
        if (this != &rhs)
        {
            memcpy(data, rhs.data, len);
            len = rhs.len;
            type = rhs.type;
            source = rhs.source;
            destiny = rhs.destiny;
        }
        return *this;
    }
};

#endif // !LAB7_PACKET_H
