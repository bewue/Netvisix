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

#include "NetUtil.h"

#include <exception>
#include <sstream>
#include <iomanip>


namespace Netvisix {

    const Tins::HWAddress<6> NetUtil::zeroAddrHW = Tins::HWAddress<6>("00:00:00:00:00:00");
    const Tins::IPv4Address NetUtil::zeroAddrIPv4 = Tins::IPv4Address("0.0.0.0");
    const Tins::IPv6Address NetUtil::zeroAddrIPv6 = Tins::IPv6Address("::");


	NetUtil::NetUtil() {
	}

	NetUtil::~NetUtil() {
	}

    bool NetUtil::getIsSubnetStringIPv4Valid(std::string subnetAddrIPv4WithPrefix) {
        std::string addrIPv4 = getSubnetAddrStringIPv4(subnetAddrIPv4WithPrefix);
        if (addrIPv4 != "") {
            int prefix = getSubnetPrefix(subnetAddrIPv4WithPrefix);
            if (prefix < 0 || prefix > 32) {
                return false;
            }
            else {
                return true;
            }
        }
        else {
            return false;
        }
    }

    bool NetUtil::getIsSubnetStringIPv6Valid(std::string subnetAddrIPv6WithPrefix) {
        std::string addrIPv6 = getSubnetAddrStringIPv6(subnetAddrIPv6WithPrefix);
        if (addrIPv6 != "") {
            int prefix = getSubnetPrefix(subnetAddrIPv6WithPrefix);
            if (prefix < 0 || prefix > 128) {
                return false;
            }
            else {
                return true;
            }
        }
        else {
            return false;
        }
    }

    int NetUtil::getSubnetPrefix(std::string subnetAddrWithPrefix) {
        // check '/' is found & is not last character
        int slashPos = subnetAddrWithPrefix.find('/');
        if (slashPos == -1 || slashPos >= (int) subnetAddrWithPrefix.length() - 1) {
            return -1;
        }

        // check prefixString is not containing illegal characters
        std::string prefixString = subnetAddrWithPrefix.substr(slashPos + 1, subnetAddrWithPrefix.length());
        if (prefixString.find_first_not_of("0123456789") != std::string::npos) {
            return -1;
        }

        int prefix = std::strtol(prefixString.c_str(), nullptr, 10);
        if (prefix < 0) {
            return -1;
        }

        return prefix;
    }

    std::string NetUtil::getSubnetAddrStringIPv4(std::string subnetAddrIPv4WithPrefix) {
        std::string addrStringIPv4 = "";

        // check '/'
        int slashPos = subnetAddrIPv4WithPrefix.find('/');
        if (slashPos != -1) {
            addrStringIPv4 = subnetAddrIPv4WithPrefix.substr(0, slashPos);
        }

        // check ipv4 address validity
        Tins::IPv4Address addrIPv4 = zeroAddrIPv4;
        try {
            addrIPv4 = Tins::IPv4Address(addrStringIPv4);
        }
        catch (std::exception& e) {
            return "";
        }

        if (addrIPv4 != zeroAddrIPv4) {
            return addrStringIPv4;
        }
        else {
            return "";
        }
    }

    std::string NetUtil::getSubnetAddrStringIPv6(std::string subnetAddrIPv6WithPrefix) {
        std::string addrStringIPv6 = "";

        // check '/'
        int slashPos = subnetAddrIPv6WithPrefix.find('/');
        if (slashPos != -1) {
            addrStringIPv6 = subnetAddrIPv6WithPrefix.substr(0, slashPos);
        }

        // check ipv6 address validity
        Tins::IPv6Address addrIPv6 = zeroAddrIPv6;
        try {
            addrIPv6 = Tins::IPv6Address(addrStringIPv6);
        }
        catch (std::exception& e) {
            return "";
        }

        if (addrIPv6 != zeroAddrIPv6) {
            return addrStringIPv6;
        }
        else {
            return "";
        }
    }

    Tins::AddressRange<Tins::IPv4Address> NetUtil::getSubnetRangeIPv4(std::string subnetAddrIPv4WithPrefix) {
        if (getIsSubnetStringIPv4Valid(subnetAddrIPv4WithPrefix) == false) {
            return Tins::IPv4Address("0.0.0.0") / 32;
        }
        else {
            std::string subnetAddr = getSubnetAddrStringIPv4(subnetAddrIPv4WithPrefix);
            int prefix = getSubnetPrefix(subnetAddrIPv4WithPrefix);
            return Tins::IPv4Address(subnetAddr) / prefix;
        }
    }

    Tins::AddressRange<Tins::IPv6Address> NetUtil::getSubnetRangeIPv6(std::string subnetAddrIPv6WithPrefix) {
        if (getIsSubnetStringIPv6Valid(subnetAddrIPv6WithPrefix) == false) {
            return Tins::IPv6Address("fd00::") / 8;
        }
        else {
            std::string subnetAddr = getSubnetAddrStringIPv6(subnetAddrIPv6WithPrefix);
            int prefix = getSubnetPrefix(subnetAddrIPv6WithPrefix);
            return Tins::IPv6Address(subnetAddr) / prefix;
        }
    }

    bool NetUtil::CheckCurrentUserSniffingPrivilegs(std::string interfaceName, std::string* excecptionString) {
        try {
            Tins::Sniffer sniffer(interfaceName);
        }
        catch (std::exception& e) {
            *excecptionString = std::string(e.what());
            return false;
        }

        return true;
    }

    std::string NetUtil::ResolveHostname(Tins::IPv4Address) {

        return "";
    }

    std::string NetUtil::getByteString(unsigned long long byteCount) {
        std::string byteString = "";

        if (byteCount < 1024.0) {
            byteString = std::to_string(byteCount);
            byteString += " B";
            return byteString;
        }

        std::stringstream ss;

        double byteCountD = (double) byteCount / 1024.0;
        if (byteCountD < 1024.0) {
            ss << std::fixed << std::setprecision(2) << byteCountD;
            byteString = ss.str();
            byteString += " KiB";
            return byteString;
        }

        byteCountD /= 1024.0;
        if (byteCountD < 1024.0) {
            ss << std::fixed << std::setprecision(2) << byteCountD;
            byteString = ss.str();
            byteString += " MiB";
            return byteString;
        }

        byteCountD /= 1024.0;
        ss << std::fixed << std::setprecision(2) << byteCountD;
        byteString = ss.str();
        byteString += " GiB";
        return byteString;
    }

} // namespace Netvisix
