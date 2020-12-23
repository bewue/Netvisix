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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <tins.h>

#include <QGraphicsScene>
#include <QMainWindow>


namespace Ui {
class MainWindow;
}

namespace Netvisix {

    class PacketHandler;
    class StatusbarDisplay;
    class HostList;

    class MainWindow : public QMainWindow {

    public:
        explicit MainWindow(QWidget *parent = 0);
        virtual ~MainWindow();

        static void updateAllWidgetFonts();

        Ui::MainWindow* getUI() { return this->ui; }

        void showInfoPopup(std::string text);
        void showQuitPopup();
        void showStopSniffingPopup();

        void updateSniffingButton();

    public slots:
         void mainUpdate();

    protected:
         void resizeEvent(QResizeEvent *event);

    private slots:
         void on_buttonStartStopSniffing_clicked();
         void on_buttonPause_clicked();
         void on_buttonLegend_clicked();

         void on_buttonHostList_clicked();

         void on_actionQuit_triggered();
         void on_actionInfo_triggered();

         void on_actionReverseDNSLookup_triggered();

    private:
        Q_OBJECT

        void closeEvent(QCloseEvent* ce);

        void updateStatusbar(quint64 dt);

        void initLegendDisplay();

        Ui::MainWindow *ui;

        PacketHandler* packetHandler;

        QTimer* timer;

        quint64 lastTime;

        StatusbarDisplay* statusbarDisplay;

        float widgetLegendWidth;

        HostList* hostList;
        float widgetHostListWidth;

#ifdef Q_OS_WIN
        const float FONT_SIZE = 9;
#else
        const float FONT_SIZE = 9;
#endif
    };

} // namespace Netvisix
#endif // MAINWINDOW_H
