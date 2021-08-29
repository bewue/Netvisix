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

#ifndef CONFIG_H
#define CONFIG_H


#include <QWidget>


namespace Netvisix {
    namespace Config {

        static const std::string T_APP_NAME             = "Netvisix";
        static const std::string T_APP_VERSION          = "1.4.1";
        static const std::string T_APP_LICENSE          = "GPLv3";
        static const std::string T_AUTHOR_NAME          = "Benjamin W.";
        static const std::string T_AUTHOR_EMAIL         = "bitbatzen@gmail.com";
        static const std::string T_WWW                  = "https://github.com/bewue/Netvisix";


        enum class ThemeID {
            Light,
            Dark,
        };

        class Theme {

            public:
                Theme(ThemeID themeID);
                virtual ~Theme();

                ThemeID themeID;

                QColor bgColor;
                QColor lanAreaColor;
                QColor lanAreaOutlineColor;

                QColor hostActiveColor;
                QColor hostInactiveColor;
                QColor hostOutlineColor;
                QColor hostLocalInterfaceMarkerColor;

                QColor packetOutlineColor;

                QColor linkColor;
        };


        class Settings {

            public:
                virtual ~Settings();
                static Settings* SharedInstance();

                static void release();

                void reset();

                Theme* theme;

            private:
                Settings();

                static Settings* instance;
        };

} // namespace Config
} // namespace Netvisix
#endif // CONFIG_H
