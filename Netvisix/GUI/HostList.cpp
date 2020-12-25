/*
 *  Copyright (C) 2020 Benjamin W. (bitbatzen@gmail.com)
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

#include "HostList.h"
#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "Config.h"
#include "Net/NetEventManager.h"
#include "Net/NetEvent.h"
#include "Net/NetUtil.h"
#include "Net/NetStatistic.h"
#include "Net/Host.h"
#include "GUI/HostInfoPopup.h"
#include "GUI/VisibleHost.h"

#include <QtWidgets>


namespace Netvisix {

    HostList::HostList(MainWindow* mainWindow) {
        this->mainWindow = mainWindow;

        tableWidget = new QTableWidget();
        tableWidget->verticalHeader()->setVisible(false);
        tableWidget->setSelectionMode(QAbstractItemView::NoSelection);
        tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

        tableWidget->setColumnCount(4);
        tableWidget->setColumnHidden(3, true);

        tableWidget->setMouseTracking(true);
        connect(tableWidget, SIGNAL(cellEntered(int, int)), this, SLOT(tableCellEntered(int, int)));

        // table header
        QStringList headerLables;
        headerLables << "Host" << "Frames" << "Bytes";
        tableWidget->setHorizontalHeaderLabels(headerLables);

        const int width = mainWindow->getUI()->widgetHostList->width() - 40;
        QHeaderView* header = tableWidget->horizontalHeader();
        header->resizeSection(0, width * 0.5f);
        header->resizeSection(1, width * 0.25f);
        header->resizeSection(2, width * 0.25f);

        mainWindow->getUI()->widgetHostList->setLayout(new QVBoxLayout());
        mainWindow->getUI()->widgetHostList->layout()->addWidget(tableWidget);

        NetEventManager::SharedInstance()->addPreparedNetEventListener(this);

        hostInfoPopup = nullptr;

        cbShowAll = mainWindow->getUI()->widgetHostListControls->findChild<QCheckBox*>("cbShowAll");

        lastUpdateTime = 0;

        QTimer* timer = new QTimer();
        QObject::connect(timer, SIGNAL(timeout()), this, SLOT(updateLoop()));
        timer->start();

        newHosts        = std::vector<Host*>();
        hostsToUpdate   = std::set<Host*>();
    }

    HostList::~HostList() {
    }

    void HostList::onPreparedNetEventNewHost(Host* newHost) {
        newHosts.push_back(newHost);
    }

    void HostList::onPreparedNetEventNewUnicastPacket(Host* sender, Host* receiver, NetEvent* netEvent) {
        hostsToUpdate.insert(sender);
        hostsToUpdate.insert(receiver);
    }

    void HostList::onPreparedNetEventNewMulticastPacket(Host* sender, NetEvent* netEvent) {
        hostsToUpdate.insert(sender);
    }

    void HostList::onHostAddrUpdate(Host* host) {
        hostsToUpdate.insert(host);
    }

    void HostList::reset() {
        tableWidget->setRowCount(0);
    }

    Host* HostList::getHostPointer(int row) {
        QTableWidgetItem* item = tableWidget->item(row, 3);
        qulonglong hostPointer = item->data(Qt::UserRole).toULongLong();
        Host* host = reinterpret_cast<Host*>(hostPointer);
        return host;
    }

    void HostList::addNewHostListItems() {
        for (auto it = newHosts.begin(); it != newHosts.end();) {
            tableWidget->insertRow(tableWidget->rowCount());
            int index = tableWidget->rowCount() - 1;

            QTableWidgetItem* item = new QTableWidgetItem();
            qulonglong hostPointer = reinterpret_cast<qulonglong>(*it);
            item->setData(Qt::UserRole, hostPointer);
            tableWidget->setItem(index, 3, item);

            updateHostListItem(index, true);

            it = newHosts.erase(it);
        }
    }

    void HostList::updateHostListItem(int row, bool force) {
        Host* host = getHostPointer(row);

        bool updateAllData = true;
        if (! force) {
            auto hostToUpdateIt = hostsToUpdate.find(host);
            if (hostToUpdateIt != hostsToUpdate.end()) {
                hostsToUpdate.erase(hostToUpdateIt);
            }
            else {
                updateAllData = false;
            }
        }

        VisibleHost* vHost = mainWindow->getUI()->widgetNetView->getVisibleHost(host);

        QColor bgColor = Qt::yellow;

        if (! vHost->getIsAlive()) {
            bgColor = Qt::white;

            if (! cbShowAll->isChecked()) {
                tableWidget->hideRow(row);
                return;
            }
        }

        tableWidget->showRow(row);

        if (updateAllData) {
            unsigned long long framesSnt = host->statistic->getItem(IPVersion::ALL, Protocol::EthernetII)->framesSnt;
            unsigned long long framesRcv = host->statistic->getItem(IPVersion::ALL, Protocol::EthernetII)->framesRcv;

            unsigned long long bytesSnt = host->statistic->getItem(IPVersion::ALL, Protocol::EthernetII)->bytesSnt;
            unsigned long long bytesRcv = host->statistic->getItem(IPVersion::ALL, Protocol::EthernetII)->bytesRcv;

            std::string hostname = host->getPreferedHostIdentifier();
            const int maxChars = 22;
            if (hostname.size() > maxChars) {
                hostname = hostname.substr(0, maxChars) + "…";
            }

            QTableWidgetItem* itemHost = new QTableWidgetItem(hostname.c_str());
            tableWidget->setItem(row, 0, itemHost);

            QTableWidgetItem* itemPackets = new QTableWidgetItem();
            itemPackets->setData(Qt::EditRole, framesSnt + framesRcv);
            tableWidget->setItem(row, 1, itemPackets);

            QTableWidgetItem* itemBytes = new QTableWidgetItem(QString::fromUtf8(NetUtil::getByteString(bytesSnt + bytesRcv).c_str()));
            tableWidget->setItem(row, 2, itemBytes);
        }

        tableWidget->item(row, 0)->setBackground(bgColor);
        tableWidget->item(row, 1)->setBackground(bgColor);
        tableWidget->item(row, 2)->setBackground(bgColor);
    }

    void HostList::updateLoop() {
        addNewHostListItems();

        if (QDateTime::currentMSecsSinceEpoch() - lastUpdateTime >= 300) {
            updateAllHostListItems(false);
            lastUpdateTime = QDateTime::currentMSecsSinceEpoch();
        }

        if (hostInfoPopup != nullptr) {
            QPoint mousePosTabelWidget = tableWidget->mapFromGlobal(QCursor::pos());
            QPoint mousePosMainWindow = mainWindow->getUI()->widgetNetView->mapFromGlobal(QCursor::pos());
            QRect rect = tableWidget->rect();
            rect.moveLeft(-20);
            if (! rect.contains(mousePosTabelWidget) && ! hostInfoPopup->geometry().contains(mousePosMainWindow)) {
                hostInfoPopup->close();
                hostInfoPopup = nullptr;
            }
        }
    }

    void HostList::tableCellEntered(int row, int column) {
        Host* host = getHostPointer(row);

        if (hostInfoPopup != nullptr && host != hostInfoPopup->getHost()) {
            hostInfoPopup->close();
            hostInfoPopup = nullptr;
        }

        if (hostInfoPopup == nullptr) {
            hostInfoPopup = new HostInfoPopup(host, mainWindow);
            hostInfoPopup->setPositionOnHostListItem(mainWindow->mapFromGlobal(QCursor::pos()));
            hostInfoPopup->show();
        }
    }

    void HostList::updateAllHostListItems(bool force) {
        for (int i = 0; i < tableWidget->rowCount(); i++) {
            updateHostListItem(i, force);
        }

        tableWidget->sortByColumn(1, Qt::DescendingOrder);
    }
}
