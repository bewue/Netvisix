/*
 *  Copyright (C) 2014 Benjamin W. (bitbatzen@gmail.com)
 *
 *  This file is part of Netvisix.
 *
 *  Netvisix is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Netvisix is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Netvisix.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef NETEVENT_H
#define NETEVENT_H

#include <tins.h>


namespace Netvisix {

    enum class Layer {
        L1,
        L2,
        L3,
    };

    enum class Protocol {
        Unkown,

        // layer 1
        EthernetII,

        // layer 2
        ARP,
        IPv4,
        IPv6,
        OtherL2,

        // layer 3
        ICMP,
        ICMPv6,
        TCP,
        UDP,
        OtherL3,
    };

    enum class AddrType {
        UNICAST,
        MULTICAST,
        UNKOWN
    };


    class NetEvent {

        public:
            NetEvent();
            virtual ~NetEvent();

            static std::string getProtocolString(Protocol protocol);

            Protocol getProtocol(Layer layer);
            void setProtocol(Layer layer, Protocol protocol);

            Protocol getTopLevelProtocol();

            bool isIPv4();
            bool isIPv6();

            unsigned long size;

            Protocol l1Protocol;
            Protocol l2Protocol;
            Protocol l3Protocol;

            Tins::HWAddress<6> srcAddrHW;
            Tins::HWAddress<6> dstAddrHW;

            Tins::IPv4Address srcAddrIPv4;
            Tins::IPv4Address dstAddrIPv4;

            Tins::IPv6Address srcAddrIPv6;
            Tins::IPv6Address dstAddrIPv6;
    };

} // namespace Netvisix
#endif // NETEVENT_H


