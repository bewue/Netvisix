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

#include "PacketHandler.h"
#include "Net/NetEvent.h"
#include "Net/NetEventManager.h"

#include <tins.h>


namespace Netvisix {


    PacketHandler::PacketHandler(std::string interfaceName, bool promiscMode) {
        Tins::SnifferConfiguration config;
        config.set_promisc_mode(promiscMode);
        sniffer = new Tins::Sniffer(interfaceName, config);
    }

    PacketHandler::~PacketHandler() {
        delete sniffer;
    }

    void PacketHandler::destroy() {
        sniffer->stop_sniff();
        terminate(); // critical call
        delete this;
    }

    void PacketHandler::run() {
        sniffer->sniff_loop(callback);
    }

    bool PacketHandler::callback(const Tins::PDU &pdu) {
        NetEventManager* nm = NetEventManager::SharedInstance();
        if (nm->getIsPaused()) {
            return true;
        }

        NetEvent* ne = new NetEvent();

        const Tins::EthernetII* pduETH = nullptr;
        const Tins::IP* pduIPv4 = nullptr;
        const Tins::IPv6* pduIPv6 = nullptr;
        const Tins::UDP* pduUDP = nullptr;
        const Tins::TCP* pduTCP = nullptr;

        ne->size = pdu.size();
        if ((pduETH = pdu.find_pdu<Tins::EthernetII>())) {
            ne->l1Protocol = Protocol::EthernetII;
            ne->srcAddrHW = pduETH->src_addr();
            ne->dstAddrHW = pduETH->dst_addr();

            // IPv4 or IPv6
            pduIPv4 = nullptr;
            pduIPv6 = nullptr;
            if ((pduIPv4 = pduETH->find_pdu<Tins::IP>())) {
                ne->l2Protocol = Protocol::IPv4;
                ne->srcAddrIPv4 = pduIPv4->src_addr();
                ne->dstAddrIPv4 = pduIPv4->dst_addr();
            }
            else if ((pduIPv6 = pduETH->find_pdu<Tins::IPv6>())) {
                ne->l2Protocol = Protocol::IPv6;
                ne->srcAddrIPv6 = pduIPv6->src_addr();
                ne->dstAddrIPv6 = pduIPv6->dst_addr();
            }

            if (pduIPv4 || pduIPv6) {

                // TCP
                if ((pduTCP = pduETH->find_pdu<Tins::TCP>())) {
                    ne->l3Protocol = Protocol::TCP;
                }

                // UDP
                else if ((pduUDP = pduETH->find_pdu<Tins::UDP>())) {
                    ne->l3Protocol = Protocol::UDP;

                    // DNS
                    if (pduUDP->sport() == 53 || pduUDP->dport() == 53) {
                        Tins::DNS pduDNS = pduUDP->rfind_pdu<Tins::RawPDU>().to<Tins::DNS>();
                        if (pduDNS.answers_count() > 0) {
                            std::string hostname = pduDNS.answers().front().dname();
                            for (const auto &answer : pduDNS.answers()) {
                                nm->handleDNSAnswer(answer.data(), hostname);
                            }
                        }
                    }
                }
                // ICMP
                else if (pduETH->find_pdu<Tins::ICMP>()) {
                    ne->l3Protocol = Protocol::ICMP;
                }
                // ICMPv6
                else if (pduETH->find_pdu<Tins::ICMPv6>()) {
                    ne->l3Protocol = Protocol::ICMPv6;
                }
                else {
                    ne->l3Protocol = Protocol::OtherL3;
                }
            }

            // ARP
            else if (pdu.find_pdu<Tins::ARP>()) {
                ne->l2Protocol = Protocol::ARP;
            }
            // ?
            else {
                ne->l2Protocol = Protocol::OtherL2;
            }
        }
        // (no ethernet II frame)
        else {
            ne->l1Protocol = Protocol::Unkown;
        }

        nm->handleNetEvent(ne);

        return true;
    }

} // namespace Netvisix
