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

#include "NetEventManager.h"
#include "Net/NetEvent.h"
#include "Net/IPreparedNetEventListener.h"
#include "Net/Host.h"
#include "Net/PacketHandler.h"
#include "Net/NetUtil.h"
#include "Net/NetStatistic.h"


namespace Netvisix {

    NetEventManager* NetEventManager::instance = 0;

    NetEventManager* NetEventManager::SharedInstance() {
        if (instance == 0) {
            instance = new NetEventManager();
        }

        return instance;
    }

    NetEventManager::NetEventManager() {
        hosts = new std::vector<Host*>();
        packetHandler = nullptr;
        netEventCounter = 0;
        hostLocalInterface = nullptr;
        dnsAnswers = new std::vector<DNSAnswer*>();
        setIsPaused(false);
    }

    NetEventManager::~NetEventManager() {
        reset();
        delete hosts;
        delete dnsAnswers;
        if (hostLocalInterface != nullptr) {
            delete hostLocalInterface;
        }
    }

    void NetEventManager::release() {
        if (instance != nullptr) {
            delete instance;
            instance = nullptr;
        }
    }

    void NetEventManager::startSniffing(std::string interfaceName, bool promiscMode, std::string subnetIPv4, std::string subnetIPv6) {
        stopSniffing();

        initHostLocalInterface(interfaceName);

        this->subnetIPv4 = NetUtil::getSubnetRangeIPv4(subnetIPv4);
        this->subnetIPv6UniqueLocal = NetUtil::getSubnetRangeIPv6(subnetIPv6);

        packetHandler = new PacketHandler(interfaceName, promiscMode);
        packetHandler->start();
        netEventCounter = 0;
    }

    void NetEventManager::stopSniffing() {
        if (packetHandler != nullptr) {
            packetHandler->destroy();
            packetHandler = nullptr;
        }
    }

    bool NetEventManager::getIsSniffingRunning() {
        return (packetHandler == nullptr) ? false : true;
    }

    void NetEventManager::reset() {
        stopSniffing();

        for (unsigned int i = 0; i < hosts->size(); i++) {
            delete hosts->at(i);
        }
        hosts->clear();

        for (unsigned int i = 0; i < dnsAnswers->size(); i++) {
            delete dnsAnswers->at(i);
        }
        dnsAnswers->clear();

        netEventCounter = 0;
        setIsPaused(false);
    }

    void NetEventManager::initHostLocalInterface(std::string interfaceName) {
        if (hostLocalInterface != nullptr) {
            delete hostLocalInterface;
            hostLocalInterface = nullptr;
        }

        std::vector<Tins::NetworkInterface> ifs = Tins::NetworkInterface::all();
        for (unsigned int i = 0; i < ifs.size(); i++) {
            Tins::NetworkInterface ni = ifs.at(i);
            if (ni.name() == interfaceName) {
                Tins::NetworkInterface::Info info = ni.addresses();
                hostLocalInterface = new Host();
                hostLocalInterface->addrHW = info.hw_addr;
                hostLocalInterface->addAddrIPv4(info.ip_addr);
                return;
            }
        }
    }

    bool NetEventManager::getIsLocalInterface(Host* h) {
        if (hostLocalInterface == nullptr || h == nullptr) {
            return false;
        }

        return (h->addrHW != NetUtil::zeroAddrHW && h->addrHW == hostLocalInterface->addrHW);
    }

    void NetEventManager::handleNetAreaSubnet(Host* host) {
        if (getIsLocalInterface(host)) {
            host->setNetArea(NetArea::LOCAL_INTERFACE);
        }
        else {
            host->setNetArea(NetArea::SUBNET);
        }
    }

    void NetEventManager::handleDNSAnswer(std::string ipAddr, std::string hostname) {
        if (ipAddr.empty() || hostname.empty()) {
            return;
        }

        removeOldDNSAnswers();

        for (unsigned int i = 0; i < dnsAnswers->size(); i++) {
            DNSAnswer* dnsAnswer = dnsAnswers->at(i);
            if (dnsAnswer->ipAddr == ipAddr) {
                return;
            }
        }

        // create new entry
        DNSAnswer* dnsAnswer = new DNSAnswer();
        dnsAnswer->ipAddr = ipAddr;
        dnsAnswer->hostname = hostname;
        dnsAnswer->timeStamp = std::clock();
        dnsAnswers->push_back(dnsAnswer);
    }

