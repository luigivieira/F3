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
	// Initialize the application
	try
	{
		F3Application::initiate(argc, argv);
	}
	catch(std::exception &e)
	{
		printf("It was not possible to initiate the application due to a critical error:\n");
		printf(e.what());
		return -1;
	}
	
	// Create the main window (as a pointer because it must be deleted before the application to avoid double deletion)
	MainWindow *pMainWindow = new MainWindow();
	QObject::connect(F3Application::instance(), SIGNAL(statusMessageShown(const QString &, const int)), pMainWindow, SLOT(showStatusMessage(const QString &, const int)));
	pMainWindow->show();

	// Execute the application
	int iRet = F3Application::run();

	// Terminate the application and the program execution
	delete pMainWindow;
	F3Application::terminate();
	return iRet;
}
