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

#include "NetView.h"
#include "Net/NetUtil.h"
#include "Net/NetEventManager.h"
#include "Net/Host.h"
#include "GUI/VisibleHost.h"
#include "GUI/VisiblePacket.h"
#include "GUI/HostInfoPopup.h"
#include "GUI/VisibleLink.h"

#include <QtWidgets>
#include <QtDebug>


namespace Netvisix {

    NetView::NetView(QWidget *parent) : QWidget(parent) {
        QPalette Pal(palette());
        Pal.setColor(QPalette::Background, QColor(160, 160, 160, 255));
        setAutoFillBackground(true);
        setPalette(Pal);

        lanAreaBrush = QBrush(QColor(170, 170, 170, 255), Qt::BrushStyle::SolidPattern);
        updateLanAreaRect();

        visibleHosts = new std::vector<VisibleHost*>();
        visiblePackets = new std::vector<VisiblePacket*>();

        visiblePacketsBuffer = new std::vector<VisiblePacket*>();
        packetBufferTimer = 0;

        int linkInstancesMax = 40;
        visibleLinks = new std::vector<VisibleLink*>();
        for (int i = 0; i < linkInstancesMax; i++) {
            visibleLinks->push_back(new VisibleLink(this));
        }

        NetEventManager::SharedInstance()->addPreparedNetEventListener(this);

        hostInfoPopup = nullptr;

        reverseDNSLookupEnabled = false;

        setIsPaused(false);
    }

    NetView::~NetView() {
        reset();

        delete visibleHosts;
        delete visiblePackets;
        delete visiblePacketsBuffer;

        for (unsigned int i = 0; i < visibleLinks->size(); i++) {
            delete visibleLinks->at(i);
        }
        delete visibleLinks;
    }

    void NetView::reset() {
        mutex.lock();
        for (unsigned int i = 0; i < visibleHosts->size(); i++) {
            delete visibleHosts->at(i);
        }
        visibleHosts->clear();

        for (unsigned int i = 0; i < visiblePackets->size(); i++) {
            delete visiblePackets->at(i);
        }
        visiblePackets->clear();

        for (unsigned int i = 0; i < visiblePacketsBuffer->size(); i++) {
            delete visiblePacketsBuffer->at(i);
        }
        visiblePacketsBuffer->clear();

        for (unsigned int i = 0; i < visibleLinks->size(); i++) {
            VisibleLink* vLink = visibleLinks->at(i);
            vLink->setIsActive(false);
        }

        setIsPaused(false);
        mutex.unlock();
    }

    VisibleHost* NetView::getVisibleHost(Host *host) {
        for (unsigned int i = 0; i < visibleHosts->size(); i++) {
            VisibleHost* vHost = visibleHosts->at(i);
            if (vHost->getHost() == host) {
                return vHost;
            }
        }

        return nullptr;
    }

    void NetView::onPreparedNetEventNewHost(Host* newHost) {
        VisibleHost* newVH = new VisibleHost(this, newHost);

        if (reverseDNSLookupEnabled && newHost->getHostname() == "") {
            reverseDNSLookup(newHost);
        }

        mutex.lock();
        visibleHosts->push_back(newVH);
        mutex.unlock();
    }

    void NetView::onPreparedNetEventNewUnicastPacket(Host* sender, Host* receiver, NetEvent* netEvent) {
        VisibleHost* vhSender = getVisibleHost(sender);
        VisibleHost* vhReceiver = getVisibleHost(receiver);

        VisiblePacket* newVP = new VisiblePacket(this, vhSender, vhReceiver, netEvent);
        mutex.lock();
        visiblePacketsBuffer->push_back(newVP);
        mutex.unlock();
    }

    void NetView::onPreparedNetEventNewMulticastPacket(Host* sender, NetEvent* netEvent) {
        VisibleHost* vhSender = getVisibleHost(sender);
        if (vhSender == nullptr) {
            qDebug() << "Missing VisibleHost!!!";
            return;
        }

        vhSender->showMulticastEffect(netEvent);
        vhSender->showHostAliveEffect();
    }

