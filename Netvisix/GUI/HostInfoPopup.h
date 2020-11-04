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

#ifndef HOSTINFOPOPUP_H
#define HOSTINFOPOPUP_H

#include <QWidget>

namespace Ui {
class HostInfoPopup;
}

namespace Netvisix {

    class VisibleHost;

    class HostInfoPopup : public QWidget {

        public:
            explicit HostInfoPopup(VisibleHost* vHost, QWidget* parent = 0);
            ~HostInfoPopup();

            VisibleHost* getHost() { return this->vHost; }

        protected:
            void paintEvent(QPaintEvent* event);

        private slots:
            void on_buttonStatistic_clicked();
            void updateLoop();

        private:
            Q_OBJECT

            void addAddr(const std::string& type, const std::string& addr, int addrLabelIndex, int spacerY);

            void updateBytes();

            QTimer* timer;
            quint64 lastUpdateTime;

            Ui::HostInfoPopup *ui;

            VisibleHost* vHost;
    };

} // namespace Netvisix
#endif // HOSTINFOPOPUP_H
