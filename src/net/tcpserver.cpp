#include "tcpserver.h"
#include "../core/common.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

namespace Epyx
{
    TCPServer::TCPServer(unsigned short port, unsigned int nbConn)
        :Server(port), nbConn(nbConn)
    {
    }

    TCPServer::~TCPServer()
    {
    }

    bool TCPServer::bind()
    {
        if (!this->_internal_bind(SOCK_STREAM))
            return false;

        // Now there is a socket, listen to nbConn connections
        EPYX_ASSERT(sockfd >= 0);
        int status = listen(sockfd, nbConn);
        if (status < 0) {
            log::error << "listen: " << log::errstd << log::endl;
            this->close();
            return false;
        }
        return true;
    }
}