    void NetEventManager::removeOldDNSAnswers() {
        const float maxAge = 5.0f;
        std::clock_t currentTime = std::clock();

        for (auto it = dnsAnswers->begin(); it != dnsAnswers->end();) {
            DNSAnswer* dnsAnswer = (*it);
            float age = (float) (currentTime - dnsAnswer->timeStamp) / CLOCKS_PER_SEC;

            if (age >= maxAge) {
                delete *it;
                it = dnsAnswers->erase(it);
            }
            else {
                ++it;
            }
        }
    }

    void NetEventManager::setHostnameFromDNSAnswers(Host* host) {
        if (host->getAddrListIPv4().empty() && host->getAddrListIPv6().empty()) {
            return;
        }

        removeOldDNSAnswers();

        for (unsigned int i = 0; i < dnsAnswers->size(); i++) {
            DNSAnswer* dnsAnswer = dnsAnswers->at(i);

            if (host->getAddrIPv4(dnsAnswer->ipAddr) != NetUtil::zeroAddrIPv4
                    || host->getAddrIPv6(dnsAnswer->ipAddr) != NetUtil::zeroAddrIPv6) {

                host->hostname = dnsAnswer->hostname;
                break;
            }
        }
    }

    void NetEventManager::handleNetEvent(NetEvent* ne) {
//        std::cout << getDebugString(ne) << std::endl;

        handleNewHosts(ne);

        handleNewPackets(ne);

        netEventCounter++;
    }

    void NetEventManager::handleNewPackets(NetEvent* ne) {
        if (pneListener == nullptr) {
            return;
        }

        // broadcast or multicast
        if (getAddrType(ne->dstAddrHW) == AddrType::MULTICAST
                || getAddrType(ne->dstAddrIPv4) == AddrType::MULTICAST
                || getAddrType(ne->dstAddrIPv6) == AddrType::MULTICAST) {

            Host* sender = getHost(ne->srcAddrHW, ne->srcAddrIPv4, ne->srcAddrIPv6);
            if (sender != nullptr) {
                sender->statistic->handleNetEvent(TrafficDirection::TD_OUT, ne);
                pneListener->onPreparedNetEventNewMulticastPacket(sender, ne);
            }

            return;
        }

        // unicast
        // sender
        Host* sender = nullptr;
        if (ne->l2Protocol == Protocol::ARP || getIsHostInSubnet(ne->srcAddrIPv4) || getIsHostInSubnet(ne->srcAddrIPv6)) {
            sender = getHost(ne->srcAddrHW, ne->srcAddrIPv4, ne->srcAddrIPv6);
        }
        else {
            sender = getHost(NetUtil::zeroAddrHW, ne->srcAddrIPv4, ne->srcAddrIPv6);
        }
        if (sender == nullptr) {
            return;
        }
        else {
            sender->statistic->handleNetEvent(TrafficDirection::TD_OUT, ne);
        }

        // receiver
        Host* receiver = nullptr;
        if (ne->l2Protocol == Protocol::ARP || getIsHostInSubnet(ne->dstAddrIPv4) || getIsHostInSubnet(ne->dstAddrIPv6)) {
            receiver = getHost(ne->dstAddrHW, ne->dstAddrIPv4, ne->dstAddrIPv6);
        }
        else {
            receiver = getHost(NetUtil::zeroAddrHW, ne->dstAddrIPv4, ne->dstAddrIPv6);
        }
        if (receiver == nullptr) {
            return;
        }
        else {
            receiver->statistic->handleNetEvent(TrafficDirection::TD_IN, ne);
        }

        pneListener->onPreparedNetEventNewUnicastPacket(sender, receiver, ne);
    }

    void NetEventManager::addHost(Host *host) {
        setHostnameFromDNSAnswers(host);

        hosts->push_back(host);

        if (pneListener != nullptr) {
            pneListener->onPreparedNetEventNewHost(host);
        }

//        std::cout << "+++ addHost() : new host added [" << hosts->size() << "]" << std::endl;
//        for (int i = 0; i < hosts->size(); i++) {
//            Host* h = hosts->at(i);
//            std::cout << "[" << i << "] " << h->addrHW << ", " << h->addrIPv4 << ", " << h->addrIPv6 << std::endl;
//        }
    }

    Host* NetEventManager::getHost(Tins::HWAddress<6> hwAddr) {
        if (hwAddr == NetUtil::zeroAddrHW) {
            return nullptr;
        }

        for (Host* h : *hosts) {
            if (h->addrHW == hwAddr) {
                return h;
            }
        }
        return nullptr;
    }

    Host* NetEventManager::getHost(Tins::IPv4Address ipv4Addr) {
        if (ipv4Addr == NetUtil::zeroAddrIPv4) {
            return nullptr;
        }

        for (Host* h : *hosts) {
            if (h->getAddrIPv4(ipv4Addr) != NetUtil::zeroAddrIPv4) {
                return h;
            }
        }
        return nullptr;
    }

