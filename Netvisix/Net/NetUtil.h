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

#ifndef NETUTIL_H
#define NETUTIL_H

#include <tins.h>


namespace Netvisix {

	class NetUtil {
		
		public:
			NetUtil();
			virtual ~NetUtil();

            static int getSubnetPrefix(std::string subnetAddrWithPrefix);
            static std::string getSubnetAddrStringIPv4(std::string subnetAddrIPv4WithPrefix);
            static std::string getSubnetAddrStringIPv6(std::string subnetAddrIPv6WithPrefix);
            static Tins::AddressRange<Tins::IPv4Address> getSubnetRangeIPv4(std::string subnetAddrIPv4WithPrefix);
            static Tins::AddressRange<Tins::IPv6Address> getSubnetRangeIPv6(std::string subnetAddrIPv6WithPrefix);

            static bool getIsSubnetStringIPv4Valid(std::string subnetAddrIPv4WithPrefix);
            static bool getIsSubnetStringIPv6Valid(std::string subnetAddrIPv6WithPrefix);

            static bool CheckCurrentUserSniffingPrivilegs(std::string interfaceName, std::string* excecptionString);

            static std::string ResolveHostname(Tins::IPv4Address);

            static std::string getByteString(unsigned long long byteCount);

            static const Tins::HWAddress<6> zeroAddrHW;
            static const Tins::IPv4Address zeroAddrIPv4;
            static const Tins::IPv6Address zeroAddrIPv6;
			
		protected:
			
		private:
			
	};
	
} // namespace Netvisix
#endif // NETUTIL_H
