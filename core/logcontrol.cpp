#include "logcontrol.h"
#include <QBuffer>
#include <QDataStream>
#include <QMap>

// +-----------------------------------------------------------
f3::LogControl::LogControl()
{
	m_pSharedMemory = new QSharedMemory("f3_log_control");
}

// +-----------------------------------------------------------
f3::LogControl::~LogControl()
{
	delete m_pSharedMemory;
}


// +-----------------------------------------------------------
bool f3::LogControl::connect()
{
	// Try to connect to an existing shared memory. If it does not
	// exist yet, try to create it.
	bool bRet = m_pSharedMemory->attach();
	if(!bRet)
	{
		QSharedMemory::SharedMemoryError eError = m_pSharedMemory->error();
		if(eError == QSharedMemory::NotFound)
			bRet = m_pSharedMemory->create(SHARED_MEMORY_SIZE);
	}
	return bRet;
}

// +-----------------------------------------------------------
bool f3::LogControl::disconnect()
{
	if(m_pSharedMemory->isAttached())
		return m_pSharedMemory->detach();
	else
		return true;
}

// +-----------------------------------------------------------
bool f3::LogControl::isConnected()
{
	return m_pSharedMemory->isAttached();
}

// +-----------------------------------------------------------
bool f3::LogControl::getLogLevel(const QString &sAppName, QtMsgType &eLevel)
{
	// Try to lock the shared memory
	if(!m_pSharedMemory->lock())
		return false;

	// Read the map from the shared memory
	QBuffer oBuffer;
	QDataStream oStream(&oBuffer);

	oBuffer.setData((char *) m_pSharedMemory->constData(), m_pSharedMemory->size());
	oBuffer.open(QBuffer::ReadOnly);
	
	QMap<QString, int> mApps;
	oStream >> mApps;

	// Unlock the shared memory before returning
	m_pSharedMemory->unlock();

	// Return the stored value for the application name (if found on the table)
	int iLevel = mApps[sAppName];
	if(iLevel != 0)
	{
		eLevel = (QtMsgType) (iLevel - 1);
		return true;
	}
	else
		return false;
}

// +-----------------------------------------------------------
bool f3::LogControl::setLogLevel(const QString &sAppName, const QtMsgType eLevel)
{
	// Try to lock the shared memory
	if(!m_pSharedMemory->lock())
		return false;

	// First Read the map from the shared memory
	QBuffer oBuffer;
	QDataStream oStream(&oBuffer);

	oBuffer.setData((char *) m_pSharedMemory->constData(), m_pSharedMemory->size());
	oBuffer.open(QBuffer::ReadOnly);

	QMap<QString, int> mApps;
	oStream >> mApps;

	// Then update the value in the map with the new log level (if needed)
	int iLevel = mApps[sAppName];
	int iNewLevel = ((int) eLevel) + 1;

	// And save the map data to the shared memory
	if(iLevel == 0 || iLevel != iNewLevel)
	{
		mApps[sAppName] = iNewLevel;

		QBuffer oNewBuffer;
		oNewBuffer.open(QBuffer::ReadWrite);
		QDataStream oNewStream(&oNewBuffer);

		oNewStream << mApps;

		int iSize = oNewBuffer.size();
		if(iSize < SHARED_MEMORY_SIZE)
		{
			char *sDataTo = (char *) m_pSharedMemory->data();
			const char *sDataFrom = oNewBuffer.data().data();
			memset(sDataTo, '\0', SHARED_MEMORY_SIZE);
			memcpy(sDataTo, sDataFrom, iSize);
		}
		else // Oops! Not enough space in the shared memory
		{
			// Unlock the shared memory before returning failure
			m_pSharedMemory->unlock();
			return false;
		}
	}

	// Unlock the shared memory before returning
	m_pSharedMemory->unlock();
	return true;
}

// +-----------------------------------------------------------
bool f3::LogControl::getAppLogData(QMap<QString, QtMsgType> &mAppLogData)
{
	// Try to lock the shared memory
	if(!m_pSharedMemory->lock())
		return false;

	// Read the map from the shared memory
	QBuffer oBuffer;
	QDataStream oStream(&oBuffer);

	oBuffer.setData((char *) m_pSharedMemory->constData(), m_pSharedMemory->size());
	oBuffer.open(QBuffer::ReadOnly);
	
	QMap<QString, int> mApps;
	oStream >> mApps;

	// Unlock the shared memory before returning
	m_pSharedMemory->unlock();

	mAppLogData.clear();
	QMap<QString, int>::iterator it;
	for(it = mApps.begin(); it != mApps.end(); ++it)
		mAppLogData.insert(it.key(), (QtMsgType) (it.value() - 1));

	return true;
}

// +-----------------------------------------------------------
bool f3::LogControl::removeAppEntry(const QString &sAppName)
{
	// Try to lock the shared memory
	if(!m_pSharedMemory->lock())
		return false;

	// First Read the map from the shared memory
	QBuffer oBuffer;
	QDataStream oStream(&oBuffer);

	oBuffer.setData((char *) m_pSharedMemory->constData(), m_pSharedMemory->size());
	oBuffer.open(QBuffer::ReadOnly);

	QMap<QString, int> mApps;
	oStream >> mApps;

	// Then remove the entry in the map with the app name
	mApps.remove(sAppName);

	// Save the map data to the shared memory
	QBuffer oNewBuffer;
	oNewBuffer.open(QBuffer::ReadWrite);
	QDataStream oNewStream(&oNewBuffer);

	oNewStream << mApps;

	int iSize = oNewBuffer.size();
	if(iSize < SHARED_MEMORY_SIZE)
	{
		char *sDataTo = (char *) m_pSharedMemory->data();
		const char *sDataFrom = oNewBuffer.data().data();
		memset(sDataTo, '\0', SHARED_MEMORY_SIZE);
		memcpy(sDataTo, sDataFrom, iSize);
	}
	else // Oops! Not enough space in the shared memory
	{
		// Unlock the shared memory before returning failure
		m_pSharedMemory->unlock();
		return false;
	}

	// Unlock the shared memory before returning
	m_pSharedMemory->unlock();
	return true;
}