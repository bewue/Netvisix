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

#include "StatusbarDisplay.h"
#include "GUI/MainWindow.h"
#include "ui_MainWindow.h"
#include "Net/NetEventManager.h"

#include <QDebug>
#include <QDateTime>
#include <QLabel>


namespace Netvisix {

    StatusbarDisplay::StatusbarDisplay(MainWindow* mainWindow) {
        this->mainWindow = mainWindow;

        lastUpdateTime = 0;

        labelHandledPacketsCount = CreateLabel();
        mainWindow->getUI()->statusBar->addWidget(labelHandledPacketsCount);

        labelVisibleHostsCount = CreateLabel();
        mainWindow->getUI()->statusBar->addWidget(labelVisibleHostsCount);

        labelFPS = CreateLabel();
        mainWindow->getUI()->statusBar->addWidget(labelFPS);

        fpsCounter = 0;
	}

	StatusbarDisplay::~StatusbarDisplay() {
        delete labelHandledPacketsCount;
        delete labelVisibleHostsCount;
        delete labelFPS;
	}

    QLabel* StatusbarDisplay::CreateLabel() {
        QLabel* label = new QLabel();
        QFont font = QFont();
        font.setPointSize(10);
        label->setFont(font);

        return label;
    }

    void StatusbarDisplay::updateStatusbar(quint64 dt) {
        fpsCounter++;

        if (QDateTime::currentMSecsSinceEpoch() - lastUpdateTime < 400) {
            return;
        }

        lastUpdateTime = QDateTime::currentMSecsSinceEpoch();

        std::string handledPackets = " Handled Packets: ";
        handledPackets += std::to_string(NetEventManager::SharedInstance()->getNetEventCount());
        labelHandledPacketsCount->setText(handledPackets.c_str());

        std::string activeHosts = " |  Hosts: ";
        activeHosts += std::to_string(mainWindow->getUI()->widgetNetView->getActiveHostsCount());
        labelVisibleHostsCount->setText(activeHosts.c_str());

        int fps = (float) fpsCounter * (1000.0 / 400.0);
        std::string fpsString = " |  FPS: ";
        fpsString += std::to_string(fps);
        labelFPS->setText(fpsString.c_str());
        fpsCounter = 0;
    }
	
} // namespace Netvisix
