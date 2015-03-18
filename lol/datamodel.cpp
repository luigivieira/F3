#include "datamodel.h"

#include <QApplication>

// +-----------------------------------------------------------
f3::DataModel::DataModel(QObject *pParent):
	QAbstractListModel(pParent)
{
	updateData();
}

// +-----------------------------------------------------------
f3::DataModel::~DataModel()
{
	m_oLogControl.disconnect();
}

// +-----------------------------------------------------------
bool f3::DataModel::updateData()
{
	m_mData.clear();
	if(!m_oLogControl.isConnected() && !m_oLogControl.connect())
		return false;

	QMap<QString, QtMsgType> mData;
	if(!m_oLogControl.getAppLogData(mData))
		return false;

	beginResetModel();
	m_mData.clear();
	m_mData = mData;
	endResetModel();
	return true;
}

// +-----------------------------------------------------------
int f3::DataModel::rowCount(const QModelIndex &oParent) const
{
	Q_UNUSED(oParent);
	return m_mData.size();
}

// +-----------------------------------------------------------
int f3::DataModel::columnCount(const QModelIndex &oParent) const
{
	Q_UNUSED(oParent);
	return 2; // Two columns of data: [App Name], [Log Level]
}

// +-----------------------------------------------------------
QVariant f3::DataModel::data(const QModelIndex &oIndex, int iRole) const
{
	if(iRole == Qt::DisplayRole && oIndex.row() < m_mData.size())
	{
		QMap<QString, QtMsgType>::const_iterator oItem = m_mData.begin() + oIndex.row();
		switch(oIndex.column())
		{
			case 0: // [App Name]
				return oItem.key();

			case 1: // [Log Level]
				switch(oItem.value())
				{
					case QtDebugMsg:
						return QApplication::translate("DataModel", "Depuração (DEBUG)");

					case QtWarningMsg:
						return QApplication::translate("DataModel", "Aviso (WARNING)");

					case QtCriticalMsg:
						return QApplication::translate("DataModel", "Crítico (CRITICAL)");

					case QtFatalMsg:
					default:
						return QApplication::translate("DataModel", "Fatal (FATAL)");
				};

			default:
				return QVariant();
		}
	}
	else
		return QVariant();
}

// +-----------------------------------------------------------
bool f3::DataModel::setData(const QModelIndex &oIndex, const QVariant &oValue, int iRole)
{
	if(iRole == Qt::UserRole)
	{
		QString sAppName = oIndex.sibling(oIndex.row(), 0).data(Qt::DisplayRole).toString();
		QtMsgType eLevel = (QtMsgType) oValue.toInt();

		QtMsgType eOldLevel;
		if(m_oLogControl.getLogLevel(sAppName, eOldLevel) && eLevel != eOldLevel)
		{
			if(m_oLogControl.setLogLevel(sAppName, eLevel))
			{
				m_mData[sAppName] = eLevel;
				emit dataChanged(oIndex, oIndex);
				return true;
			}
			else
				return false;
		}
		else
			return true;
	}
	else
		return false;
}

// +-----------------------------------------------------------
Qt::ItemFlags f3::DataModel::flags(const QModelIndex &oIndex) const
{
	switch(oIndex.column())
	{
		case 0: // [App Name]
			return Qt::ItemIsEnabled | Qt::ItemIsSelectable;

		case 1: // [Log Level]
			return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;

		default:
			return Qt::NoItemFlags;
	}
}

// +-----------------------------------------------------------
QVariant f3::DataModel::headerData(int iSection, Qt::Orientation eOrientation, int iRole) const
{
	Q_UNUSED(eOrientation);
	if(iRole == Qt::DisplayRole)
	{
		switch(iSection)
		{
			case 0: // [App Name]
				return QApplication::translate("DataModel", "Nome da Aplicação em Execução");

			case 1: // [Log Level]
				return QApplication::translate("DataModel", "Nível de Log Atual");

			default:
				return QVariant();
		}
	}
	else
		return QVariant();
}