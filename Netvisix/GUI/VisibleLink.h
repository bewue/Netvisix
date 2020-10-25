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

#ifndef VISIBLELINK_H
#define VISIBLELINK_H

#include "GUI/VisibleBase.h"


namespace Netvisix {

    class VisibleHost;

    class VisibleLink : public VisibleBase {

        public:
            VisibleLink(NetView* netView);
            virtual ~VisibleLink();

            virtual void onVisibleUpdate(quint64 dt);
            virtual void onVisiblePaint(QPainter& painter);

            bool getIsActive() { return this->isActive; }
            void setIsActive(bool isActive) { this->isActive = isActive; }

            bool equals(VisibleHost* sender, VisibleHost* receiver);

            void showLink(VisibleHost* sender, VisibleHost* receiver);

        private:
            VisibleHost* sender;
            VisibleHost* receiver;

            bool isActive;
            quint64 timer;
            float stayDuration;
    };

} // namespace Netvisix
#endif // VISIBLELINK_H
