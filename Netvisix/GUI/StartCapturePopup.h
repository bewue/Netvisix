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

#ifndef StartCapturePopup_H
#define StartCapturePopup_H

#include <tins.h>

#include <QWidget>


namespace Ui {
class StartCapturePopup;
}

namespace Netvisix {

    class MainWindow;

    class StartCapturePopup : public QWidget {

        public:
            explicit StartCapturePopup(MainWindow* parent);
            ~StartCapturePopup();

        private slots:
            void on_buttonStart_clicked();
            void on_buttonCancel_clicked();

        private:
            Q_OBJECT

            void updateInterfacesList();
            std::string getInterfaceListName(Tins::NetworkInterface networkInterface);
            std::vector<Tins::NetworkInterface> interfaces;

            Ui::StartCapturePopup *ui;

            MainWindow* mainWindow;
    };

} // namespace Netvisix
#endif // StartCapturePopup_H
