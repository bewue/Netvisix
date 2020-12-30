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


#include "Config.h"

#include <QDebug>


namespace Netvisix {
    namespace Config {

        Settings* Settings::instance = 0;

        Settings* Settings::SharedInstance() {
            if (instance == 0) {
                instance = new Settings();
            }

            return instance;
        }

        Settings::Settings() {
        }

        Settings::~Settings() {
        }

        void Settings::release() {
            if (instance != nullptr) {
                delete instance;
                instance = nullptr;
            }
        }


        Theme::Theme(ThemeID themeID) {
            this->themeID = themeID;

            switch (themeID) {
                case ThemeID::Light:
                    bgColor                         = QColor(160, 160, 160, 255);
                    lanAreaColor                    = QColor(170, 170, 170, 255);
                    lanAreaOutlineColor             = QColor(180, 180, 180, 255);

                    hostActiveColor                 = QColor(110, 110, 130, 255);
                    hostInactiveColor               = QColor(160, 160, 160, 255);
                    hostOutlineColor                = QColor(140, 140, 140, 255);
                    hostLocalInterfaceMarkerColor   = QColor(190, 190, 190, 255);

                    packetOutlineColor              = QColor(80, 80, 80, 255);

                    linkColor                       = QColor(200, 200, 200, 255);
                    break;

                case ThemeID::Dark:
                    bgColor                         = QColor(50, 50, 50, 255);
                    lanAreaColor                    = QColor(60, 60, 60, 255);
                    lanAreaOutlineColor             = QColor(80, 80, 80, 255);

                    hostActiveColor                 = QColor(110, 110, 130, 255);
                    hostInactiveColor               = QColor(110, 110, 130, 0);
                    hostOutlineColor                = QColor(120, 120, 120, 255);
                    hostLocalInterfaceMarkerColor   = QColor(60, 60, 60, 255);

                    packetOutlineColor              = QColor(120, 120, 120, 255);

                    linkColor                       = QColor(100, 100, 100, 255);
                    break;
            }
        }

        Theme::~Theme() {
        }

} // namespace Config
} // namespace Netvisix
