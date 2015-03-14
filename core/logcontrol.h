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

		bool connect();

		bool disconnect();

		bool getLogLevel(const QString &sAppName, QtMsgType &eLevel);

		bool setLogLevel(const QString &sAppName, const QtMsgType eLevel);

		bool getAppLogData(QMap<QString, QtMsgType> &mAppLogData);

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

