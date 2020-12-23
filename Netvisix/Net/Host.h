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

#ifndef HOST_H
#define HOST_H

#include <tins.h>


namespace Netvisix {

    enum class NetArea;
    class NetStatistic;

    class Host {

        public:
            Host();
            virtual ~Host();

//            std::string getDebugString();

            void setNetArea(NetArea netArea) { this->netArea = netArea; }
            NetArea getNetArea() { return this->netArea; }

            void addAddrIPv4(Tins::IPv4Address ipv4, bool update);
            const Tins::IPv4Address& getAddrIPv4(Tins::IPv4Address ipv4) const;
            const Tins::IPv4Address& getAddrIPv4(const std::string& ipv4) const;
            const std::vector<Tins::IPv4Address>& getAddrListIPv4() const { return addrListIPv4; }

            void addAddrIPv6(Tins::IPv6Address ipv6, bool update);
            const Tins::IPv6Address& getAddrIPv6(Tins::IPv6Address ipv6) const;
            const Tins::IPv6Address& getAddrIPv6(const std::string& ipv6) const;
            const std::vector<Tins::IPv6Address>& getAddrListIPv6() const { return addrListIPv6; }

            std::string getPreferedHostIdentifier();

            std::string getHostname() { return hostname; }
            void setHostname(const std::string& hostname, bool update);

            bool getHostnameIsFromReverseDNSLookp() { return hostnameIsFromReverseDNSLookp; }
            void setHostnameIsFromReverseDNSLookp(bool fromRDNS) { this->hostnameIsFromReverseDNSLookp = fromRDNS; }

            const Tins::HWAddress<6>& getAddrHW() { return addrHW; }
            void setAddrHW(Tins::HWAddress<6> hw, bool update);

            NetStatistic* statistic;

        private:
            void onAddrUpdate();

            NetArea netArea;

            std::string hostname;
            bool hostnameIsFromReverseDNSLookp;

            Tins::HWAddress<6> addrHW;

            std::vector<Tins::IPv4Address> addrListIPv4;
            std::vector<Tins::IPv6Address> addrListIPv6;
    };

} // namespace Netvisix
#endif // HOST_H
