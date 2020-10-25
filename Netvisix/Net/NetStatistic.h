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

#ifndef NETSTATISTIC_H
#define NETSTATISTIC_H

#include <Net/NetEvent.h>

#include <vector>


namespace Netvisix {

    enum class IPVersion {
        IPV4,
        IPV6,
        ALL
    };

    enum class TrafficDirection {
        TD_IN,
        TD_OUT
    };

    class NetStatistic {

        public:
            struct Item {
                IPVersion ipVersion;
                Layer layer;
                Protocol protocol;
                unsigned long long packetsSnt = 0;
                unsigned long long packetsRcv = 0;
                unsigned long long bytesSnt = 0;
                unsigned long long bytesRcv = 0;
            };

            NetStatistic();
            virtual ~NetStatistic();

            void addProtocol(IPVersion ipVersion, Layer layer, Protocol protocol);

            void handleNetEvent(TrafficDirection trafficDirection, NetEvent* netEvent);

            Item* getItem(IPVersion ipVersion, Protocol protocol);

        private:
            std::vector<Item*>* items;

            void handleTrafficDirection(TrafficDirection trafficDirection, Item* item, NetEvent* ne);
    };

} // namespace Netvisix
#endif // NETSTATISTIC_H
