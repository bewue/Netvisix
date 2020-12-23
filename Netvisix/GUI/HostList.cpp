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

#include <QtWidgets>


namespace Netvisix {

    HostList::HostList(MainWindow* mainWindow) {
        this->mainWindow = mainWindow;

        tableWidget = new QTableWidget();
        tableWidget->verticalHeader()->setVisible(false);
        tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

        tableWidget->setColumnCount(4);
        tableWidget->setColumnHidden(3, true);

        tableWidget->setMouseTracking(true);
        connect(tableWidget, SIGNAL(cellEntered(int, int)), this, SLOT(tableCellEntered(int, int)));

        // color
        QPalette palette = tableWidget->palette();
        palette.setBrush(QPalette::Highlight, QBrush(Qt::white));
        palette.setBrush(QPalette::HighlightedText, QBrush(Qt::black));
        tableWidget->setPalette(palette);

        // table header
        QStringList headerLables;
        headerLables << "Host" << "Packets" << "Bytes";
        tableWidget->setHorizontalHeaderLabels(headerLables);

        const int width = mainWindow->getUI()->widgetHostList->width() - 40;
        QHeaderView* header = tableWidget->horizontalHeader();
        header->resizeSection(0, width * 0.5f);
        header->resizeSection(1, width * 0.25f);
        header->resizeSection(2, width * 0.25f);

        mainWindow->getUI()->widgetHostList->setLayout(new QVBoxLayout());
        mainWindow->getUI()->widgetHostList->layout()->addWidget(tableWidget);

        NetEventManager::SharedInstance()->addPreparedNetEventListener(this);

        updateTimer = 0;

        hostInfoPopup = nullptr;
    }

    HostList::~HostList() {
    }

    void HostList::onPreparedNetEventNewHost(Host* newHost) {
        tableWidget->insertRow(tableWidget->rowCount());
        int index = tableWidget->rowCount() - 1;

        QTableWidgetItem* item = new QTableWidgetItem();
        qulonglong hostPointer = reinterpret_cast<qulonglong>(newHost);
        item->setData(Qt::UserRole, hostPointer);
        tableWidget->setItem(index, 3, item);
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

    void HostList::updateHostListItem(int row) {
        Host* host = getHostPointer(row);

        unsigned long long packetsSnt = host->statistic->getItem(IPVersion::ALL, Protocol::EthernetII)->packetsSnt;
        unsigned long long packetsRcv = host->statistic->getItem(IPVersion::ALL, Protocol::EthernetII)->packetsRcv;

        unsigned long long bytesSnt = host->statistic->getItem(IPVersion::ALL, Protocol::EthernetII)->bytesSnt;
        unsigned long long bytesRcv = host->statistic->getItem(IPVersion::ALL, Protocol::EthernetII)->bytesRcv;

        tableWidget->setItem(row, 0, new QTableWidgetItem(host->getPreferedHostIdentifier().c_str()));

        QTableWidgetItem* itemPackets = new QTableWidgetItem();
        itemPackets->setData(Qt::EditRole, packetsSnt + packetsRcv);
        tableWidget->setItem(row, 1, itemPackets);

        tableWidget->setItem(row, 2, new QTableWidgetItem(QString::fromUtf8(NetUtil::getByteString(bytesSnt + bytesRcv).c_str())));

        tableWidget->sortByColumn(1, Qt::DescendingOrder);
    }

    void HostList::onUpdate(quint64 dt) {
        updateTimer += dt;
        if (updateTimer >= 300) {
            for (int i = 0; i < tableWidget->rowCount(); i++) {
                updateHostListItem(i);
            }

            updateTimer = 0;
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
            QPoint mousePos = mainWindow->mapFromGlobal(QCursor::pos());
            hostInfoPopup = new HostInfoPopup(host, QPoint(mainWindow->getUI()->widgetNetView->width(), mousePos.y()), mainWindow);
            hostInfoPopup->show();
        }
    }
}
