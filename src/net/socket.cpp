#include "socket.h"
#include "../core/common.h"
#include <iostream>
#include <cstring>
#if defined (WIN32)
    #include <winsock2.h>
#else
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <netdb.h>
    #include <stdio.h>
    #include <arpa/inet.h>
    #define SOCKET_ERROR (-1)
#endif

namespace Epyx
{
    // Socket library initialisation
    int Socket::is_init = 0;
    Mutex Socket::init_mutex;

    void Socket::init(){
        if (is_init == 0){
            #if defined (WIN32)
                WSADATA WSAData;
                int error = WSAStartup(MAKEWORD(2,2), &WSAData);
            #endif
        }
        init_mutex.lock();
        is_init++;
        init_mutex.unlock();
    }

    void Socket::fini(){
        init_mutex.lock();
        is_init--;
        init_mutex.unlock();
        if (is_init == 0){
            #if defined (WIN32)
                WSACleanup();
            #endif
        }
    }

    Socket::Socket()
        :sock(-1)
    {
    }

    Socket::Socket(const Address &addr)
        :sock(-1), address(addr)
    {
    }

    Socket::Socket(const char* addr, unsigned short port)
        :sock(-1), address(addr, port)
    {
    }

    Socket::Socket(int sock, const Address &addr)
        :sock(sock), address(addr)
    {
        EPYX_ASSERT(sock >= 0);
    }

    Socket::~Socket()
    {
        this->close();
    }

    void Socket::setFd(int sockfd)
    {
        EPYX_ASSERT(this->sock < 0);
        EPYX_ASSERT(sockfd >= 0);
        this->sock = sockfd;
    }

    int Socket::getFd()
    {
        EPYX_ASSERT(sock >= 0);
        return sock;
    }

    bool Socket::isOpened()
    {
        return (sock >= 0);
    }

    void Socket::setAddress(const Address& addr)
    {
        address = addr;
    }

    Address Socket::getAddress()
    {
        return address;
    }

    void Socket::setLocalAddress(const Address& addr)
    {
        localAddress = addr;
    }

    Address Socket::getLocalAddress()
    {
        return localAddress;
    }

   

    void Socket::close(){ //TODO Implement the tests.
        //Epyx::log::debug << "Closing Socket ..." << Epyx::log::endl;
        //Is Socket connected? If so,
        if (sock < 0)
            return;
        ::shutdown(sock,SHUT_RDWR);
        ::close(sock);
        sock = -1;
        //Epyx::log::debug << "Socket Closed" << Epyx::log::endl;
    }

    
    /**
     * Send all bytes through the network
     * return: number of sent bytes
     */
    void Socket::sendAll(const void *data, int size)
    {
        int sentBytes;
        EPYX_ASSERT(data != NULL);
        while (size > 0) {
            sentBytes = this->send(data, size);
            if (!sentBytes)
                throw FailException("Socket", "Unable to send data");
            EPYX_ASSERT(sentBytes <= size);
            size -= sentBytes;
        }
    }

    /**
     * Send text through the network
     */
    void Socket::write(std::string message)
    {
        // The c_str contains one more character than the length of the string : \0
        // but we do not want to send \0 character through the network
        this->sendAll(message.c_str(), message.length());
    }

    
    /**
     * Receive exactly (size) bytes from the network
     * @data: buffer pointer
     * @size: buffer size
     */
    void Socket::recvAll(void *data, int size)
    {
        int recvBytes;
        EPYX_ASSERT(data != NULL);
        // TODO: feof !
        while (size > 0) {
            recvBytes = this->recv(data, size);
            if (!recvBytes)
                throw FailException("Socket", "Unable to recv data");
            EPYX_ASSERT(recvBytes <= size);
            size -= recvBytes;
        }
    }

    /**
     * Receive a line from the network
     * @out: output stream where data are written
     * return: true if a line was written, false otherwise
     *
     * TODO: Use an internal buffer instead of read one character each time
     */
    bool Socket::recvLine(std::ostream& out)
    {
        char c;
        while (this->recv(&c, 1)) {
            // Socket may receive \r\n end-of-line
            if (this->last_eol == '\r' && c == '\n')
                continue;

            // Detect line end character
            if (c == '\n' || c == '\r') {
                this->last_eol = c;
                return true;
            }

            // Reset EOL if a normal character is received
            if (this->last_eol)
                this->last_eol = 0;

            out << c;
        }
        return false;
    }

    /**
     * Read a line into a string
     */
    std::string Socket::read()
    {
        std::ostringstream out;
        if (!this->recvLine(out))
            return "";
        return out.str();
    }
}
