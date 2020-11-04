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
#include "Net/Host.h"
#include "Net/NetUtil.h"
#include "Net/NetEventManager.h"
#include "GUI/VisibleHost.h"
#include "GUI/StatisticPopup.h"

#include <QtWidgets>


namespace Netvisix {

    HostInfoPopup::HostInfoPopup(VisibleHost* vHost, QWidget* parent) :
            QWidget(parent),
            ui(new Ui::HostInfoPopup) {

        this->vHost = vHost;

        ui->setupUi(this);

        timer = new QTimer();
        QObject::connect(timer, SIGNAL(timeout()), this, SLOT(updateLoop()));
        timer->start();

        setAttribute(Qt::WA_DeleteOnClose);

        setStyleSheet("QWidget#HostInfoPopup {border: 2px solid rgb(110, 110, 110); background-color: rgb(150, 150, 150);}");

        ui->buttonStatistic->setStyleSheet("background-color: rgb(200, 200, 200)");

        QPoint vHostPos = vHost->getPosition().toPoint();

        const std::vector<Tins::IPv4Address> addrListIPv4 = vHost->getHost()->getAddrListIPv4();
        const std::vector<Tins::IPv6Address> addrListIPv6 = vHost->getHost()->getAddrListIPv6();
        int addrCount = addrListIPv4.size() + addrListIPv6.size();
        if (vHost->getHost()->addrHW != NetUtil::zeroAddrHW) {
            addrCount++;
        }

        const int spacerY = 4;
        const int heightAddition = addrCount * (ui->labelAddr->height() + spacerY) + 25;
        resize(width(), height() + heightAddition);

        float offsetX = 0;
        float offsetY = -height();
        if (vHostPos.x() + width() > parentWidget()->width()) {
            offsetX = -width();
        }
        if (vHostPos.y() - height() < 0) {
            offsetY = 0;
        }

        QPoint pos = vHostPos + QPoint(offsetX, offsetY);
        move(pos);

        NetView* nv = (NetView*) parent;
        if (vHost->getHost()->hostname != "") {
            std::string hostname = "<b>" + vHost->getHost()->hostname + "</b>";
            if (vHost->getHost()->hostnameFromReverseDNSLookp) {
                hostname = "(rDNS) " + hostname;
            }
            ui->labelHostname->setText(hostname.c_str());
        }
        else if (nv->getReverseDNSLookupEnabled() == false) {
            ui->labelHostname->setText("(rDNS disabled!)");
        }

        // the addresses
        int addrLabelIndex = 0;
        if (vHost->getHost()->addrHW != NetUtil::zeroAddrHW) {
            addAddr("HW", vHost->getHost()->addrHW.to_string(), addrLabelIndex, spacerY);
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

        // byte counters
        ui->labelBytesSnt->move(ui->labelBytesSnt->pos().x(), ui->labelBytesSnt->pos().y() + heightAddition - 10);
        ui->labelBytesRcv->move(ui->labelBytesRcv->pos().x(), ui->labelBytesRcv->pos().y() + heightAddition - 10);

        updateBytes();

        // statistic button
        ui->buttonStatistic->move(ui->buttonStatistic->pos().x(), ui->buttonStatistic->pos().y() + heightAddition);

        MainWindow::updateAllWidgetFonts();
    }

    HostInfoPopup::~HostInfoPopup() {
        delete ui;
    }

    void HostInfoPopup::addAddr(const std::string& type, const std::string& addr, int addrLabelIndex, int spacerY) {
        int posY = ui->labelAddr->pos().y() + addrLabelIndex * (ui->labelAddr->height() + spacerY);

        QLabel* labelType = new QLabel(type.c_str(), this);
        int typePosX = ui->labelAddr->pos().x();
        labelType->move(typePosX, posY);

        QLabel* labelAddr = new QLabel(addr.c_str(), this);
        int addrPosX = ui->labelAddrField->pos().x();
        labelAddr->move(addrPosX, posY);
    }

    void HostInfoPopup::paintEvent(QPaintEvent* event) {
        QPainter p(this);
        QStyleOption opt;
        opt.init(this);
        style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    }

    void HostInfoPopup::updateBytes() {
        unsigned long long bytesSnt = this->vHost->getHost()->statistic->getItem(IPVersion::ALL, Protocol::ETHERNETII)->bytesSnt;
        unsigned long long bytesRcv = this->vHost->getHost()->statistic->getItem(IPVersion::ALL, Protocol::ETHERNETII)->bytesRcv;

        ui->labelBytesSnt->setText("Snt: <b>" + QString::fromUtf8(NetUtil::getByteString(bytesSnt).c_str()) + "</b>");
        ui->labelBytesRcv->setText("Rcv: <b>" + QString::fromUtf8(NetUtil::getByteString(bytesRcv).c_str()) + "</b>");
    }

    void HostInfoPopup::updateLoop() {
        if (NetEventManager::SharedInstance()->getIsSniffingRunning() == false) {
            close();
            return;
        }

        if (QDateTime::currentMSecsSinceEpoch() - lastUpdateTime >= 300) {
            updateBytes();
            lastUpdateTime = QDateTime::currentMSecsSinceEpoch();
        }
    }

} // namespace Netvisix

void Netvisix::HostInfoPopup::on_buttonStatistic_clicked() {
    Host* host = vHost->getHost();
    std::string title = "";

    if (host->hostname.empty() == false) {
        title += host->hostname;
    } else if (host->getAddrListIPv4().empty() == false) {
        title += host->getAddrListIPv4().at(0).to_string();
    } else if (host->getAddrListIPv6().empty() == false) {
        title += host->getAddrListIPv6().at(0).to_string();
    } else if (host->addrHW != NetUtil::zeroAddrHW) {
        title += host->addrHW.to_string();
    }

    StatisticPopup* statisticPopup = new StatisticPopup(vHost->getHost()->statistic, title, parentWidget()->parentWidget()->parentWidget());
    statisticPopup->show();
}
