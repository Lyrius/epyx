/**
 * @file gttpacket.h
 * @brief Packet of the Generic Text Template
 */

#ifndef EPYX_GTTPACKET_H
#define EPYX_GTTPACKET_H

#include <map>
#include <string>
#include <ostream>

namespace Epyx
{
    class GTTPacket
    {
    public:
        GTTPacket();
        ~GTTPacket();
        friend std::ostream& operator<<(std::ostream& os, const GTTPacket& pkt);

        /**
         * Public variables
         */
        std::string protocol;
        std::string method;
        std::map<std::string, std::string> headers;
        long size;
        char *body;
    };
}

#endif /* EPYX_GTTPACKET_H */
