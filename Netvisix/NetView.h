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

#ifndef NETVIEW_H
#define NETVIEW_H

#include "Net/IPreparedNetEventListener.h"

#include <QWidget>
#include <QHostInfo>

#include <mutex>


namespace Netvisix {

    class VisibleBase;
    class VisibleHost;
    class VisiblePacket;
    enum class NetArea;
    class HostInfoPopup;
    class VisibleLink;

    class NetView : public QWidget, public IPreparedNetEventListener {

        public:
            NetView(QWidget *parent = 0);
            ~NetView();

            void onUpdate(quint64 dt);

            bool isHostColliding(VisibleHost* vHost, QPointF targetPos, float radius);
            QPointF getRandomPosition(NetArea netArea, float radius);
            QPointF getFreeHostPosition(VisibleHost* vHost);

            VisibleHost* getVisibleHost(Host* host);

            virtual void onPreparedNetEventNewHost(Host* newHost);
            virtual void onPreparedNetEventNewUnicastPacket(Host* sender, Host* receiver, NetEvent* netEvent);
            virtual void onPreparedNetEventNewMulticastPacket(Host* sender, NetEvent* netEvent);

            virtual void onHostAddrUpdate(Host* host) {}

            static float getRandF() {
                int r = rand() % 10000;
                return (float) r / 10000;
            }

            unsigned int getActivePacketsCount() { return this->visiblePackets->size(); }
            unsigned int getActiveHostsCount() { return this->visibleHosts->size(); }

            bool getIsPaused() { return this->isPaused; }
            void setIsPaused(bool paused) { this->isPaused = paused; }
            void reset();

            void setReverseDNSLookupEnabled(bool enabled);
            bool getReverseDNSLookupEnabled() { return this->reverseDNSLookupEnabled; }

        private slots:
            void onReverseDNSLookupAnswer(QHostInfo hostInfo);

        protected:
            void mousePressEvent(QMouseEvent *event);

            void paintEvent(QPaintEvent *event);

            void resizeEvent(QResizeEvent * event);

        private:
            Q_OBJECT

            void updateLanAreaRect();
            void updateHostPositions();

            VisibleHost* getHostAtPosition(QPointF pos, bool visibleOnly);
            void handleHostInfoPopup(quint64 dt);

            void showLink(VisibleHost* sender, VisibleHost* receiver);

            void updatePacketBuffer(quint64 dt);
            void setBufferPacketStates();

            void reverseDNSLookup(Host* host);

            std::vector<VisibleHost*>* visibleHosts;
            std::vector<VisiblePacket*>* visiblePackets;

            std::vector<VisiblePacket*>* visiblePacketsBuffer;
            float packetBufferTimer;

            std::vector<VisibleLink*>* visibleLinks;

            bool isPaused;

            std::mutex mutex;

            QRectF lanAreaRect;
            QRectF lanAreaInnerRect;
            QBrush lanAreaBrush;

            float resizeEventTimer;
            bool wasResized;

            HostInfoPopup* hostInfoPopup;

            bool reverseDNSLookupEnabled;
        };

} // namespace Netvisix
#endif // NETVIEW_H
