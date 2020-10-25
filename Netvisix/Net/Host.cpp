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

#include "Host.h"
#include "Net/NetEventManager.h"
#include "Net/NetStatistic.h"
#include "Net/NetUtil.h"

namespace Netvisix {

    Host::Host() {
        hostname = "";
        hostnameFromReverseDNSLookp = false;

        addrHW = Tins::HWAddress<6>("00:00:00:00:00:00");
        netArea = NetArea::UNKOWN;

        statistic = new NetStatistic();
    }

    Host::~Host() {
        delete statistic;
    }

//    std::string Host::getDebugString() {
//        std::string s = "Host : " + addrHW.to_string() + "  " + addrIPv4.to_string() + "  " + addrIPv6.to_string();
//        return s;
//    }

    void Host::addAddrIPv4(Tins::IPv4Address ipv4) {
        if (ipv4 != NetUtil::zeroAddrIPv4) {
            addrListIPv4.push_back(ipv4);
        }
    }

    const Tins::IPv4Address& Host::getAddrIPv4(Tins::IPv4Address ipv4) const {
        if (ipv4 == NetUtil::zeroAddrIPv4) {
            return NetUtil::zeroAddrIPv4;
        }

        auto ip = std::find(addrListIPv4.begin(), addrListIPv4.end(), ipv4);
        return ip != addrListIPv4.end() ? *ip : NetUtil::zeroAddrIPv4;
    }

    const Tins::IPv4Address& Host::getAddrIPv4(const std::string& ipv4) const {
        for (const Tins::IPv4Address& ip : addrListIPv4) {
                if (ip.to_string() == ipv4) {
                    return ip;
                }
        }

        return NetUtil::zeroAddrIPv4;
    }

    void Host::addAddrIPv6(Tins::IPv6Address ipv6) {
        if (ipv6 != NetUtil::zeroAddrIPv6) {
            addrListIPv6.push_back(ipv6);
        }
    }

    const Tins::IPv6Address& Host::getAddrIPv6(Tins::IPv6Address ipv6) const {
        if (ipv6 == NetUtil::zeroAddrIPv6) {
            return NetUtil::zeroAddrIPv6;
        }

        auto ip = std::find(addrListIPv6.begin(), addrListIPv6.end(), ipv6);
        return ip != addrListIPv6.end() ? *ip : NetUtil::zeroAddrIPv6;
    }

    const Tins::IPv6Address& Host::getAddrIPv6(const std::string& ipv6) const {
        for (const Tins::IPv6Address& ip : addrListIPv6) {
                if (ip.to_string() == ipv6) {
                    return ip;
                }
        }

        return NetUtil::zeroAddrIPv6;
    }

} // namespace Netvisix
