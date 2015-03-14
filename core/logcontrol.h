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

#ifndef LOGCONTROL_H
#define LOGCONTROL_H

#include "core_global.h"

#include <QSharedMemory>

namespace f3
{
	/**
	 * Represents the different prototypical emotional labels.
	 * 
	 */
	class CORE_EXPORT LogControl
	{
	public:
		/**
		 * Class constructor.
		 */
		LogControl();
		
		/** 
		 * Class destructor.
		 */
		virtual ~LogControl();

		/**
		 * Establishes a "connection" with the log control system. That basically
		 * means to create or attach to the shared memory using for exchanging data.
		 * @return Boolean indicating if the operation was successful (true) or not (false).
		 */
		bool connect();

		/**
		 * Interrupts the "connection" with the log control system. That basically
		 * means to dettach from the shared memory using for exchanging data.
		 * @return Boolean indicating if the operation was successful (true) or not (false).
		 */
		bool disconnect();

		/**
		 * Queries the current log level of the application with the given name.
		 * @param sAppName QString with the application name to query.
		 * @param eLevel Reference to a QtMsgType enumeration to receive the application current log level.
		 * @return Boolean indicating if the operation was successful (true) or not (false).
		 * This method returns false if the log control has not been connected before or if
		 * the application with the given name has not been registered yet (it is probably not
		 * running).
		 */
		bool getLogLevel(const QString &sAppName, QtMsgType &eLevel);

		/**
		 * Updates the current log level of the application with the given name.
		 * This method automatically registers the application in the log control by creating
		 * an entry in the shared memory used to manage the data.
		 * @param sAppName QString with the application name to update.
		 * @param eLevel QtMsgType enumeration value with the new application log level.
		 * @return Boolean indicating if the operation was successful (true) or not (false).
		 * This method returns false if the log control has not been connected before.
		 */
		bool setLogLevel(const QString &sAppName, const QtMsgType eLevel);

		/**
		 * Queries the map of all application names and their current log levels.
		 * @param mAppLogData Reference to a QMap<QString, QtMsgType> to receive the
		 * mapping of all existing (running) application names and their current log levels.
		 * @return Boolean indicating if the operation was successful (true) or not (false).
		 * This method returns false if the log control has not been connected before.
		 */
		bool getAppLogData(QMap<QString, QtMsgType> &mAppLogData);

		/**
		 * Removes the entry of the given application name from the log control.
		 * This method is supposed to be called by every application before it is closed.
		 * @param sAppName QString with the application name to be removed from the shared memory.
		 * @return Boolean indicating if the operation was successful (true) or not (false).
		 * This method returns false if the log control has not been connected before. If the application
		 * name does not exist already in the shared memory, the operation returns as successful.
		 */
		bool removeAppEntry(const QString &sAppName);

	private:
		/** Shared memory used for inter-process communication. */
		QSharedMemory *m_pSharedMemory;

		/**
		 * Constant defining the size (in bytes) of the shared memory used
		 * for inter-process communication. The data in the shared memory is
		 * a QMap<String, int> (mapping the application name to the active log level)
		 * that is streaming from/to the shared memory. So, this size was estimated
		 * to allow storing at least 10 entries with app names of 60 digits.
		 */
		static const int SHARED_MEMORY_SIZE = 1048576;
	};
};

#endif// LOGCONTROL_H

