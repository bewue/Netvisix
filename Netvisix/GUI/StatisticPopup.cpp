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

#include "StatisticPopup.h"
#include "ui_StatisticPopup.h"
#include "MainWindow.h"
#include "Net/NetStatistic.h"
#include "Net/NetEvent.h"
#include "Net/NetEventManager.h"
#include "Net/NetUtil.h"

#include <QDateTime>
#include <QTimer>
#include <QTableWidget>

#include <iomanip>
#include <sstream>

namespace Netvisix {

    StatisticPopup::StatisticPopup(NetStatistic* statistic, std::string title, QWidget* parent) :
            QWidget(parent, Qt::Window),
            ui(new Ui::StatisticPopup) {

        ui->setupUi(this);

        setAttribute(Qt::WA_DeleteOnClose);

        this->statistic = statistic;

        QPalette Pal(palette());
        Pal.setColor(QPalette::Background, QColor(200, 200, 200, 255));
        setAutoFillBackground(true);
        setPalette(Pal);

        lastUpdateTime = 0;

        setWindowTitle(title.c_str());

        timer = new QTimer();
        QObject::connect(timer, SIGNAL(timeout()), this, SLOT(updateLoop()));
        timer->start();

        items = new std::vector<StatisticItem*>();

        float frameWidth = START_POS_X + 5 * OFFSET_X;
        float frameHeight = START_POS_Y + 18 * OFFSET_Y + 10;
        setFixedSize(frameWidth, frameHeight);

        // gui layout
        const float columnTitlePosY = 10;
        createColumnTitleLabel(START_POS_X + 1 * OFFSET_X, columnTitlePosY, "PcktsSnt");
        createColumnTitleLabel(START_POS_X + 2 * OFFSET_X, columnTitlePosY, "PcktsRcv");
        createColumnTitleLabel(START_POS_X + 3 * OFFSET_X, columnTitlePosY, "BytesSnt");
        createColumnTitleLabel(START_POS_X + 4 * OFFSET_X, columnTitlePosY, "BytesRcv");

        addItem(statistic->getItem(IPVersion::ALL, Protocol::ETHERNETII), "Frames", 0);
        addItem(statistic->getItem(IPVersion::ALL, Protocol::UNKOWN), "Unkown", 1);

        // IPv4
        float posY = START_POS_Y + 2.6f * OFFSET_Y;
        createSubTitleLabel(START_POS_X, posY, "IPv4");
        createLine(posY + 25);

        addItem(statistic->getItem(IPVersion::IPV4, Protocol::ARP), "ARP", 4);

        addItem(statistic->getItem(IPVersion::IPV4, Protocol::ICMP), "ICMP", 6);
        addItem(statistic->getItem(IPVersion::IPV4, Protocol::IGMP), "IGMP", 7);

        addItem(statistic->getItem(IPVersion::IPV4, Protocol::TCP), "TCP", 9);
        addItem(statistic->getItem(IPVersion::IPV4, Protocol::UDP), "UDP", 10);

        // IPv6
        posY = START_POS_Y + 11.6f * OFFSET_Y;
        createSubTitleLabel(START_POS_X, posY, "IPv6");
        createLine(posY + 25);

        addItem(statistic->getItem(IPVersion::IPV6, Protocol::ICMPv6), "ICMPv6", 13);

        addItem(statistic->getItem(IPVersion::IPV6, Protocol::TCP), "TCP", 15);
        addItem(statistic->getItem(IPVersion::IPV6, Protocol::UDP), "UDP", 16);

        ui->labelInfo->move(frameWidth / 2 - ui->labelInfo->width() / 2, frameHeight - 25);

        updateStatisticDisplay();

        MainWindow::updateAllWidgetFonts();
    }

    StatisticPopup::~StatisticPopup() {
        delete timer;
        delete ui;

        for (unsigned int i = 0; i < items->size(); i++) {
            delete items->at(i);
        }
        delete items;
    }

    void StatisticPopup::addItem(NetStatistic::Item *item, std::string name, int y) {
        float posY = START_POS_Y + y * OFFSET_Y;

        StatisticItem* sItem = new StatisticItem();

        sItem->item = item;

        sItem->labelName = new QLabel(name.c_str(), this);
        sItem->labelName->move(START_POS_X, posY);
        sItem->labelName->setStyleSheet("QLabel { color: rgb(70, 70, 130) }");

        sItem->labelPacketsSnt = createCounterLabel(START_POS_X + 1 * OFFSET_X, posY);
        sItem->labelPacketsRcv = createCounterLabel(START_POS_X + 2 * OFFSET_X, posY);
        sItem->labelBytesSnt = createCounterLabel(START_POS_X + 3 * OFFSET_X, posY);
        sItem->labelBytesRcv = createCounterLabel(START_POS_X + 4 * OFFSET_X, posY);

        items->push_back(sItem);
    }

    void StatisticPopup::createColumnTitleLabel(float posX, float posY, std::string text) {
        QLabel* label = new QLabel(text.c_str(), this);
        label->move(posX, posY);
        label->setFixedWidth(200);
        label->setStyleSheet("QLabel { color: rgb(70, 70, 130) }");
    }

    void StatisticPopup::createSubTitleLabel(float posX, float posY, std::string text) {
        QLabel* label = new QLabel(text.c_str(), this);
        label->move(posX, posY);
        label->setFixedWidth(200);
        label->setStyleSheet("QLabel { color: rgb(130, 130, 180) }");
    }

    void StatisticPopup::createLine(float posY) {
        QFrame* line = new QFrame(this);
        line->setGeometry(QRect(8, posY, width() - 16, 2));

        QPalette Pal(palette());
        Pal.setColor(QPalette::Background, QColor(160, 160, 180));
        line->setAutoFillBackground(true);
        line->setPalette(Pal);
    }

    QLabel* StatisticPopup::createCounterLabel(float posX, float posY) {
        QLabel* label = new QLabel(this);
        label->move(posX, posY);
        label->setFixedWidth(200);

        return label;
    }

    void StatisticPopup::updateStatisticDisplay() {
        for (unsigned int i = 0; i < items->size(); i++) {
            StatisticItem* sItem = items->at(i);

            handleCountTextColor(sItem->labelPacketsSnt, sItem->item->packetsSnt);
            sItem->labelPacketsSnt->setText(QString::number(sItem->item->packetsSnt));

            handleCountTextColor(sItem->labelPacketsRcv, sItem->item->packetsRcv);
            sItem->labelPacketsRcv->setText(QString::number(sItem->item->packetsRcv));

            handleCountTextColor(sItem->labelBytesSnt, sItem->item->bytesSnt);
            sItem->labelBytesSnt->setText(NetUtil::getByteString(sItem->item->bytesSnt).c_str());

            handleCountTextColor(sItem->labelBytesRcv, sItem->item->bytesRcv);
            sItem->labelBytesRcv->setText(NetUtil::getByteString(sItem->item->bytesRcv).c_str());
        }
    }

    void StatisticPopup::handleCountTextColor(QLabel* label, int value) {
        if (value == 0) {
            label->setStyleSheet("QLabel { color: rgb(180, 180, 180) }");
        }
        else {
            label->setStyleSheet("QLabel { color: rgb(0, 0, 0) }");
        }
    }

    void StatisticPopup::updateLoop() {
        if (NetEventManager::SharedInstance()->getIsSniffingRunning() == false) {
            close();
            return;
        }

        if (QDateTime::currentMSecsSinceEpoch() - lastUpdateTime >= 300) {
            updateStatisticDisplay();
            lastUpdateTime = QDateTime::currentMSecsSinceEpoch();
        }
    }

} // namespace Netvisix
