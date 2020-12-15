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

        l1Protocol = Protocol::Unkown;
        l2Protocol = Protocol::Unkown;
        l3Protocol = Protocol::Unkown;

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
            return Protocol::Unkown;
        }
    }

    Protocol NetEvent::getTopLevelProtocol() {
        if (l3Protocol != Protocol::Unkown) {
            return l3Protocol;
        }
        else if (l2Protocol != Protocol::Unkown) {
            return l2Protocol;
        }
        else if (l1Protocol != Protocol::Unkown) {
            return l1Protocol;
        }
        else {
            return Protocol::Unkown;
        }
    }

    std::string NetEvent::getProtocolString(Protocol protocol) {
        switch (protocol) {
        case Protocol::Unkown:
            return "Unkown";
        case Protocol::OtherL2:
            return "OtherL2";
        case Protocol::OtherL3:
            return "OtherL3";

        case Protocol::EthernetII:
            return "EthernetII";

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
        case Protocol::TCP:
            return "TCP";
        case Protocol::UDP:
            return "UDP";

        default:
            return "???";
        }
    }

    bool NetEvent::isIPv4() {
        return (l2Protocol == Protocol::IPv4);
    }

    bool NetEvent::isIPv6() {
        return (l2Protocol == Protocol::IPv6);
    }

} // namespace Netvisix
