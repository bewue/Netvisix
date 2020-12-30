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

#include "VisibleLink.h"
#include "Config.h"
#include "GUI/VisibleHost.h"


namespace Netvisix {

    VisibleLink::VisibleLink(NetView* netView) : VisibleBase(netView) {
        Config::Theme* theme = Config::Settings::SharedInstance()->theme;
        color = theme->linkColor;
        isActive = false;
    }

    VisibleLink::~VisibleLink() {
    }

    bool VisibleLink::equals(VisibleHost *sender, VisibleHost *receiver) {
        if ((this->sender == sender && this->receiver == receiver) || (this->sender == receiver && this->receiver == sender)) {
            return true;
        }
        else {
            return false;
        }
    }

    void VisibleLink::showLink(VisibleHost* sender, VisibleHost* receiver) {
        this->sender = sender;
        this->receiver = receiver;

        timer = 0;
        double diffX = receiver->getPosition().x() - sender->getPosition().x();
        double diffY = receiver->getPosition().y() - sender->getPosition().y();
        double length = std::sqrt(diffX * diffX + diffY * diffY);
        stayDuration = length / 0.35f;

        isActive = true;
    }

    void VisibleLink::onVisibleUpdate(quint64 dt) {
        if (isActive) {
            timer += dt;
            if (timer >= stayDuration) {
                isActive = false;
            }
        }
    }

    void VisibleLink::onVisiblePaint(QPainter& painter) {
        if (isActive) {
            painter.setRenderHint(QPainter::Antialiasing, true);
            painter.setPen(color);
            painter.drawLine(sender->getPosition(), receiver->getPosition());
            painter.setRenderHint(QPainter::Antialiasing, false);
        }
    }

} // namespace Netvisix
