#include "tcpsocket.h"
#include "../core/common.h"

namespace Epyx{
    TCPSocket::TCPSocket()
    {
        this->sock = ::socket(AF_INET, SOCK_STREAM, 0);
        if (this->sock == -1)
            throw ErrException("TCPSocket", "socket");
    }
    TCPSocket::TCPSocket(Address& addr)
        :Socket(addr)
    {
        this->sock = ::socket(AF_INET, SOCK_STREAM, 0);
        if (this->sock == -1)
            throw ErrException("TCPSocket", "socket");
    }
}