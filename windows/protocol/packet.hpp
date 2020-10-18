#ifndef LAB7_PACKET_H
#define LAB7_PACKET_H

#include <string>

class DataPacket {
public:
    char type;
    int source;   // for send messgae to another client
    int destiny;  // for send messgae to another client
    size_t len;
    char* data;

    DataPacket(char _type = '0', const char* _msg = nullptr, size_t _len = 0, int _source = -1, int _destiny = -1) :
        type(_type), len(_len), source(_source), destiny(_destiny)
    {
        if (len)
        {
            data = new char[len];
            memcpy(data, _msg, len);
        }
        else
            data = nullptr;
    }

    DataPacket(const DataPacket& p) : type(p.type), len(p.len), source(p.source), destiny(p.destiny)
    {
        delete[] data;
        memcpy(data, p.data, len);
    }

    ~DataPacket() { delete[] data; }

    DataPacket& operator=(const DataPacket& rhs)
    {
        if (this != &rhs)
        {
            delete[] data;
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
