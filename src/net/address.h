/**
 * IP addresses and ports management
 */
#ifndef EPYX_ADDRESS_H
#define EPYX_ADDRESS_H

#include <iostream>
#include <arpa/inet.h>

namespace Epyx
{
    class Address
    {
    private:
        unsigned short port;
        char ipStr[INET6_ADDRSTRLEN + 1];
        int ipVersion;//4 or 6

    public:
        Address();
        Address(const char *ip, unsigned short port_, int ipVersion_ = 4);
        Address(const struct sockaddr *saddr);
        Address(const Address& addr);

        const char* ip();
        unsigned short getPort();

        // Usage : use a sockaddr_storage stucture to get information for IPv4
        // and IPv6 addresses
        void getSockAddr(struct sockaddr *saddr);

        friend std::ostream& operator<<(std::ostream& os, const Address& addr);
        friend std::ostream& operator<<(std::ostream& os, const Address *addr);

        /**
         * @brief get a std::string representation of the address
         */
        std::string toString();


        friend bool operator==(const Address& addr1, const Address& addr2);

        friend inline bool operator!=(const Address& addr1, const Address& addr2)
        {
            return !(addr1 == addr2);
        }
    };
}

#endif /* EPYX_ADDRESS_H */
