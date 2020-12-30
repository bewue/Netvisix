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

#include "HostInfoPopup.h"
#include "NetView.h"
#include "MainWindow.h"
#include "ui_HostInfoPopup.h"
#include "ui_MainWindow.h"
#include "Net/Host.h"
#include "Net/NetUtil.h"
#include "Net/NetEventManager.h"
#include "GUI/VisibleHost.h"
#include "GUI/StatisticPopup.h"

#include <QtWidgets>


namespace Netvisix {

    HostInfoPopup::HostInfoPopup(Host* host, MainWindow* mainWindow) :
            QWidget(mainWindow->getUI()->widgetNetView),
            ui(new Ui::HostInfoPopup) {

        this->mainWindow    = mainWindow;
        this->host          = host;

        ui->setupUi(this);

        lastUpdateTime = 0;

        timer = new QTimer();
        QObject::connect(timer, SIGNAL(timeout()), this, SLOT(updateLoop()));
        timer->start();

        setAttribute(Qt::WA_DeleteOnClose);

        setStyleSheet("QWidget#HostInfoPopup {border: 2px solid rgb(110, 110, 110); background-color: rgb(150, 150, 150);}");

        ui->buttonStatistic->setStyleSheet("background-color: rgb(200, 200, 200)");

        const std::vector<Tins::IPv4Address> addrListIPv4 = host->getAddrListIPv4();
        const std::vector<Tins::IPv6Address> addrListIPv6 = host->getAddrListIPv6();
        int addrCount = addrListIPv4.size() + addrListIPv6.size();
        if (host->getAddrHW() != NetUtil::zeroAddrHW) {
            addrCount++;
        }

        const int spacerY = 4;
        const int heightAddition = addrCount * (ui->labelAddr->height() + spacerY) + 25;
        resize(width(), height() + heightAddition);

        if (host->getHostname() != "") {
            std::string hostname = "<b>" + host->getHostname() + "</b>";
            if (host->getHostnameIsFromReverseDNSLookp()) {
                hostname = "(rDNS) " + hostname;
            }
            ui->labelHostname->setText(hostname.c_str());
        }
        else if (mainWindow->getUI()->widgetNetView->getReverseDNSLookupEnabled() == false) {
            ui->labelHostname->setText("(rDNS disabled!)");
        }

        // the addresses
        int addrLabelIndex = 0;
        if (host->getAddrHW() != NetUtil::zeroAddrHW) {
            addAddr("HW", host->getAddrHW().to_string(), addrLabelIndex, spacerY);
            addrLabelIndex++;
        }

        for (const Tins::IPv4Address& ip : addrListIPv4) {
            addAddr("IPv4", ip.to_string(), addrLabelIndex, spacerY);
            addrLabelIndex++;
        }

        for (const Tins::IPv6Address& ip : addrListIPv6) {
            addAddr("IPv6", ip.to_string(), addrLabelIndex, spacerY);
            addrLabelIndex++;
        }

        // packet counters
        ui->labelFramesSnt->move(ui->labelFramesSnt->pos().x(), ui->labelFramesSnt->pos().y() + heightAddition - 10);
        ui->labelFramesRcv->move(ui->labelFramesRcv->pos().x(), ui->labelFramesRcv->pos().y() + heightAddition - 10);

        // byte counters
        ui->labelBytesSnt->move(ui->labelBytesSnt->pos().x(), ui->labelBytesSnt->pos().y() + heightAddition - 10);
        ui->labelBytesRcv->move(ui->labelBytesRcv->pos().x(), ui->labelBytesRcv->pos().y() + heightAddition - 10);

        updateCounters();

        // statistic button
        ui->buttonStatistic->move(ui->buttonStatistic->pos().x(), ui->buttonStatistic->pos().y() + heightAddition);

        MainWindow::updateAllWidgetFonts();
    }

    HostInfoPopup::~HostInfoPopup() {
        delete ui;
    }

    void HostInfoPopup::setPositionOnVisibleHost(QPoint hostPos) {
        float offsetX = 0;
        float offsetY = -height();
        if (hostPos.x() + width() > parentWidget()->width()) {
            offsetX = -width();
        }
        if (hostPos.y() - height() < 0) {
            offsetY = 0;
        }

        move(hostPos + QPoint(offsetX, offsetY));
    }

    void HostInfoPopup::setPositionOnHostListItem(QPoint mousePos) {
        int posY = mousePos.y() - height();
        posY = std::min(posY, parentWidget()->height() - height());
        posY = std::max(posY, 0);

        move(QPoint(parentWidget()->width() - width(), posY));
    }

    void HostInfoPopup::addAddr(const std::string& type, const std::string& addr, int addrLabelIndex, int spacerY) {
        int posY = ui->labelAddr->pos().y() + addrLabelIndex * (ui->labelAddr->height() + spacerY);

        QLabel* labelType = new QLabel(type.c_str(), this);
        int typePosX = ui->labelAddr->pos().x();
        labelType->move(typePosX, posY);

        QLabel* labelAddr = new QLabel(std::string("<b>" + addr + "</b>").c_str(), this);
        labelAddr->setTextInteractionFlags(Qt::TextSelectableByMouse);
        int addrPosX = ui->labelAddrField->pos().x();
        labelAddr->move(addrPosX, posY);
    }

    void HostInfoPopup::paintEvent(QPaintEvent* event) {
        QPainter p(this);
        QStyleOption opt;
        opt.init(this);
        style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    }

    void HostInfoPopup::updateCounters() {
        unsigned long long framesSnt = host->statistic->getItem(IPVersion::ALL, Protocol::EthernetII)->framesSnt;
        unsigned long long framesRcv = host->statistic->getItem(IPVersion::ALL, Protocol::EthernetII)->framesRcv;

        ui->labelFramesSnt->setText("FramesSnt: <b>" + QString::number(framesSnt) + "</b>");
        ui->labelFramesRcv->setText("FramesRcv: <b>" + QString::number(framesRcv) + "</b>");

        unsigned long long bytesSnt = host->statistic->getItem(IPVersion::ALL, Protocol::EthernetII)->bytesSnt;
        unsigned long long bytesRcv = host->statistic->getItem(IPVersion::ALL, Protocol::EthernetII)->bytesRcv;

        ui->labelBytesSnt->setText("BytesSnt: &nbsp;<b>" + QString::fromUtf8(NetUtil::getByteString(bytesSnt).c_str()) + "</b>");
        ui->labelBytesRcv->setText("BytesRcv: &nbsp;<b>" + QString::fromUtf8(NetUtil::getByteString(bytesRcv).c_str()) + "</b>");
    }

    void HostInfoPopup::updateLoop() {
        if (NetEventManager::SharedInstance()->getIsSniffingRunning() == false) {
            close();
            return;
        }

        if (QDateTime::currentMSecsSinceEpoch() - lastUpdateTime >= 300) {
            updateCounters();
            lastUpdateTime = QDateTime::currentMSecsSinceEpoch();
        }
    }

} // namespace Netvisix

void Netvisix::HostInfoPopup::on_buttonStatistic_clicked() {
    std::string title = host->getPreferedHostIdentifier();

    StatisticPopup* statisticPopup = new StatisticPopup(host->statistic, title, mainWindow);
    statisticPopup->show();
}
