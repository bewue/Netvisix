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

#include "NetStatistic.h"

namespace Netvisix {

    NetStatistic::NetStatistic() {
        items = new std::vector<Item*>();

        addProtocol(IPVersion::ALL, Layer::L1, Protocol::EthernetII);
        addProtocol(IPVersion::ALL, Layer::L1, Protocol::Unkown);
        addProtocol(IPVersion::ALL, Layer::L2, Protocol::OtherL2);
        addProtocol(IPVersion::IPV4, Layer::L2, Protocol::ARP);

        // IPv4
        addProtocol(IPVersion::IPV4, Layer::L3, Protocol::ICMP);
        addProtocol(IPVersion::IPV4, Layer::L3, Protocol::TCP);
        addProtocol(IPVersion::IPV4, Layer::L3, Protocol::UDP);
        addProtocol(IPVersion::IPV4, Layer::L3, Protocol::OtherL3);

        // IPv6
        addProtocol(IPVersion::IPV6, Layer::L3, Protocol::ICMPv6);
        addProtocol(IPVersion::IPV6, Layer::L3, Protocol::TCP);
        addProtocol(IPVersion::IPV6, Layer::L3, Protocol::UDP);
        addProtocol(IPVersion::IPV6, Layer::L3, Protocol::OtherL3);
    }

    NetStatistic::~NetStatistic() {
        for (unsigned int i = 0; i < items->size(); i++) {
            delete items->at(i);
        }
        delete items;
    }

    NetStatistic::Item* NetStatistic::getItem(IPVersion ipVersion, Protocol protocol) {
        unsigned int itemsSize = items->size();
        for (unsigned int i = 0; i < itemsSize; i++) {
            Item* item = items->at(i);
            if (item->ipVersion == ipVersion && item->protocol == protocol) {
                return item;
            }
        }

        return nullptr;
    }

    void NetStatistic::addProtocol(IPVersion ipVersion, Layer layer, Protocol protocol) {
        Item* item = new Item();
        item->ipVersion = ipVersion;
        item->layer = layer;
        item->protocol = protocol;
        items->push_back(item);
    }

    void NetStatistic::handleNetEvent(TrafficDirection trafficDirection, NetEvent* netEvent) {
        unsigned int itemsSize = items->size();
        for (unsigned int i = 0; i < itemsSize; i++) {
            Item* item = items->at(i);

            if (netEvent->getProtocol(item->layer) == item->protocol) {

                if (item->ipVersion == IPVersion::ALL
                        || (netEvent->isIPv4() && item->ipVersion == IPVersion::IPV4)
                        || (netEvent->isIPv6() && item->ipVersion == IPVersion::IPV6)
                        || item->protocol == Protocol::ARP) {

                    handleTrafficDirection(trafficDirection, item, netEvent);
                }
            }
        }
    }

    void NetStatistic::handleTrafficDirection(TrafficDirection trafficDirection, Item *item, NetEvent *ne) {
        if (trafficDirection == TrafficDirection::TD_IN) {
            item->packetsRcv++;
            item->bytesRcv += ne->size;
        }
        else {
            // OUT
            item->packetsSnt++;
            item->bytesSnt += ne->size;
        }
    }

} // namespace Netvisix