    void NetView::setReverseDNSLookupEnabled(bool enabled) {
        reverseDNSLookupEnabled = enabled;
        if (enabled) {
            for (unsigned int i = 0; i < visibleHosts->size(); i++) {
                VisibleHost* v = visibleHosts->at(i);
                if (v->getHost()->getHostname() == "") {
                    reverseDNSLookup(v->getHost());
                }
            }
        }
    }

    void NetView::reverseDNSLookup(Host* host) {
        if (host->getAddrListIPv4().size() > 0) {
            QHostInfo::lookupHost(host->getAddrListIPv4().at(0).to_string().c_str(), this, SLOT(onReverseDNSLookupAnswer(QHostInfo)));
        } else if (host->getAddrListIPv6().size() > 0) {
            QHostInfo::lookupHost(host->getAddrListIPv6().at(0).to_string().c_str(), this, SLOT(onReverseDNSLookupAnswer(QHostInfo)));
        }
    }

    void NetView::onReverseDNSLookupAnswer(QHostInfo hostInfo) {
        if (hostInfo.error() != QHostInfo::NoError || hostInfo.addresses().empty()) {
            return;
        }

        std::string queryIP = hostInfo.addresses().at(0).toString().toStdString();

        for (unsigned int i = 0; i < visibleHosts->size(); i++) {
            VisibleHost* v = visibleHosts->at(i);

            if (v->getHost()->getAddrIPv4(queryIP) != NetUtil::zeroAddrIPv4
                    || v->getHost()->getAddrIPv6(queryIP) != NetUtil::zeroAddrIPv6) {

                std::string hostName = hostInfo.hostName().toStdString();

                if (v->getHost()->getAddrIPv4(hostName) == NetUtil::zeroAddrIPv4
                        && v->getHost()->getAddrIPv6(hostName) == NetUtil::zeroAddrIPv6) {
                    v->getHost()->setHostname(hostName, true);
                    v->getHost()->setHostnameIsFromReverseDNSLookp(true);
                }

                break;
            }
        }
    }

    void NetView::mousePressEvent(QMouseEvent *event) {
        QPoint mousePos = mapFromGlobal(QCursor::pos());

        VisibleHost* vHost = getHostAtPosition(mousePos, true);
        if (vHost != nullptr) {
            bool marked = !vHost->getMarked();
            vHost->setMarked(marked);
        }
    }

    VisibleHost* NetView::getHostAtPosition(QPointF pos, bool visibleOnly) {
        for (unsigned int i = 0; i < visibleHosts->size(); i++) {
            VisibleHost* v = visibleHosts->at(i);
            if (visibleOnly && ! v->getIsVisible()) {
                continue;
            }

            double diffX = v->getPosition().x() - pos.x();
            double diffY = v->getPosition().y() - pos.y();
            double length = std::sqrt(diffX * diffX + diffY * diffY);

            if (length <= v->getRadius()) {
                return v;
            }
        }

        return nullptr;
    }

    bool NetView::isHostColliding(VisibleHost* vHost, QPointF targetPos, float radius) {
        // (checking target positions)
        for (unsigned int i = 0; i < visibleHosts->size(); i++) {
            VisibleHost* v = visibleHosts->at(i);
            if (v == vHost) {
                continue;
            }

            float vRadius = v->getRadius();
            if (v->getHost()->getNetArea() == NetArea::LOCAL_INTERFACE) {
                vRadius *= 5.0f;
            }

            QRectF vRect = QRectF(v->getTargetPosition().x() - vRadius, v->getTargetPosition().y() - vRadius, vRadius * 2, vRadius * 2);
            QRectF targetRect = QRectF(targetPos.x() - radius, targetPos.y() - radius, radius * 2, radius * 2);

            if (vRect.intersects(targetRect)) {
                return true;
            }
        }

        return false;
    }

    QPointF NetView::getFreeHostPosition(VisibleHost* vHost) {
        int roundsMax = 500;
        float radiusMin = vHost->getRadius();
        float radiusMax = radiusMin * 10.0f;
        NetArea netArea = vHost->getHost()->getNetArea();

        if (netArea == NetArea::LOCAL_INTERFACE) {
            return QPointF(size().width() * 0.5f, size().height() * 0.5f);
        }

        QPointF newPos;
        for (int i = 0; i < roundsMax; i++) {
            float radius = radiusMin + ((radiusMax - radiusMin) * (1.0f - float (i / (float) roundsMax)));
            newPos = getRandomPosition(netArea, vHost->getRadius());
            if (isHostColliding(vHost, newPos, radius) == false) {
                break;
            }
        }

        return newPos;
    }

