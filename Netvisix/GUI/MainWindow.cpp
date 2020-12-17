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

#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "Config.h"
#include "Net/NetEventManager.h"
#include "Net/NetUtil.h"
#include "GUI/StatusbarDisplay.h"
#include "GUI/VisibleHost.h"
#include "GUI/VisiblePacket.h"
#include "GUI/StartCapturePopup.h"

#include <QDebug>
#include <QDateTime>
#include <QTimer>
#include <QLabel>
#include <QMessageBox>
#include <QDesktopWidget>
#include <QGraphicsPixmapItem>


namespace Netvisix {


    MainWindow::MainWindow(QWidget *parent) :
            QMainWindow(parent),
            ui(new Ui::MainWindow) {

        QFont font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
        font.setPointSize(MainWindow::FONT_SIZE);
        QApplication::setFont(font);

        ui->setupUi(this);

        setWindowTitle(Config::T_APP_NAME.c_str());
        setWindowIcon(QPixmap(":/Resources/icon-256x256.png"));

        ui->buttonLegend->setIcon(QIcon(":/Resources/icon_legend.png"));
        ui->buttonLegend->setIconSize(QSize(16, 16));

        ui->buttonPause->setIcon(QIcon(":/Resources/icon_pause.png"));
        ui->buttonPause->setIconSize(QSize(13, 13));

        updateSniffingButton();

        lastTime = QDateTime::currentMSecsSinceEpoch();

        timer = new QTimer();
        QObject::connect(timer, SIGNAL(timeout()), this, SLOT(mainUpdate()));
        timer->start();

        statusbarDisplay = new StatusbarDisplay(this);

        initLegendDisplay();

        QDesktopWidget widget;
        QRect screenSize = widget.availableGeometry(widget.primaryScreen());
        QSize winSize = QSize(screenSize.width() * 0.75f, screenSize.height() * 0.85f);

        setGeometry((screenSize.width() - winSize.width()) / 2, (screenSize.height() - winSize.height()) * 0.35f, winSize.width(), winSize.height());

        MainWindow::updateAllWidgetFonts();
    }

    MainWindow::~MainWindow() {
        NetEventManager::release();
        delete statusbarDisplay;
        delete ui;
        delete timer;
    }

    void MainWindow::updateAllWidgetFonts() {
        foreach (QWidget *widget, QApplication::allWidgets()) {
            widget->setFont(QApplication::font());
            widget->update();
        }
    }

    void MainWindow::closeEvent(QCloseEvent *ce) {
        ce->ignore();
        showQuitPopup();
    }

    void MainWindow::mainUpdate() {
        quint64 currentTime = QDateTime::currentMSecsSinceEpoch();
        quint64 dt = currentTime - lastTime;
        lastTime = currentTime;

        ui->widgetNetView->onUpdate(dt);

        statusbarDisplay->updateStatusbar(dt);
    }

    void MainWindow::showInfoPopup(std::string text) {
        QMessageBox msgBox;
        msgBox.information(this, " ", text.c_str());
    }

    void MainWindow::showQuitPopup() {
        QMessageBox msgBox(this);
        std::string text = "Quit " + Config::T_APP_NAME + "?";
        QPushButton* buttonOk = msgBox.addButton(QMessageBox::Ok);
        msgBox.addButton(QMessageBox::Cancel);
        msgBox.setWindowTitle(" ");
        msgBox.setText(text.c_str());
        msgBox.exec();
        if (msgBox.clickedButton() == buttonOk) {
            QCoreApplication::quit();
        }
    }

    void MainWindow::showStopSniffingPopup() {
        QMessageBox msgBox(this);
        QPushButton* buttonOk = msgBox.addButton(QMessageBox::Ok);
        msgBox.addButton(QMessageBox::Cancel);
        msgBox.setWindowTitle(" ");
        msgBox.setText("Stop Sniffing?\r\n\r\nAll progress will be lost!");
        msgBox.exec();
        if (msgBox.clickedButton() == buttonOk) {
            // stop sniffing
            NetEventManager::SharedInstance()->reset();
            ui->widgetNetView->reset();
            ui->buttonPause->setChecked(false);
        }

        updateSniffingButton();
    }