    Host* NetEventManager::getHost(Tins::IPv6Address ipv6Addr) {
        if (ipv6Addr == NetUtil::zeroAddrIPv6) {
            return nullptr;
        }

        for (Host* h : *hosts) {
            if (h->getAddrIPv6(ipv6Addr) != NetUtil::zeroAddrIPv6) {
                return h;
            }
        }
        return nullptr;
    }

    Host* NetEventManager::getHost(Tins::HWAddress<6> hwAddr, Tins::IPv4Address ipv4Addr, Tins::IPv6Address ipv6Addr) {
        Host* h = nullptr;
        if (hwAddr != NetUtil::zeroAddrHW && (h = getHost(hwAddr))) {
            return h;
        }
        else if (ipv4Addr != NetUtil::zeroAddrIPv4 && (h = getHost(ipv4Addr))) {
            return h;
        }
        else if (ipv6Addr != NetUtil::zeroAddrIPv6 && (h = getHost(ipv6Addr))) {
            return h;
        }
        else {
            return nullptr;
        }
    }

    bool NetEventManager::getIsHostInSubnet(Tins::IPv4Address ipv4Addr) {
        if (ipv4Addr == NetUtil::zeroAddrIPv4) {
            return false;
        }

        if (subnetIPv4.contains(ipv4Addr) && (*subnetIPv4.end()) != ipv4Addr) {
            return true;
        }
        else {
            return false;
        }
    }

    bool NetEventManager::getIsHostInSubnet(Tins::IPv6Address ipv6Addr) {
        if (ipv6Addr == NetUtil::zeroAddrIPv6) {
            return false;
        }
        // unique local unicast (editable by user (as ipv4 subnet))
        if (subnetIPv6UniqueLocal.contains(ipv6Addr) && (*subnetIPv6UniqueLocal.end()) != ipv6Addr) {
            return true;
        }

        // link local address (not editable by user)
        if (subnetIPv6LinkLocal.contains(ipv6Addr) && (*subnetIPv6LinkLocal.end()) != ipv6Addr) {
            return true;
        }

        return false;
    }

    Tins::IPv4Address NetEventManager::getSubnetBroadcastAddrIPv4() {
        return (*subnetIPv4.end());
    }

    void NetEventManager::handleNewHosts(NetEvent *ne) {
        if (ne->l1Protocol != Protocol::ETHERNETII) {
            return;
        }

        // arp
        if (ne->l2Protocol == Protocol::ARP) {
            Host* h;
            h = getHost(ne->srcAddrHW);
            if (h == nullptr && getAddrType(ne->srcAddrHW) == AddrType::UNICAST) {
                Host* nHost = new Host();
                nHost->addrHW = ne->srcAddrHW;
                handleNetAreaSubnet(nHost);
                addHost(nHost);
            }

            h = getHost(ne->dstAddrHW);
            if (h == nullptr && getAddrType(ne->dstAddrHW) == AddrType::UNICAST) {
                Host* nHost = new Host();
                nHost->addrHW = ne->dstAddrHW;
                handleNetAreaSubnet(nHost);
                addHost(nHost);
            }

            return;
        }

        // ip
        if (ne->isIPv4()) {
            checkNewHost(ne->srcAddrHW, ne->srcAddrIPv4);
            checkNewHost(ne->dstAddrHW, ne->dstAddrIPv4);
        }
        else if (ne->isIPv6()) {
            checkNewHost(ne->srcAddrHW, ne->srcAddrIPv6);
            checkNewHost(ne->dstAddrHW, ne->dstAddrIPv6);
        }
    }

    void NetEventManager::checkNewHost(Tins::HWAddress<6> hwAddr, Tins::IPv4Address ipv4Addr) {
        AddrType addrTypeHW = getAddrType(hwAddr);
        AddrType addrTypeIP = getAddrType(ipv4Addr);
        if (addrTypeHW != AddrType::UNICAST && addrTypeIP != AddrType::UNICAST) {
            return;
        }

        Host* hIP = getHost(ipv4Addr);
        if (hIP == nullptr) {
            if (getIsHostInSubnet(ipv4Addr)) {
                Host* hHW = getHost(hwAddr);
                if (hHW == nullptr) {
                    // in lan / ip and hw not in list / add new host
                    Host* nHost = new Host();
                    if (addrTypeIP == AddrType::UNICAST) {
                        nHost->addAddrIPv4(ipv4Addr);
                    }
                    if (addrTypeHW == AddrType::UNICAST) {
                        nHost->addrHW = hwAddr;
                    }
                    handleNetAreaSubnet(nHost);
                    addHost(nHost);
                }
                else {
                    // in lan / hw in list / add ip
                    if (addrTypeIP == AddrType::UNICAST) {
                        hHW->addAddrIPv4(ipv4Addr);
                    }
                }
            }
            else if (addrTypeIP == AddrType::UNICAST) {
                // not in lan / ip not in list (do not handle hw) / add new host
                Host* nHost = new Host();
                nHost->addAddrIPv4(ipv4Addr);
                nHost->setNetArea(NetArea::OUTSIDE_SUBNET);
                addHost(nHost);
            }
        }
    }

