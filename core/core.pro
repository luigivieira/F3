#
# Copyright (C) 2015 Luiz Carlos Vieira (http://www.luiz.vieira.nom.br)
#
# This file is part of Fun from Faces (f3).
#
# f3 is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# f3 is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.
#

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

win32 {
	TEMPLATE = vclib
	QMAKE_LFLAGS += /INCREMENTAL:NO
} else {
	TEMPLATE = lib
}

CONFIG += staticlib

SOURCES += application.cpp

HEADERS += application.h

win32 {
	INCLUDEPATH += C:\opencv-2.4.10\build\include
	LIBS += -LC:\opencv-2.4.10\build\x86\vc12\lib
} else:unix {
	INCLUDEPATH += /usr/local/include
	LIBS += -L/usr/local/lib
}

CONFIG(debug, debug|release) {

	TARGET = cored
	DESTDIR = ./debug
	OBJECTS_DIR = ./debug/tmp
	MOC_DIR = ./debug/tmp
	RCC_DIR = ./debug/tmp
	UI_DIR = ./debug/tmp

	win32.LIBS += -lopencv_core2410d \
				  -lopencv_imgproc2410d \
				  -lopencv_highgui2410d \
				  -lopencv_ml2410d \
				  -lopencv_video2410d \
				  -lopencv_features2d2410d \
				  -lopencv_calib3d2410d \
				  -lopencv_objdetect2410d \
				  -lopencv_contrib2410d \
				  -lopencv_legacy2410d \
				  -lopencv_flann2410d

	unix.LIBS += -lopencv_core \
				 -lopencv_imgproc \
				 -lopencv_highgui \
				 -lopencv_ml \
				 -lopencv_video \
				 -lopencv_features2d \
				 -lopencv_calib3d \
				 -lopencv_objdetect \
				 -lopencv_contrib \
				 -lopencv_legacy \
				 -lopencv_flann

} else {

	TARGET = core
	DESTDIR = ./release
	OBJECTS_DIR = ./release/tmp
	MOC_DIR = ./release/tmp
	RCC_DIR = ./release/tmp
	UI_DIR = ./release/tmp

	win32.LIBS += -lopencv_core2410 \
				  -lopencv_imgproc2410 \
				  -lopencv_highgui2410 \
				  -lopencv_ml2410 \
				  -lopencv_video2410 \
				  -lopencv_features2d2410 \
				  -lopencv_calib3d2410 \
				  -lopencv_objdetect2410 \
				  -lopencv_contrib2410 \
				  -lopencv_legacy2410 \
				  -lopencv_flann2410

	unix.LIBS += -lopencv_core \
				 -lopencv_imgproc \
				 -lopencv_highgui \
				 -lopencv_ml \
				 -lopencv_video \
				 -lopencv_features2d \
				 -lopencv_calib3d \
				 -lopencv_objdetect \
				 -lopencv_contrib \
				 -lopencv_legacy \
				 -lopencv_flann
}