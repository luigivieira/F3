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
#include "application.h"
#include <typeinfo>
#include <QtGlobal>
#include <QDateTime>
#include <QFileInfo>
#include <QDebug>
#include <QMessageBox>
#include <iostream>
#include <stdexcept>

using namespace std;

// Initiate the singleton instance with null
f3::F3Application* f3::F3Application::m_soInstance = NULL;

// +-----------------------------------------------------------
void f3::F3Application::initiate(int argc, char* argv[])
{
	if(m_soInstance == NULL)
		m_soInstance = new F3Application(argc, argv);
}

// +-----------------------------------------------------------
int f3::F3Application::run()
{
	if(m_soInstance != NULL)
	{
		string sAppName = qPrintable(QCoreApplication::applicationFilePath());
		
		qDebug() << sAppName.c_str() << "started.";
		int iRet = m_soInstance->exec();
		qDebug() << sAppName.c_str() << "ended.";

		m_soInstance->closeLog();
		return iRet;
	}
	else
		throw runtime_error("F3Application singleton not initialized");
}

// +-----------------------------------------------------------
f3::F3Application::F3Application(int argc, char* argv[]):
    QApplication(argc, argv)
{
	QString sLogFile = QCoreApplication::applicationFilePath();
	sLogFile.replace(".exe", ""); // For windows
	sLogFile += ".log";
    
	m_oLogFile.open(qPrintable(sLogFile), ios::app);
	if(!m_oLogFile.is_open())
	{
		QString sError = QString("Error opening log file: %1").arg(sLogFile);
		throw runtime_error(qPrintable(sError));
	}

	qInstallMessageHandler(&f3::F3Application::handleLogOutput);
}

// +-----------------------------------------------------------
void f3::F3Application::closeLog()
{
	m_oLogFile.flush();
	m_oLogFile.close();
}

// +-----------------------------------------------------------
bool f3::F3Application::notify(QObject* pReceiver, QEvent* pEvent)
{
    try
	{
        return QApplication::notify(pReceiver, pEvent);
    }
    catch (std::exception &e)
	{
        qFatal("Exception %s sending event [%s] to object [%s] (%s)",
            e.what(), typeid(*pEvent).name(), qPrintable(pReceiver->objectName()), typeid(*pReceiver).name());
    }
    catch (...)
	{
        qFatal("Exception sending event [%s] to object [%s] (%s)",
            typeid(*pEvent).name(), qPrintable(pReceiver->objectName()), typeid(*pReceiver).name());
    }

    return false;
}

// +-----------------------------------------------------------
void f3::F3Application::handleLogOutput(QtMsgType oType, const QMessageLogContext& oContext, const QString& sMsg) {
	QString sNow = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
	QString sSource = QFileInfo(QFile(oContext.file).fileName()).fileName();

	switch (oType)
	{
		case QtDebugMsg:
			m_soInstance->m_oLogFile << qPrintable(sNow) << " (" << qPrintable(sSource) << ":" << oContext.line << ", " << oContext.function << ") DEBUG: " << qPrintable(sMsg) << endl;
			m_soInstance->m_oLogFile.flush();
			break;

		case QtWarningMsg:
			m_soInstance->m_oLogFile << qPrintable(sNow) << " (" << qPrintable(sSource) << ":" << oContext.line << ", " << oContext.function << ") WARNING: " << qPrintable(sMsg) << endl;
			m_soInstance->m_oLogFile.flush();
			break;

		case QtCriticalMsg:
			m_soInstance->m_oLogFile << qPrintable(sNow) << " (" << qPrintable(sSource) << ":" << oContext.line << ", " << oContext.function << ") CRITICAL: " << qPrintable(sMsg) << endl;
			m_soInstance->m_oLogFile.flush();
			break;

		case QtFatalMsg:
			QApplication::beep();
			QMessageBox::critical(NULL, qApp->translate("Main", "Erro de Execução"), qApp->translate("Main", "Uma exceção grave ocorreu e a aplicação precisará ser encerrada. Por favor, verifique o arquivo de log para detalhes."), QMessageBox::Ok);

			cerr       << qPrintable(sNow) << " (" << qPrintable(sSource) << ":" << oContext.line << ", " << oContext.function << ") FATAL: " << qPrintable(sMsg) << endl;
			m_soInstance->m_oLogFile << qPrintable(sNow) << " (" << qPrintable(sSource) << ":" << oContext.line << ", " << oContext.function << ") FATAL: " << qPrintable(sMsg) << endl;
			terminate();
			exit(-2);
	}
}