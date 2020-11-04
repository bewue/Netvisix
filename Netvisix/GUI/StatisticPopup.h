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

#ifndef STATISTICPOPUP_H
#define STATISTICPOPUP_H

#include "Net/NetStatistic.h"

#include <QWidget>
#include <QLabel>


namespace Ui {
class StatisticPopup;
}

namespace Netvisix {

    class NetStatistic;
    enum class Protocol;
    enum class IPVersion;

    class StatisticPopup : public QWidget {

        public:
            struct StatisticItem {
                NetStatistic::Item* item;
                QLabel* labelName;
                QLabel* labelPacketsSnt;
                QLabel* labelPacketsRcv;
                QLabel* labelBytesSnt;
                QLabel* labelBytesRcv;
            };

            explicit StatisticPopup(NetStatistic* statistic, std::string title, QWidget* parent);
            ~StatisticPopup();

        public slots:
             void updateLoop();

        private:
            Q_OBJECT

            void addItem(NetStatistic::Item* item, std::string name, int y);

            void updateStatisticDisplay();

            void handleCountTextColor(QLabel* label, int value);

            void createColumnTitleLabel(float posX, float posY, std::string text);
            void createSubTitleLabel(float posX, float posY, std::string text);
            void createLine(float posY);
            QLabel* createCounterLabel(float posX, float posY);

            Ui::StatisticPopup *ui;

            NetStatistic* statistic;

            QTimer* timer;
            quint64 lastUpdateTime;

            std::vector<StatisticItem*>* items;

            const float START_POS_X = 10;
            const float START_POS_Y = 40;
            const float OFFSET_X = 140;
            const float OFFSET_Y = 24;
    };

} // namespace Netvisix
#endif // STATISTICPOPUP_H
