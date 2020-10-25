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

#ifndef NETEVENTMANAGER_H
#define NETEVENTMANAGER_H

#include <string>
#include <vector>
#include <time.h>

#include <tins.h>


namespace Netvisix {

    class NetEvent;
    enum class AddrType;
    class Host;
    class IPreparedNetEventListener;
    class PacketHandler;

    enum class NetArea {
        LOCAL_INTERFACE,
        SUBNET,
        OUTSIDE_SUBNET,
        UNKOWN
    };

    class NetEventManager {

        public:
            virtual ~NetEventManager();
            static NetEventManager* SharedInstance();

            static void release();

            void reset();

            void startSniffing(std::string interfaceName, bool promiscMode, std::string subnetIPv4, std::string subnetIPv6);
            void stopSniffing();
            bool getIsSniffingRunning();

            void handleNetEvent(NetEvent* ne);

            void handleDNSAnswer(std::string ipAddr, std::string hostname);

            void setPreparedNetEventListener(IPreparedNetEventListener* listener) { this->pneListener = listener; }

            bool getIsPaused() { return this->isPaused; }
            void setIsPaused(bool paused) { this->isPaused = paused; }

            unsigned long getNetEventCount() { return this->netEventCounter; }

            bool getIsLocalInterface(Host* h);

        private:
            struct DNSAnswer {
                std::string ipAddr;
                std::string hostname;
                std::clock_t timeStamp;
            };

            NetEventManager();

            void handleNewHosts(NetEvent* ne);

            void handleNewPackets(NetEvent* ne);

            Host* getHost(Tins::HWAddress<6> hwAddr);
            Host* getHost(Tins::IPv4Address ipv4Addr);
            Host* getHost(Tins::IPv6Address ipv6Addr);
            Host* getHost(Tins::HWAddress<6> hwAddr, Tins::IPv4Address ipv4Addr, Tins::IPv6Address ipv6Addr);

            void addHost(Host* host);

            void checkNewHost(Tins::HWAddress<6> hwAddr, Tins::IPv4Address ipv4Addr);
            void checkNewHost(Tins::HWAddress<6> hwAddr, Tins::IPv6Address ipv6Addr);

            bool getIsHostInSubnet(Tins::IPv4Address ipv4Addr);
            bool getIsHostInSubnet(Tins::IPv6Address ipv6Addr);

            Tins::IPv4Address getSubnetBroadcastAddrIPv4();

            AddrType getAddrType(Tins::HWAddress<6> hwAddr);
            AddrType getAddrType(Tins::IPv4Address ipv4Addr);
            AddrType getAddrType(Tins::IPv6Address ipv6Addr);

            std::string getDebugString(NetEvent* ne);

            void initHostLocalInterface(std::string interfaceName);

            void handleNetAreaSubnet(Host* host);

            void removeOldDNSAnswers();
            void setHostnameFromDNSAnswers(Host* host);

            static NetEventManager* instance;

            PacketHandler* packetHandler;

            std::vector<Host*>* hosts;
            unsigned long netEventCounter;

            IPreparedNetEventListener* pneListener;

            const Tins::AddressRange<Tins::IPv6Address> subnetIPv6LinkLocal = Tins::IPv6Address("fe80::") / 10;

            Tins::AddressRange<Tins::IPv6Address> subnetIPv6UniqueLocal = Tins::IPv6Address("fd00::") / 8;
            Tins::AddressRange<Tins::IPv4Address> subnetIPv4 = Tins::IPv4Address("0.0.0.0") / 32;

            bool isPaused;

            Host* hostLocalInterface;

            std::vector<DNSAnswer*>* dnsAnswers;
    };

} // namespace Netvisix
#endif // NETEVENTMANAGER_H
