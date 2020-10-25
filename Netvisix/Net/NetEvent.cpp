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

#include "NetEvent.h"
#include "Net/NetUtil.h"


namespace Netvisix {

    NetEvent::NetEvent() {
        size = 0;

        l1Protocol = Protocol::UNKOWN;
        l2Protocol = Protocol::UNKOWN;
        l3Protocol = Protocol::UNKOWN;

        srcAddrHW = NetUtil::zeroAddrHW;
        dstAddrHW = NetUtil::zeroAddrHW;

        srcAddrIPv4 = NetUtil::zeroAddrIPv4;
        dstAddrIPv4 = NetUtil::zeroAddrIPv4;

        srcAddrIPv6 = NetUtil::zeroAddrIPv6;
        dstAddrIPv6 = NetUtil::zeroAddrIPv6;
    }

    NetEvent::~NetEvent() {
    }

    Protocol NetEvent::getProtocol(Layer layer) {
        switch (layer) {
        case Layer::L1:
            return l1Protocol;
        case Layer::L2:
            return l2Protocol;
        case Layer::L3:
            return l3Protocol;
        default:
            return Protocol::UNKOWN;
        }
    }

    Protocol NetEvent::getTopLevelProtocol() {
        if (l3Protocol != Protocol::UNKOWN) {
            return l3Protocol;
        }
        else if (l2Protocol != Protocol::UNKOWN) {
            return l2Protocol;
        }
        else if (l1Protocol != Protocol::UNKOWN) {
            return l1Protocol;
        }
        else {
            return Protocol::UNKOWN;
        }
    }

    std::string NetEvent::getProtocolString(Protocol protocol) {
        switch (protocol) {
        case Protocol::UNKOWN:
            return "UNKOWN";

        case Protocol::ETHERNETII:
            return "ETHERNETII";

        case Protocol::ARP:
            return "ARP";
        case Protocol::IPv4:
            return "IPv4";
        case Protocol::IPv6:
            return "IPv6";
        case Protocol::ICMP:
            return "ICMP";
        case Protocol::ICMPv6:
            return "ICMPv6";
        case Protocol::IGMP:
            return "IGMP";

        case Protocol::TCP:
            return "TCP";
        case Protocol::UDP:
            return "UDP";

        default:
            return "???";
        }
    }

    bool NetEvent::isIPv4() {
        if (l2Protocol == Protocol::IPv4 || l2Protocol == Protocol::IGMP || l2Protocol == Protocol::ICMP) {
            return true;
        }
        else {
            return false;
        }
    }

    bool NetEvent::isIPv6() {
        if (l2Protocol == Protocol::IPv6 || l2Protocol == Protocol::ICMPv6) {
            return true;
        }
        else {
            return false;
        }
    }

} // namespace Netvisix
