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

HEADERS += \
	StatisticsLibrary/io/converttracetoint.h \
	StatisticsLibrary/base/flipcorrelation.h \
	StatisticsLibrary/base/FlippableCorLoc.h \
	StatisticsLibrary/base/intnolev_functors.h \
	StatisticsLibrary/base/intcorrelation.h \
	StatisticsLibrary/base/mydebug.h \
	StatisticsLibrary/base/random.h \
	StatisticsLibrary/base/mtrandom.h \
	StatisticsLibrary/base/stats.h \
	StatisticsLibrary/base/ValueLoc.h \
	StatisticsLibrary/base/corloc.h 
SOURCES += \
	StatisticsLibrary/base/FlippableCorLoc.cpp \
	StatisticsLibrary/base/mydebug.cpp \
	StatisticsLibrary/base/random.cpp \
	StatisticsLibrary/base/mt19937ar.cpp \
	StatisticsLibrary/base/stats.cpp \
	StatisticsLibrary/base/ValueLoc.cpp
