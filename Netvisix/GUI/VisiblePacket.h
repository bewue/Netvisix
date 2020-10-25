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

#ifndef VISIBLEPACKET_H
#define VISIBLEPACKET_H

#include "GUI/VisibleBase.h"
#include "Net/NetEvent.h"


namespace Netvisix {

    class VisibleHost;

    class VisiblePacket : public VisibleBase {

        public:
            VisiblePacket(NetView* netView, VisibleHost* sender, VisibleHost* receiver, NetEvent* netEvent);
            virtual ~VisiblePacket();

            virtual void onVisibleUpdate(quint64 dt);
            virtual void onVisiblePaint(QPainter& painter);

            virtual void onReachedTargetPosition();

            static QColor getPacketColor(Protocol protocol);

            VisibleHost* getSender() { return this->sender; }
            VisibleHost* getReceiver() { return this->receiver; }
            NetEvent* getNetEvent() { return this->netEvent; }

            bool equals(VisiblePacket* vp);

            int getEqualPacketsInBuffer() { return this->equalPacketsInBuffer; }
            void setEqualPacketsInBuffer(unsigned int count) { this->equalPacketsInBuffer = count; }

        private:
            VisibleHost* sender;
            VisibleHost* receiver;
            NetEvent* netEvent;

            QBrush brushIPv6;

            unsigned int equalPacketsInBuffer;

            QPointF vectorToTarget;
    };

} // namespace Netvisix
#endif // VISIBLEPACKET_H
