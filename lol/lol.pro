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
           mainwindow.cpp \
           aboutwindow.cpp \
           datamodel.cpp \
           leveldelegate.cpp
           
HEADERS += mainwindow.h \
           aboutwindow.h \
           datamodel.h \
           leveldelegate.h

FORMS += mainwindow.ui \
         aboutwindow.ui

win32 {
	INCLUDEPATH += C:\opencv-2.4.10\build\include
	LIBS += -LC:\opencv-2.4.10\build\x86\vc11\lib
} else:unix {
	INCLUDEPATH += /usr/local/include
	LIBS += -L/usr/local/lib
}

DEPENDS += ../core
INCLUDEPATH += ../core

CONFIG(debug, debug|release) {

	TARGET = f3lold
	DESTDIR = ../debug
	OBJECTS_DIR = ../debug/tmp/lol
	MOC_DIR = ../debug/tmp/lol
	RCC_DIR = ../debug/tmp/lol
	UI_DIR = ../debug/tmp/lol

	LIBS += -L../debug
	LIBS += -lf3cored

} else {

	TARGET = f3lol
	DESTDIR = ../release
	OBJECTS_DIR = ../release/tmp/lol
	MOC_DIR = ../release/tmp/lol
	RCC_DIR = ../release/tmp/lol
	UI_DIR = ../release/tmp/lol

	LIBS += -L../release
	LIBS += -lf3core
    
}

RESOURCES += resources.qrc
win32:RC_FILE = win_resources.rc

unix {
    target.path = /usr/local/bin
    INSTALLS += target
}