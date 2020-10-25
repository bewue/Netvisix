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

#include "VisibleBase.h"

#include <QWidget>


namespace Netvisix {

    VisibleBase::VisibleBase(NetView* netView) {
        this->netView = netView;
        color = QColor(255, 0, 255, 255);
        brush = QBrush(color, Qt::BrushStyle::SolidPattern);

        setRadius(20);
        setPosition(radius, radius);

        speed = 0.1f;
        targetPos = getPosition();

        toDelete = false;
    }

    VisibleBase::~VisibleBase() {
    }

    void VisibleBase::setPosition(float x, float y) {
        position.setX(x);
        position.setY(y);
    }

    void VisibleBase::setPosition(QPointF point) {
        setPosition(point.x(), point.y());
    }

    void VisibleBase::setRadius(float radius) {
        this->radius = radius;
    }

    QPointF VisibleBase::getVectorToTarget() {
        double diffX = targetPos.x() - position.x();
        double diffY = targetPos.y() - position.y();

        double length = std::sqrt(diffX * diffX + diffY * diffY);
        double vX = diffX / length;
        double vY = diffY / length;

        return QPointF(vX, vY);
    }

    void VisibleBase::onVisibleUpdate(quint64 dt) {
        if (position != targetPos) {
            double diffX = targetPos.x() - position.x();
            double diffY = targetPos.y() - position.y();
            if (std::abs(diffX) < 5.0f && std::abs(diffY) < 5.0f) {
                position = targetPos;
                onReachedTargetPosition();
                return;
            }

            double length = std::sqrt(diffX * diffX + diffY * diffY);
            double vX = (diffX / length) * speed * dt;
            double vY = (diffY / length) * speed * dt;

            double newX = position.x() + vX;
            double newY = position.y() + vY;

            setPosition(newX, newY);
        }
    }

    void VisibleBase::onVisiblePaint(QPainter &painter) {
        painter.setBrush(brush);
        painter.drawEllipse(position, radius, radius);
    }

    void VisibleBase::onReachedTargetPosition() {
    }

} // namespace Netvisix
