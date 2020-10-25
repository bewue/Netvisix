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

#ifndef VISIBLEBASE_H
#define VISIBLEBASE_H

#include <QtWidgets>


namespace Netvisix {

    class NetView;

    class VisibleBase {

        public:
            VisibleBase(NetView* netView);
            virtual ~VisibleBase();

            virtual void onVisibleUpdate(quint64 dt);
            virtual void onVisiblePaint(QPainter& painter);

            virtual void onReachedTargetPosition();

            QPointF getPosition() { return this->position; }
            void setPosition(float x, float y);
            void setPosition(QPointF point);

            QPointF getTargetPosition() { return this->targetPos; }
            void setTargetPosition(QPointF point) { this->targetPos = point; }

            float getRadius() { return this->radius; }
            void setRadius(float radius);

            bool getToDelete() { return this->toDelete; }
            void setToDelete(bool toDelete) { this->toDelete = toDelete; }

            QPointF getVectorToTarget();

        protected:
            bool toDelete;

            NetView* netView;

            float speed;
            QPointF targetPos;

            QColor color;

            QBrush brush;

        private:
            QPointF position;
            float radius;

        };

} // namespace Netvisix
#endif // VISIBLEBASE_H
