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

#ifndef STATUSBARDISPLAY_H
#define STATUSBARDISPLAY_H

#include <QMainWindow>
#include <QLabel>


namespace Netvisix {

    class MainWindow;

	class StatusbarDisplay {
		
		public:
            StatusbarDisplay(MainWindow* mainWindow);
            virtual ~StatusbarDisplay();

            void updateStatusbar(quint64 dt);
			
		protected:
			
		private:
            QLabel* CreateLabel();

            MainWindow* mainWindow;

            quint64 lastUpdateTime;

            QLabel* labelHandledPacketsCount;
            QLabel* labelVisibleHostsCount;
            QLabel* labelFPS;

            int fpsCounter;
			
	};
	
} // namespace Netvisix
#endif // STATUSBARDISPLAY_H
