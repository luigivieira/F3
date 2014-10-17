#
# Copyright (C) 2014 Luiz Carlos Vieira (http://www.luiz.vieira.nom.br)
#
# This file is part of Fun-From-Face (f3).
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
	INCLUDEPATH += C:\opencv-2.4.9\opencv\build\include
	LIBS += -LC:\opencv-2.4.9\opencv\build\x86\vc12\lib
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
		LIBS += -lopencv_core249d \
				-lopencv_imgproc249d \
				-lopencv_highgui249d \
				-lopencv_ml249d \
				-lopencv_video249d \
				-lopencv_features2d249d \
				-lopencv_calib3d249d \
				-lopencv_objdetect249d \
				-lopencv_contrib249d \
				-lopencv_legacy249d \
				-lopencv_flann249d
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
		LIBS += -lopencv_core249 \
				-lopencv_imgproc249 \
				-lopencv_highgui249 \
				-lopencv_ml249 \
				-lopencv_video249 \
				-lopencv_features2d249 \
				-lopencv_calib3d249 \
				-lopencv_objdetect249 \
				-lopencv_contrib249 \
				-lopencv_legacy249 \
				-lopencv_flann249
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
