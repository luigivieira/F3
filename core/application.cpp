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
#include <QSettings>

#include <iostream>
#include <stdexcept>

using namespace std;

// Initiate the singleton instance with null
f3::F3Application* f3::F3Application::m_spInstance = NULL;

// +-----------------------------------------------------------
void f3::F3Application::initiate(int argc, char* argv[], const QString &sAppName, const bool bCreateLog)
{
	if(!m_spInstance)
	{
		m_spInstance = new F3Application(argc, argv);
		m_spInstance->setup(sAppName, bCreateLog);
	}
}

// +-----------------------------------------------------------
void f3::F3Application::loadAndShowMainWindow(const QMainWindow *pMainWindow)
{
	m_pMainWindow = (QMainWindow*) pMainWindow;
	QSettings oSettings;
	m_pMainWindow->restoreGeometry(oSettings.value("geometry").toByteArray());
    m_pMainWindow->restoreState(oSettings.value("windowState").toByteArray());
	m_pMainWindow->show();
}

// +-----------------------------------------------------------
void f3::F3Application::terminate()
{
	if(m_spInstance)
	{
		m_spInstance->clean();
		delete m_spInstance;
		m_spInstance = NULL;
	}
}

// +-----------------------------------------------------------
void f3::F3Application::setup(const QString &sAppName, const bool bCreateLog)
{
	QCoreApplication::setOrganizationName("Fun from Faces");
    QCoreApplication::setOrganizationDomain("https://github.com/luigivieira/F3");
    QCoreApplication::setApplicationName(sAppName);

	m_sAppName = sAppName;
	m_bCreateLog = bCreateLog;
	m_pMainWindow = NULL;

	// Only install the message handler and initialize the log control if required
	if(m_bCreateLog)
	{
		if(!m_oLogControl.connect())
			throw runtime_error("Error initiating the inter-process communication");

		QString sLogFile = QCoreApplication::applicationFilePath();
		sLogFile.replace(".exe", ""); // For windows
		sLogFile += ".log";
    
		m_oLogFile.open(qPrintable(sLogFile), ios::app);
		if(!m_oLogFile.is_open())
		{
			QString sError = QString("Error opening log file [%1] for writing").arg(sLogFile);
			throw runtime_error(qPrintable(sError));
		}

		qInstallMessageHandler(&f3::F3Application::handleLogOutput);

		// Default to only log fatal messages
		m_eLogLevel = QtFatalMsg;
		m_oLogControl.setLogLevel(m_sAppName, m_eLogLevel);
	}
}

// +-----------------------------------------------------------
void f3::F3Application::clean()
{
	if(m_bCreateLog)
	{
		m_oLogFile.flush();
		m_oLogFile.close();
		m_oLogControl.removeAppEntry(m_sAppName);
		m_oLogControl.disconnect();
	}

	if(m_pMainWindow)
	{
		QSettings oSettings;
		oSettings.setValue("geometry", m_pMainWindow->saveGeometry());
		oSettings.setValue("windowState", m_pMainWindow->saveState());
		delete m_pMainWindow;
	}
}

// +-----------------------------------------------------------
int f3::F3Application::run()
{
	if(m_spInstance)
	{
		string sAppName = qPrintable(QCoreApplication::applicationFilePath());
		
		qDebug() << sAppName.c_str() << "started.";
		int iRet = m_spInstance->exec();
		qDebug() << sAppName.c_str() << "ended.";
		
		return iRet;
	}
	else
		throw runtime_error("F3Application singleton not initialized");
}

// +-----------------------------------------------------------
f3::F3Application* f3::F3Application::instance()
{
	return m_spInstance;
}

// +-----------------------------------------------------------
f3::F3Application::F3Application(int argc, char* argv[]):
    QApplication(argc, argv)
{
}

// +-----------------------------------------------------------
f3::F3Application::~F3Application()
{
}

// +-----------------------------------------------------------
bool f3::F3Application::notify(QObject* pReceiver, QEvent* pEvent)
{
    try
	{
		// Check if there is an update in the log level for this application
		if(m_bCreateLog)
		{
			QtMsgType eLevel;
			if(m_oLogControl.getLogLevel(m_sAppName, eLevel) && m_eLogLevel != eLevel)
				m_eLogLevel = eLevel;
		}

		// Retransmit the event notification
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
void f3::F3Application::handleLogOutput(QtMsgType eType, const QMessageLogContext &oContext, const QString &sMsg) {
	// Do not log the message if its type is bigger then the maximum configured log level
	if(eType < instance()->m_eLogLevel)
		return;

	QString sNow = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
	QString sSource = QFileInfo(QFile(oContext.file).fileName()).fileName();

	switch (eType)
	{
		case QtDebugMsg:
			m_spInstance->m_oLogFile << qPrintable(sNow) << " (" << qPrintable(sSource) << ":" << oContext.line << ", " << oContext.function << ") DEBUG: " << qPrintable(sMsg) << endl;
			m_spInstance->m_oLogFile.flush();
			break;

		case QtWarningMsg:
			m_spInstance->m_oLogFile << qPrintable(sNow) << " (" << qPrintable(sSource) << ":" << oContext.line << ", " << oContext.function << ") WARNING: " << qPrintable(sMsg) << endl;
			m_spInstance->m_oLogFile.flush();
			break;

		case QtCriticalMsg:
			m_spInstance->m_oLogFile << qPrintable(sNow) << " (" << qPrintable(sSource) << ":" << oContext.line << ", " << oContext.function << ") CRITICAL: " << qPrintable(sMsg) << endl;
			m_spInstance->m_oLogFile.flush();
			break;

		case QtFatalMsg:
			QApplication::beep();
			QMessageBox::critical(NULL, qApp->translate("Main", "Erro de Execução"), qApp->translate("Main", "Uma exceção grave ocorreu e a aplicação precisará ser encerrada. Por favor, verifique o arquivo de log para detalhes."), QMessageBox::Ok);

			cerr << qPrintable(sNow) << " (" << qPrintable(sSource) << ":" << oContext.line << ", " << oContext.function << ") FATAL: " << qPrintable(sMsg) << endl;
			m_spInstance->m_oLogFile << qPrintable(sNow) << " (" << qPrintable(sSource) << ":" << oContext.line << ", " << oContext.function << ") FATAL: " << qPrintable(sMsg) << endl;
			terminate();
			exit(-2);
	}
}

// +-----------------------------------------------------------
void f3::F3Application::showStatusMessage(const QString &sMsg, const int iTimeout)
{
	if(m_spInstance)
		m_spInstance->emit statusMessageShown(sMsg, iTimeout);
}