    QPointF NetView::getRandomPosition(NetArea netArea, float radius) {
        if (netArea == NetArea::SUBNET) {
            float randX = lanAreaInnerRect.left() + (lanAreaInnerRect.width() - radius * 2) * getRandF() + radius;
            float randY = lanAreaInnerRect.top() + (lanAreaInnerRect.height() - radius * 2) * getRandF() + radius;
            return QPointF(randX, randY);
        }
        else {
            float randVRect = getRandF();
            float randMirror = getRandF();
            float randX = 0;
            float randY = 0;

            if (randVRect <= 0.65f) {
                // top rect (full width)
                randX = (size().width() - radius * 2) * getRandF() + radius;
                randY = (lanAreaRect.top() - radius * 2) * getRandF() + radius;
                if (randMirror <= 0.5f) {
                    // bottom rect (full width)
                    randY += lanAreaRect.bottom();
                }
            }
            else {
                // left center rect
                randX = (lanAreaRect.left() - radius * 2) * getRandF() + radius;
                randY = (lanAreaRect.height() - radius * 2) * getRandF() + radius + lanAreaRect.top();
                if (randMirror <= 0.5f) {
                    // right center rect
                    randX += lanAreaRect.right();
                }
            }

            return QPointF(randX, randY);
        }
    }

    void NetView::updateLanAreaRect() {
        float width = size().width() * 0.55f;
        float height = size().height() * 0.55f;
        lanAreaRect = QRectF(size().width() / 2 - width / 2, size().height() / 2 - height / 2, width, height);

        float innerWidth = (width / 2) * std::sqrt(2);
        float innerHeight = (height/ 2) * std::sqrt(2);
        lanAreaInnerRect = QRectF(size().width() / 2 - innerWidth / 2, size().height() / 2 - innerHeight / 2, innerWidth, innerHeight);
    }

    void NetView::resizeEvent(QResizeEvent *event) {
        updateLanAreaRect();
        wasResized = true;
        resizeEventTimer = 0;
    }

    void NetView::updateHostPositions() {
        for (unsigned int i = 0; i < visibleHosts->size(); i++) {
            VisibleHost* v = visibleHosts->at(i);
            v->setTargetPosition(getFreeHostPosition(v));
        }
    }

    void NetView::handleHostInfoPopup(quint64 dt) {
        QPoint mousePos = mapFromGlobal(QCursor::pos());

        if (hostInfoPopup == nullptr) {
            VisibleHost* vHost = getHostAtPosition(mousePos, true);
            if (vHost != nullptr) {
                hostInfoPopup = new HostInfoPopup(vHost->getHost(), (MainWindow*) parentWidget()->parentWidget());
                hostInfoPopup->setPositionOnVisibleHost(vHost->getPosition().toPoint());
                hostInfoPopup->show();
            }
        }
        else {
            VisibleHost* vHost = getHostAtPosition(mousePos, false);
            if (hostInfoPopup->geometry().contains(mousePos) == false && (vHost == nullptr || vHost != getVisibleHost(hostInfoPopup->getHost()))) {
                hostInfoPopup->close();
                hostInfoPopup = nullptr;
            }
        }
    }

    void NetView::showLink(VisibleHost *sender, VisibleHost *receiver) {
        VisibleLink* inactiveLink = nullptr;
        unsigned int visibleLinksCount = visibleLinks->size();

        for (unsigned int i = 0; i < visibleLinksCount; i++) {
            VisibleLink* link = visibleLinks->at(i);

            if (link->getIsActive()) {
                if (link->equals(sender, receiver)) {
                    link->showLink(sender, receiver);
                    return;
                }
            }
            else if (inactiveLink == nullptr) {
                inactiveLink = link;
            }
        }

        if (inactiveLink != nullptr) {
            inactiveLink->showLink(sender, receiver);
        }
    }

