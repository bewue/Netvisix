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

#ifndef VISIBLEHOST_H
#define VISIBLEHOST_H

#include "VisibleBase.h"


namespace Netvisix {

    class Host;
    enum class Protocol;
    class NetEvent;

    class VisibleHost : public VisibleBase {

        public:
            VisibleHost(NetView* netView, Host* host);
            virtual ~VisibleHost();

            virtual void onVisibleUpdate(quint64 dt);
            virtual void onVisiblePaint(QPainter& painter);

            void showMulticastEffect(NetEvent* netEvent);
            void showHostAliveEffect();

            Host* getHost() { return this->host; }

            void setMarked(bool marked) { this->isMarked = marked; }
            bool getMarked() { return this->isMarked; }

            const static Qt::BrushStyle MULTICAST_BRUSH_STYLE;

            bool getIsVisible() { return this->isVisible || this->isMarked; }

        private:
            struct MulticastEffectData {
                bool isActive = false;
                float currentRadius = 0;
                QColor color = Qt::black;
                QBrush brush;
            };
            std::vector<MulticastEffectData*>* mcEffectDataList;

            bool aliveEffectActive;
            quint64 aliveEffectTimer;
            const float ALIVE_EFFECT_STAY_DURATION = 5000;
            const float ALIVE_EFFECT_FADE_OUT_DURATION = 1000;
            QColor colorAliveEffect;
            QBrush brushAliveEffect;

            bool isVisible;
            quint64 visibleTimer;
            const float VISIBLE_DURATION = 11000;

            Host* host;

            bool isMarked;
            QBrush brushMarked;

            QColor colorLocalInterfaceMarker;
        };

} // namespace Netvisix
#endif // VISIBLEHOST_H
