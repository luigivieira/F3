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
#include "logcontrol.h"

namespace f3
{
	/**
	 * Custom Qt application class, mainly used to intercept message notifications in the fff project applications.
	 * Made into a singleton in version 2.0.
	 */
	class CORE_EXPORT F3Application: public QApplication
	{
		Q_OBJECT
	private:
		/** File stream used to log application messages. */
		std::ofstream m_oLogFile;

		/** Instance for the singleton. */
		static F3Application* m_spInstance;

		/** Current maximum log level for the application. */
		QtMsgType m_eLogLevel;

	public:
		/**
		 * Initiates the singleton object.
		 * This method throws a std::runtime_error exception if something bad
		 * happens during initialization (i.e. if the log file can be opened/created or the
		 * inter-process communication can not be established).
		 * @param argc Integer with the number of parameters received from the command line.
		 * @param argv String array with the list of parameters received from the command line.
		 */
		static void initiate(int argc, char* argv[]);

		/**
		 * Terminates the singleton object.
		 */
		static void terminate();

		/**
		 * Runs the application on the singleton instance (encapsulating the exec method for easy
		 * and safe access).
		 */
		static int run();

		/**
		 * Returns the static instance of the singleton, so the application
		 * can be connected outside for one or more signals.
		 */
		static F3Application* instance();

		/**
		 * Displays a text message in the main window status bar.
		 * @param sMsg QString with the message to be displayed.
		 * @param iTimeout Integer with the number of miliseconds
		 * by which the message will be displayed. The default is 5000
		 * (i.e. 5 seconds).
		 */
		static void showStatusMessage(const QString &sMsg, const int iTimeout = 5000);

		/**
		 * Handles the notification of messages in the application event loop.
		 * @param pReceiver Pointer to the QObject that shall receive the message.
		 * @param pEvent Pointer to the QEvent with the message information.
		 */
        bool notify(QObject* pReceiver, QEvent* pEvent);

	signals:

		/**
		 * Signal indicating that a status message was requested to be displayed on the
		 * application main window.
		 * @param sMsg QString with the message to be displayed.
		 * @param iTimeout Integer with the number of miliseconds
		 * by which the message will be displayed.
		 */
		void statusMessageShown(const QString &sMsg, const int iTimeout);

	protected:
		/**
		 * Class constructor.
		 * @param argc Number of arguments received from the command line.
		 * @param argv Array of char pointers with the arguments received from the command line.
		 */
        F3Application(int argc, char* argv[]);

		/** 
		 * Class destructor.
		 */
		virtual ~F3Application();

		/**
		 * Configure the instance of the application.
		 */
		void setup();

		/**
		 * Clean the instance of the application.
		 */
		void clean();

		/**
		 * Log and exception message handler for application events.
 		 * @param eType QtMsgType enum value with the type of the log event.
		 * @param oContext QMessageLogContext instance with information on where the event happened (function, line, etc)
		 * @param sMsg QString instance with the event message.
		 */
		static void handleLogOutput(QtMsgType eType, const QMessageLogContext& oContext, const QString& sMsg);

	private:

		/** Object used to allow the external update of the log level. */
		LogControl m_oLogControl;

		/** Name of the application, used for the log control via inter-process communication. */
		static const QString APP_NAME;
	};
}

#endif // APPLICATION_H