    void NetView::setBufferPacketStates() {
        unsigned int packetBufferSize = visiblePacketsBuffer->size();
        for (unsigned int i = 0; i < packetBufferSize; i++) {
            VisiblePacket* vp1 = visiblePacketsBuffer->at(i);

            if (vp1->getToDelete()) {
                continue;
            }

            for (unsigned int k = 0; k < packetBufferSize; k++) {
                VisiblePacket* vp2 = visiblePacketsBuffer->at(k);

                if (vp1 == vp2 || vp2->getToDelete()) {
                    continue;
                }

                if (vp1->equals(vp2)) {
                    vp1->setEqualPacketsInBuffer(vp1->getEqualPacketsInBuffer() + 1);
                    vp2->setToDelete(true);
                }
            }
        }
    }

    void NetView::updatePacketBuffer(quint64 dt) {
        packetBufferTimer += dt;

        if (packetBufferTimer >= 50) {
            mutex.lock();
            setBufferPacketStates();

            for (auto it = visiblePacketsBuffer->begin(); it != visiblePacketsBuffer->end();) {
                VisiblePacket* vp = (*it);
                if (vp->getToDelete()) {
                    delete *it;
                    it = visiblePacketsBuffer->erase(it);
                }
                else {
                    visiblePackets->push_back(vp);
                    showLink(vp->getSender(), vp->getReceiver());
                    ++it;
                }
            }

            visiblePacketsBuffer->clear();
            mutex.unlock();

            packetBufferTimer = 0;
        }
    }

    void NetView::onUpdate(quint64 dt) {
        // handle resize event (host repositioning)
        if (wasResized) {
            resizeEventTimer += dt;
            if (resizeEventTimer >= 500) {
                updateHostPositions();
                resizeEventTimer = 0;
                wasResized = false;
            }
        }

        handleHostInfoPopup(dt);

        if (isPaused) {
            return;
        }

        updatePacketBuffer(dt);

        mutex.lock();
        // visibleLinks
        unsigned int visibleLinksCount = visibleLinks->size();
        for (unsigned int i = 0; i < visibleLinksCount; i++) {
            VisibleLink* link = visibleLinks->at(i);
            if (link->getIsActive()) {
                link->onVisibleUpdate(dt);
            }
        }

        // visibleHosts
        unsigned int visibleHostsCount = visibleHosts->size();
        for (unsigned int i = 0; i < visibleHostsCount; i++) {
            VisibleHost* v = visibleHosts->at(i);
            v->onVisibleUpdate(dt);
        }

        for (auto it = visiblePackets->begin(); it != visiblePackets->end();) {
            (*it)->onVisibleUpdate(dt);

            if ((*it)->getToDelete()) {
                delete *it;
                it = visiblePackets->erase(it);
            }
            else {
                ++it;
            }
        }
        mutex.unlock();

        update();
    }

    void NetView::paintEvent(QPaintEvent *event) {
        QPainter painter(this);

        painter.setPen(QColor(180, 180, 180, 255));

        // lan area
        painter.setBrush(lanAreaBrush);
        painter.drawEllipse(lanAreaRect);

        mutex.lock();
        // visibleLinks
        unsigned int visibleLinksCount = visibleLinks->size();
        for (unsigned int i = 0; i < visibleLinksCount; i++) {
            VisibleLink* link = visibleLinks->at(i);
            if (link->getIsActive()) {
                link->onVisiblePaint(painter);
            }
        }

        painter.setPen(QColor(80, 80, 80, 255));

        // visiblePackets
        unsigned int visiblePacketsCount = visiblePackets->size();
        for (unsigned int i = 0; i < visiblePacketsCount; i++) {
            VisiblePacket* v = visiblePackets->at(i);
             v->onVisiblePaint(painter);
        }

        painter.setPen(QColor(100, 100, 100, 255));

        // visibleHosts
        unsigned int visibleHostsCount = visibleHosts->size();
        for (unsigned int i = 0; i < visibleHostsCount; i++) {
            VisibleHost* v = visibleHosts->at(i);
            v->onVisiblePaint(painter);
        }
        mutex.unlock();
    }

} // namespace Netvisix
