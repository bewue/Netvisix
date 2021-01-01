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

#include "VisiblePacket.h"
#include "VisibleHost.h"


namespace Netvisix {

    VisiblePacket::VisiblePacket(NetView* netView, VisibleHost* sender, VisibleHost* receiver, NetEvent* netEvent) : VisibleBase(netView) {
        this->sender = sender;
        this->receiver = receiver;
        this->netEvent = netEvent;

        speed = 0.4f;

        color = VisiblePacket::getPacketColor(netEvent->getTopLevelProtocol());
        brush = QBrush(color, Qt::BrushStyle::SolidPattern);
        brushIPv6 = QBrush(Qt::black, Qt::BrushStyle::SolidPattern);

        setRadius(5);
        setPosition(sender->getPosition());
        sender->showHostAliveEffect();
        targetPos = receiver->getPosition();

        equalPacketsInBuffer = 0;
        vectorToTarget = getVectorToTarget();
    }

    VisiblePacket::~VisiblePacket() {
        delete netEvent;
    }

    bool VisiblePacket::equals(VisiblePacket *vp) {
        if (receiver == vp->getReceiver() && sender == vp->getSender() && netEvent->getTopLevelProtocol() == vp->getNetEvent()->getTopLevelProtocol()) {
            return true;
        }
        else {
            return false;
        }
    }

    QColor VisiblePacket::getPacketColor(Protocol protocol) {
        switch (protocol) {
            case Protocol::EthernetII:
                return Qt::white;
            case Protocol::IPv4:
                return Qt::white;
            case Protocol::IPv6:
                return Qt::white;

            case Protocol::OtherL2:
                return QColor(240, 240, 240, 255);
            case Protocol::OtherL3:
                return Qt::white;

            case Protocol::ARP:
                return Qt::black;
            case Protocol::ICMP:
                return QColor(210, 210, 0, 255);
            case Protocol::ICMPv6:
                return QColor(210, 210, 0, 255);

            case Protocol::TCP:
                return QColor(0, 100, 200, 255);
            case Protocol::UDP:
                return QColor(20, 120, 20, 255);

            default:
                return Qt::white;
        }
    }

    void VisiblePacket::onVisibleUpdate(quint64 dt) {
        VisibleBase::onVisibleUpdate(dt);

        targetPos = receiver->getPosition();
    }

     void VisiblePacket::onVisiblePaint(QPainter& painter) {
        VisibleBase::onVisiblePaint(painter);

        if (netEvent->isIPv6()) {
            painter.setBrush(brushIPv6);
            painter.drawEllipse(getPosition(), getRadius() * 0.3f, getRadius() * 0.3f);
        }

        if (equalPacketsInBuffer > 0) {
            float r = getRadius();
            painter.setBrush(brush);
            painter.drawEllipse(getPosition() + vectorToTarget * 4, r, r);

            if (equalPacketsInBuffer > 1) {
                painter.drawEllipse(getPosition() + vectorToTarget * 8, r, r);
            }
        }
     }

     void VisiblePacket::onReachedTargetPosition() {
        receiver->showHostAliveEffect();
        setToDelete(true);
     }

} // namespace Netvisix
