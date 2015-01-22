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
#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include <QObject>
#include <fstream>

#include "core_global.h"

namespace f3
{
	/**
	 * Custom Qt application class, mainly used to intercept message notifications in the fff project applications.
	 * Made into a singleton in version 2.0.
	 */
	class CORE_EXPORT F3Application: public QApplication
	{
	private:
		/** File stream used to log application messages. */
		std::ofstream m_oLogFile;

		/** Instance for the singleton. */
		static F3Application* m_soInstance;

	public:
		/**
		 * Initiates the singleton object.
		 */
		static void initiate(int argc, char* argv[]);

		/**
		 * Runs the application on the singleton instance (encapsulating the exec method for easy
		 * and safe access).
		 */
		static int run();

		/**
		 * Handles the notification of messages in the application event loop.
		 * @param pReceiver Pointer to the QObject that shall receive the message.
		 * @param pEvent Pointer to the QEvent with the message information.
		 */
        bool notify(QObject* pReceiver, QEvent* pEvent);

	protected:
		/**
		 * Class constructor.
		 * @param argc Number of arguments received from the command line.
		 * @param argv Array of char pointers with the arguments received from the command line.
		 */
        F3Application(int argc, char* argv[]);

	private:
		/**
		 * Close the log file.
		 */
		void closeLog();

		/**
		 * Log and exception message handler for application events.
 		 * @param oType QtMsgType instance with the type of the log event.
		 * @param oContext QMessageLogContext instance with information on where the event happened (function, line, etc)
		 * @param sMsg QString instance with the event message.
		 */
		static void handleLogOutput(QtMsgType oType, const QMessageLogContext& oContext, const QString& sMsg);
	};
}

#endif // APPLICATION_H
