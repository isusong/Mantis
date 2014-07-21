# 
#  Copyright 2008-2014 Iowa State University
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

# TARGET = mantis

# ensure one "debug_and_release" in CONFIG, for clarity...
debug_and_release {
    CONFIG -= debug_and_release
    CONFIG += debug_and_release
}
    # ensure one "debug" or "release" in CONFIG so they can be used as
    #   conditionals instead of writing "CONFIG(debug, debug|release)"...
CONFIG(debug, debug|release) {
    CONFIG -= debug release
    CONFIG += debug
}
CONFIG(release, debug|release) {
    CONFIG -= debug release
    CONFIG += release
}

win32:CONFIG(release, debug|release): TARGET = mantis
else:win32:CONFIG(debug, debug|release): TARGET = mantisd
else: TARGET = mantis

QT += opengl \
	  script
unix:LIBS += -lGLU
win32:LIBS += -lGLU32
# CONFIG += debug
CONFIG += console
CONFIG += qwt


HEADERS += \
	../core/RangeImage.h \
	../core/al3d_file.h \
	../core/ScriptInterface.h \
	../core/CleaningCode/Clean.h \
	../core/CleaningCode/FastConnectComponent.h \
	../core/CleaningCode/FilterImage.h \
	../core/CleaningCode/SpikeRemoval.h \
	../core/CleaningCode/FindValidMarkRange.h \
	../core/CleaningCode/ManipulatePlate.h \
	../core/CleaningCode/ComputeFlatScrewdriverTipCsys.h \
	../core/CleaningCode/CleanVirtualMark.h \
	../core/Profile.h \
	../core/CsvTable.h \
	../core/VirtualTip.h \
	../core/StreamBuffer.h \
        ../core/logger.h \
        ../core/utlqt.h \
	../QtBoxesDemo/QGLExtensionWrangler/glextensions.h \
	../core/StatInterface.h \
	../StatisticsLibrary/io/converttracetoint.h \
	../StatisticsLibrary/base/flipcorrelation.h \
	../StatisticsLibrary/base/FlippableCorLoc.h \
	../StatisticsLibrary/base/intnolev_functors.h \
	../StatisticsLibrary/base/intcorrelation.h \
	../StatisticsLibrary/base/mydebug.h \
	../StatisticsLibrary/base/random.h \
	../StatisticsLibrary/base/mtrandom.h \
	../StatisticsLibrary/base/stats.h \
	../StatisticsLibrary/base/ValueLoc.h \
	../StatisticsLibrary/base/corloc.h \
	../gui/WindowManager.h \
	../core/View.h \
	../gui/qwt-plots/statPlot.h \
	../gui/RangeImageViewer.h \
	../gui/GraphicsWidget.h \
	../gui/RangeImageRenderer.h \
	../gui/GenericModel.h \
	../QtBoxesDemo/QTrackBall/scene.h \
	../QtBoxesDemo/QTrackBall/trackball.h \
	../gui/Investigator.h \
	../gui/StatisticsWidget.h \
	../gui/StatisticsSettingsDialog.h \
	../gui/InvestigatorSubWidget.h \
	../gui/TipWidget.h \
	../gui/PlateWidget.h \
	../gui/Selection.h \
    ../gui/SplitCmpThumbLoaderWidget.h \
    ../gui/SplitCmpViewCtrlsWidget.h \
    ../gui/QListWidgetItemEx.h \
    ../gui/QMdiSplitCmpWnd.h \
    ../core/box.h \
    ../core/vector3.h \
    ../core/basevector.h

SOURCES += \
	../core/RangeImage.cpp \
	../core/al3d_file.cpp \
	../core/ScriptInterface.cpp \
	../core/CleaningCode/Clean.cpp \
	../core/CleaningCode/FastConnectComponent.cpp \
	../core/CleaningCode/FilterImage.cpp \
	../core/CleaningCode/SpikeRemoval.cpp \
	../core/CleaningCode/FindValidMarkRange.cpp \
	../core/CleaningCode/ManipulatePlate.cpp \
	../core/CleaningCode/ComputeFlatScrewdriverTipCsys.cpp \
	../core/CleaningCode/CleanVirtualMark.cpp \
	../core/Profile.cpp \
	../core/CsvTable.cpp \
	../core/VirtualTip.cpp \
	../core/StreamBuffer.cpp \
        ../core/logger.cpp \
        ../core/utlqt.cpp \
	../QtBoxesDemo/QGLExtensionWrangler/glextensions.cpp \
	../core/StatInterface.cpp \
	../StatisticsLibrary/base/FlippableCorLoc.cpp \
	../StatisticsLibrary/base/mydebug.cpp \
	../StatisticsLibrary/base/random.cpp \
	../StatisticsLibrary/base/mt19937ar.cpp \
	../StatisticsLibrary/base/stats.cpp \
	../StatisticsLibrary/base/ValueLoc.cpp \
	../gui/WindowManager.cpp \
	../core/View.cpp \
	../gui/qwt-plots/statPlot.cpp \
	../gui/RangeImageViewer.cpp \
	../gui/GraphicsWidget.cpp \
	../gui/RangeImageRenderer.cpp \
	../gui/GenericModel.cpp \
	../QtBoxesDemo/QTrackBall/trackball.cpp \
	../gui/Investigator.cpp \
	../gui/StatisticsWidget.cpp \
	../gui/StatisticsSettingsDialog.cpp \
	../gui/InvestigatorSubWidget.cpp \
	../gui/TipWidget.cpp \
	../gui/PlateWidget.cpp \
	../gui/Selection.cpp \
	main.cpp \
    ../gui/SplitCmpThumbLoaderWidget.cpp \
    ../gui/SplitCmpViewCtrlsWidget.cpp \
    ../gui/QListWidgetItemEx.cpp \
    ../gui/QMdiSplitCmpWnd.cpp

RESOURCES += ../core/core.qrc \
			 ../gui/gui.qrc

FORMS += ../gui/StatisticsWidget.ui \
		 ../gui/StatisticsSettingsDialog.ui \
    ../gui/SplitCmpThumbLoaderWidget.ui \
    ../gui/SplitCmpViewCtrlsWidget.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../qwt_Qt481_Release/lib/ -lqwt
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../qwt_Qt481_Debug/lib/ -lqwtd
else:symbian: LIBS += -lqwt
else:unix: LIBS += -L$$PWD/../../../qwt_MSVC2008_Debug/lib/ -lqwt

INCLUDEPATH += $$PWD/../../../qwt-6.1/src
DEPENDPATH += $$PWD/../../../qwt-6.1/src

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../qwt_Qt481_Release/lib/ -lqwtmathml
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../qwt_Qt481_Debug/lib/ -lqwtmathmld
else:symbian: LIBS += -lqwtmathml
else:unix: LIBS += -L$$PWD/../../../qwt_MSVC2008_Debug/lib/ -lqwtmathml

INCLUDEPATH += $$PWD/../../../qwt-6.1/src
DEPENDPATH += $$PWD/../../../qwt-6.1/src
