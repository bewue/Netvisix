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

#include "StartCapturePopup.h"
#include "NetView.h"
#include "MainWindow.h"
#include "Config.h"
#include "ui_StartCapturePopup.h"
#include "Net/NetEventManager.h"
#include "Net/NetUtil.h"

#include <QtWidgets>


namespace Netvisix {

    StartCapturePopup::StartCapturePopup(MainWindow* parent) :
            QWidget(parent, Qt::Popup | Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint),
            ui(new Ui::StartCapturePopup) {

        mainWindow = parent;

        ui->setupUi(this);

        setWindowTitle("Start Capture");
        move(parent->pos().x() + parent->size().width() / 2 - size().width() / 2, parent->pos().y() + parent->size().height() * 0.2f);
        setFixedSize(QSize(460, 256));
        setWindowModality(Qt::ApplicationModal);
        setWindowState(Qt::WindowActive);

        interfaces = std::vector<Tins::NetworkInterface>();
        updateInterfacesList();

        ui->labelInfo->setStyleSheet("QLabel { color : red; }");

        MainWindow::updateAllWidgetFonts();
    }

    StartCapturePopup::~StartCapturePopup() {
        delete ui;
    }

    std::string StartCapturePopup::getInterfaceListName(Tins::NetworkInterface networkInterface) {
#ifdef Q_OS_WIN
        Tins::NetworkInterface::Info info = networkInterface.addresses();
        std::string ifListName = info.ip_addr.to_string();
        if (info.hw_addr != nullptr) {
            ifListName += " [" + info.hw_addr.to_string() + "]";
        }
        return ifListName;
#else
        return networkInterface.name();
#endif
    }

    void StartCapturePopup::updateInterfacesList() {
        interfaces.clear();
        ui->comboBoxInterfaces->clear();

        Tins::NetworkInterface defaultInterface;
        try {
            defaultInterface = Tins::NetworkInterface::default_interface();
            interfaces.push_back(defaultInterface);
            ui->comboBoxInterfaces->addItem(getInterfaceListName(defaultInterface).c_str());
        } catch (Tins::invalid_interface const &) {
            //mainWindow->showInfoPopup("Not Connected?");
        }

        std::vector<Tins::NetworkInterface> ifs = Tins::NetworkInterface::all();
        for (unsigned int i = 0; i < ifs.size(); i++) {
            if (ifs.at(i) != defaultInterface) {
                interfaces.push_back(ifs.at(i));
                std::string ifListName = getInterfaceListName(ifs.at(i));
                ui->comboBoxInterfaces->addItem(ifListName.c_str());
            }
        }
    }

} // namespace Netvisix

void Netvisix::StartCapturePopup::on_buttonStart_clicked() {
    std::string selectedInterface = interfaces.at(ui->comboBoxInterfaces->currentIndex()).name();
    std::string exceptionString = "";

    // check user privilegs
    if (NetUtil::CheckCurrentUserSniffingPrivilegs(selectedInterface, &exceptionString) == false) {
#ifdef Q_OS_WIN
        mainWindow->showInfoPopup(exceptionString);
#else
        mainWindow->showInfoPopup(std::string(exceptionString + "\r\n\r\nTry to run " + Config::T_APP_NAME + " with root privileges.").c_str());
#endif
        return;
    }

    // check subnet validity
    std::string subnetStringIPv4 = ui->lineEditNetworkIPv4->text().toStdString();
    std::string subnetStringIPv6 = ui->lineEditNetworkIPv6->text().toStdString();
    if (NetUtil::getIsSubnetStringIPv4Valid(subnetStringIPv4) == false) {
        ui->labelInfo->setText("Invalid IPv4 Subnet!");
        return;
    }

    if (NetUtil::getIsSubnetStringIPv6Valid(subnetStringIPv6) == false) {
        ui->labelInfo->setText("Invalid IPv6 Subnet!");
        return;
    }

    // start sniffing
    NetEventManager::SharedInstance()->startSniffing(selectedInterface, ui->checkBoxPromiscMode->isChecked(), subnetStringIPv4, subnetStringIPv6);

    mainWindow->updateSniffingButton();

    close();
}

void Netvisix::StartCapturePopup::on_buttonCancel_clicked() {
    mainWindow->updateSniffingButton();
    close();
}