    void NetEventManager::checkNewHost(Tins::HWAddress<6> hwAddr, Tins::IPv6Address ipv6Addr) {
        AddrType addrTypeHW = getAddrType(hwAddr);
        AddrType addrTypeIP = getAddrType(ipv6Addr);
        if (addrTypeHW != AddrType::UNICAST && addrTypeIP != AddrType::UNICAST) {
            return;
        }

        Host* hIP = getHost(ipv6Addr);
        if (hIP == nullptr) {
            if (getIsHostInSubnet(ipv6Addr)) {
                Host* hHW = getHost(hwAddr);
                if (hHW == nullptr) {
                    // in lan / ip and hw not in list / add new host
                    Host* nHost = new Host();
                    if (addrTypeIP == AddrType::UNICAST) {
                        nHost->addAddrIPv6(ipv6Addr);
                    }
                    if (addrTypeHW == AddrType::UNICAST) {
                        nHost->addrHW = hwAddr;
                    }
                    handleNetAreaSubnet(nHost);
                    addHost(nHost);
                }
                else {
                    // in lan / hw in list / add ip
                    if (addrTypeIP == AddrType::UNICAST) {
                        hHW->addAddrIPv6(ipv6Addr);
                    }
                }
            }
            else if (addrTypeIP == AddrType::UNICAST) {
                // not in lan / ip not in list (do not handle hw) / add new host
                Host* nHost = new Host();
                nHost->addAddrIPv6(ipv6Addr);
                nHost->setNetArea(NetArea::OUTSIDE_SUBNET);
                addHost(nHost);
            }
        }
    }

    AddrType NetEventManager::getAddrType(Tins::HWAddress<6> hwAddr) {
        if (hwAddr == NetUtil::zeroAddrHW) {
            return AddrType::UNKOWN;
        }

        if (hwAddr.is_unicast()) {
            return AddrType::UNICAST;
        }
        else {
            return AddrType::MULTICAST;
        }
    }

    AddrType NetEventManager::getAddrType(Tins::IPv4Address ipv4Addr) {
        if (ipv4Addr == NetUtil::zeroAddrIPv4) {
            return AddrType::UNKOWN;
        }

        if (ipv4Addr.is_broadcast() || ipv4Addr.is_multicast() || ipv4Addr == getSubnetBroadcastAddrIPv4()) {
            return AddrType::MULTICAST;
        }
        else {
            return AddrType::UNICAST;
        }
    }

    AddrType NetEventManager::getAddrType(Tins::IPv6Address ipv6Addr) {
        if (ipv6Addr == NetUtil::zeroAddrIPv6) {
            return AddrType::UNKOWN;
        }

        if (ipv6Addr.is_multicast()) {
            return AddrType::MULTICAST;
        }
        else {
            return AddrType::UNICAST;
        }
    }

    std::string NetEventManager::getDebugString(NetEvent* netEvent) {
        std::string s = "[" + std::to_string(netEventCounter) + "]"
                  + "[" + std::to_string(netEvent->size) + "]" + "   \t"

                  + "[" + NetEvent::getProtocolString(netEvent->l1Protocol)
                  + ":" + NetEvent::getProtocolString(netEvent->l2Protocol)
                  + ":" + NetEvent::getProtocolString(netEvent->l3Protocol) + "]\t\t";

        if (netEvent->l1Protocol != Protocol::UNKOWN) {
            s += "[" + netEvent->srcAddrHW.to_string() + " > " + netEvent->dstAddrHW.to_string() + "]  ";

            if (netEvent->l2Protocol != Protocol::ARP && netEvent->l2Protocol != Protocol::UNKOWN) {
                if (netEvent->l2Protocol == Protocol::IPv6 || netEvent->l2Protocol == Protocol::ICMPv6) {
                    s += "[" + netEvent->srcAddrIPv6.to_string() + " > " + netEvent->dstAddrIPv6.to_string() + "]  ";
                }
                else {
                  s += "[" + netEvent->srcAddrIPv4.to_string() + " > " + netEvent->dstAddrIPv4.to_string() + "]  ";
                }
            }
        }

        return s;
    }

} // namespace Netvisix