    void MainWindow::updateSniffingButton() {
         if (NetEventManager::SharedInstance()->getIsSniffingRunning()) {
            ui->buttonStartStopSniffing->setIcon(QIcon(":/Resources/icon_stop.png"));
            ui->buttonPause->setEnabled(true);
         }
         else {
            ui->buttonStartStopSniffing->setIcon(QIcon(":/Resources/icon_start.png"));
            ui->buttonPause->setEnabled(false);
         }

         ui->buttonStartStopSniffing->setChecked(false);

         ui->buttonStartStopSniffing->setIconSize(QSize(13, 13));
    }

    void MainWindow::initLegendDisplay() {
        QGraphicsScene* gScene = new QGraphicsScene(this);

        ui->gvColorARP->setScene(gScene);
        ui->gvColorARP->setForegroundBrush(VisiblePacket::getPacketColor(Protocol::ARP));

        ui->gvColorICMP->setScene(gScene);
        ui->gvColorICMP->setForegroundBrush(VisiblePacket::getPacketColor(Protocol::ICMP));

        ui->gvColorTCP->setScene(gScene);
        ui->gvColorTCP->setForegroundBrush(VisiblePacket::getPacketColor(Protocol::TCP));

        ui->gvColorUDP->setScene(gScene);
        ui->gvColorUDP->setForegroundBrush(VisiblePacket::getPacketColor(Protocol::UDP));

        ui->gvColorOtherL2->setScene(gScene);
        ui->gvColorOtherL2->setForegroundBrush(VisiblePacket::getPacketColor(Protocol::OtherL2));

        ui->gvColorOtherL3->setScene(gScene);
        ui->gvColorOtherL3->setForegroundBrush(VisiblePacket::getPacketColor(Protocol::OtherL3));

        // ipv6
        QGraphicsScene* gSceneIPv6Packet = new QGraphicsScene(this);
        gSceneIPv6Packet->addPixmap(QPixmap(":/Resources/ipv6-packet.png"));
        ui->gvColorIPv6Packet->setScene(gSceneIPv6Packet);
        ui->gvColorIPv6Packet->setStyleSheet("background: transparent");

        ui->gvColorIPv6Multicast->setScene(gScene);
        ui->gvColorIPv6Multicast->setForegroundBrush(QBrush(Qt::darkGray, VisibleHost::MULTICAST_BRUSH_STYLE));
    }

} // namespace Netvisix


void Netvisix::MainWindow::on_buttonStartStopSniffing_clicked() {
    if (NetEventManager::SharedInstance()->getIsSniffingRunning()) {
        showStopSniffingPopup();
    }
    else {
        StartCapturePopup* startCapturePopup = new StartCapturePopup(this);
        startCapturePopup->show();
    }
}

void Netvisix::MainWindow::on_buttonPause_clicked() {
    bool pause = !ui->widgetNetView->getIsPaused();
    ui->widgetNetView->setIsPaused(pause);
    NetEventManager::SharedInstance()->setIsPaused(pause);
}

void Netvisix::MainWindow::on_buttonLegend_clicked() {
    bool showColorInfo = ui->widgetLegendDisplay->width() == 0;
    if (showColorInfo) {
        ui->widgetLegendDisplay->setMaximumWidth(widgetLegendWidth);
    }
    else {
        // hide legend
        widgetLegendWidth = ui->widgetLegendDisplay->width();
        ui->widgetLegendDisplay->setMaximumWidth(0);
    }
    update();
}

void Netvisix::MainWindow::on_actionQuit_triggered() {
    showQuitPopup();
}

void Netvisix::MainWindow::on_actionInfo_triggered() {
    QMessageBox msgBox(this);
    msgBox.addButton(QMessageBox::Ok);
    msgBox.setWindowTitle(" ");
    std::string text = Config::T_APP_NAME + " " + Config::T_APP_VERSION
            + "\r\n\r\n" + Config::T_APP_NAME + " is licensed under the " + Config::T_APP_LICENSE
            + "\r\n\r\nDeveloper: " + Config::T_AUTHOR_NAME
            + "\r\nContact: " + Config::T_AUTHOR_EMAIL
            + "\r\n\r\n" + Config::T_CODE_URL + "\r\n";
    msgBox.setText(text.c_str());
    msgBox.exec();
}

void Netvisix::MainWindow::on_actionReverseDNSLookup_triggered() {
    bool rDNSLookups = ui->actionReverseDNSLookup->isChecked();
    ui->widgetNetView->setReverseDNSLookupEnabled(rDNSLookups);
}
