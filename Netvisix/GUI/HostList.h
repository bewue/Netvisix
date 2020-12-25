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

#ifndef HostList_H
#define HostList_H

#include "Net/IPreparedNetEventListener.h"

#include <QWidget>
#include <QTableWidget>
#include <QCheckBox>

#include <set>
#include <mutex>


namespace Netvisix {

    class MainWindow;
    class HostInfoPopup;

    class HostList : public QObject, public IPreparedNetEventListener {
        Q_OBJECT

        public:
            explicit HostList(MainWindow* mainWindow);
            ~HostList();

            virtual void onPreparedNetEventNewHost(Host* newHost);
            virtual void onPreparedNetEventNewUnicastPacket(Host* sender, Host* receiver, NetEvent* netEvent);
            virtual void onPreparedNetEventNewMulticastPacket(Host* sender, NetEvent* netEvent);
            virtual void onHostAddrUpdate(Host* host);

            void reset();

        private slots:
            void tableCellEntered(int row, int column);
            void updateLoop();

        private:
            Host* getHostPointer(int row);

            void addNewHostListItems();

            void updateAllHostListItems(bool force);

            void updateHostListItem(int row, bool force);

            MainWindow* mainWindow;

            QTableWidget* tableWidget;

            quint64 lastUpdateTime;

            HostInfoPopup* hostInfoPopup;

            QCheckBox* cbShowAll;

            std::vector<Host*> newHosts;
            std::set<Host*> hostsToUpdate;

            //std::mutex mutex;
    };

} // namespace Netvisix
#endif // HostList_H
