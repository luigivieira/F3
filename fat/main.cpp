/*
 * Copyright (C) 2015 Luiz Carlos Vieira (http://www.luiz.vieira.nom.br)
 *
 * This file is part of Fun from Faces (f3).
 *
 * f3 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * f3 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "mainwindow.h"
#include "application.h"

#include <QObject>

using namespace f3;

// +-----------------------------------------------------------
int main(int argc, char *argv[])
{
	F3Application::initiate(argc, argv);
	MainWindow w;
	
	// Connect the application to the main window, so status messages sent to the application
	// are displayed at the main window.
	QObject::connect(F3Application::instance(), SIGNAL(statusMessageShown(const QString &, const int)), &w, SLOT(showStatusMessage(const QString &, const int)));

	w.show();
	return F3Application::run();
}
