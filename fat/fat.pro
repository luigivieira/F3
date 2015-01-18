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
	TEMPLATE = vcapp
	QMAKE_LFLAGS += /INCREMENTAL:NO
} else {
	TEMPLATE = app
}

SOURCES += main.cpp \
           mainwindow.cpp

HEADERS += mainwindow.h

FORMS += mainwindow.ui

win32 {
	INCLUDEPATH += C:\opencv-2.4.10\build\include
	LIBS += -LC:\opencv-2.4.10\build\x86\vc12\lib
} else:unix {
	INCLUDEPATH += /usr/local/include
	LIBS += -L/usr/local/lib
}

INCLUDEPATH += ../core

CONFIG(debug, debug|release) {

	TARGET = fatd
	DESTDIR = ./debug
	OBJECTS_DIR = ./debug/tmp
	MOC_DIR = ./debug/tmp
	RCC_DIR = ./debug/tmp
	UI_DIR = ./debug/tmp

	LIBS += -L$$PWD/../core/debug
	LIBS += -lcored

	win32 {
		LIBS += -lopencv_core2410d \
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
	} else:unix {
		LIBS += -lopencv_core \
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

} else {

	TARGET = fat
	DESTDIR = ./release
	OBJECTS_DIR = ./release/tmp
	MOC_DIR = ./release/tmp
	RCC_DIR = ./release/tmp
	UI_DIR = ./release/tmp

	LIBS += -L$$PWD/../core/release
	LIBS += -lcore

	win32 {
		LIBS += -lopencv_core2410 \
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
	} else:unix {
		LIBS += -lopencv_core \
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
}

RESOURCES += resources.qrc
win32:RC_FILE = win_resources.rc
