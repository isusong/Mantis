# 
#  Copyright 2012, 2013 Iowa State University
# 
#  This file is part of Mantis.
#  
#  Mantis is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#  
#  Mantis is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#  
#  You should have received a copy of the GNU General Public License
#  along with Mantis.  If not, see <http://www.gnu.org/licenses/>.
# 
#  Author: Laura Ekstrand (ldmil@iastate.edu)
# 

CONFIG      += qt
QT	    += opengl \
		   script

HEADERS		+= mainWindow.h \
			   graphicsWidget.h \
			   settingsDialog.h \
			   ../core/RangeImage.h \
			   ../core/Profile.h \
			   ../core/al3d_file.h \
			   ../core/CleaningCode/Clean.h \
			   ../core/CleaningCode/FastConnectComponent.h \
			   ../core/CleaningCode/FilterImage.h \
			   ../core/CleaningCode/SpikeRemoval.h \
			   ../core/CleaningCode/FindValidMarkRange.h \
			   ../core/CleaningCode/ManipulatePlate.h \
			   ../core/CleaningCode/ComputeFlatScrewdriverTipCsys.h \
                           ../core/logger.h \
			   cleaningDialog.h
SOURCES		+= mainWindow.cpp \
			   graphicsWidget.cpp \
			   settingsDialog.cpp \
			   ../core/RangeImage.cpp \
			   ../core/Profile.cpp \
			   ../core/al3d_file.cpp \
			   ../core/CleaningCode/Clean.cpp \
			   ../core/CleaningCode/FastConnectComponent.cpp \
			   ../core/CleaningCode/FilterImage.cpp \
			   ../core/CleaningCode/SpikeRemoval.cpp \
			   ../core/CleaningCode/FindValidMarkRange.cpp \
			   ../core/CleaningCode/ManipulatePlate.cpp \
			   ../core/CleaningCode/ComputeFlatScrewdriverTipCsys.cpp \
                           ../core/logger.cpp \
			   cleaningDialog.cpp \
			   main.cpp
FORMS		+= mainWindowForm.ui \
	           settingsDialog.ui \
			   cleaningDialog.ui
RESOURCES   += resources/gcaIcons.qrc
