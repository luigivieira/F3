#include "facedatasetmodel.h"
#include <assert.h>

#include <QFileInfo>
#include <QApplication>
#include <QDebug>

// +-----------------------------------------------------------
f3::FaceDatasetModel::FaceDatasetModel(QObject *pParent):
	QAbstractListModel(pParent)
{
	m_pFaceDataset = new FaceDataset();
}

// +-----------------------------------------------------------
f3::FaceDatasetModel::~FaceDatasetModel()
{
	delete m_pFaceDataset;
}


// +-----------------------------------------------------------
int f3::FaceDatasetModel::rowCount(const QModelIndex &oParent) const
{
	Q_UNUSED(oParent);
	return m_pFaceDataset->size();
}

// +-----------------------------------------------------------
int f3::FaceDatasetModel::columnCount(const QModelIndex &oParent) const
{
	Q_UNUSED(oParent);
	return 2; // There are two columns of data: [Image Name], [Emotion Label]
}

// +-----------------------------------------------------------
QVariant f3::FaceDatasetModel::data(const QModelIndex &oIndex, int iRole) const
{
	QPixmap oImage;
	QString sFileName;
	EmotionLabel eLabel;

	switch(iRole)
	{
		// Listing textual data
		case Qt::DisplayRole:
			switch(oIndex.column())
			{
				case 0: // [Image Name]
					if(!m_pFaceDataset->getImageFileName(oIndex.row(), sFileName))
						return QVariant();
					else
						return QFileInfo(sFileName).baseName();

				case 1: // [Emotion Label]
					m_pFaceDataset->getEmotionLabel(oIndex.row(), eLabel);
					return eLabel.getName();

				default:
					return QVariant();
			}

		// Listing decoration data (image thumbnails)
		case Qt::DecorationRole:
			if(oIndex.column() == 0)
				return m_lCachedThumbnails[oIndex.row()];
			else
				return QVariant();

		// User role used to get original dataset data
		case Qt::UserRole:
			switch(oIndex.column())
			{
				case 0: // The complete image file name+path
					if(!m_pFaceDataset->getImageFileName(oIndex.row(), sFileName))
						return QVariant();
					else
						return sFileName;

				case 1: // The original image data
					if(!m_pFaceDataset->getImage(oIndex.row(), oImage))
						oImage = QPixmap(":/images/brokenimage");
					return oImage;

				default:
					return QVariant();
			}

		// Any other role, return invalid
		default:
			return QVariant();
	}
}

// +-----------------------------------------------------------
QVariant f3::FaceDatasetModel::headerData(int iSection, Qt::Orientation eOrientation, int iRole) const
{
	Q_UNUSED(eOrientation);
	switch(iRole)
	{
		// Display data (the dataset contents)
		case Qt::DisplayRole:
			switch(iSection)
			{
				case 0: // [Image]
					return QApplication::translate("FaceDatasetModel", "Imagem");

				case 1: // [Emotion Label]
					return QApplication::translate("FaceDatasetModel", "Rótulo Emocional");

				default:
					return QVariant();
			}
			

		case Qt::DecorationRole:
			if(iSection == 0)
				return QApplication::translate("FaceDatasetModel", "Miniatura");
			else
				return QVariant();

		default:
			return QVariant();
	}
}

// +-----------------------------------------------------------
bool f3::FaceDatasetModel::loadFromFile(const QString &sFileName, QString &sMsgError)
{
	beginResetModel();
	bool bRet = m_pFaceDataset->loadFromFile(sFileName, sMsgError);

	// Rebuild the thumbnails cache
	if(bRet)
	{
		m_lCachedThumbnails.clear();
		for(int i = 0; i < m_pFaceDataset->size(); i++)
			m_lCachedThumbnails.append(buildThumbnail(i));
	}

	endResetModel();
	return bRet;
}

// +-----------------------------------------------------------
bool f3::FaceDatasetModel::saveToFile(const QString &sFileName, QString &sMsgError) const
{
	return m_pFaceDataset->saveToFile(sFileName, sMsgError);
}

// +-----------------------------------------------------------
bool f3::FaceDatasetModel::addImages(const QStringList &lImageFiles)
{
	int iFirst = m_pFaceDataset->size();
	int iLast = iFirst + lImageFiles.size() - 1;
	beginInsertRows(QModelIndex(), iFirst, iLast);
	for(int i = 0; i < lImageFiles.size(); i++)
	{
		m_pFaceDataset->addImage(lImageFiles[i]);
		m_lCachedThumbnails.append(buildThumbnail(m_pFaceDataset->size() - 1));
	}
	endInsertRows();
	return true;
}

// +-----------------------------------------------------------
bool f3::FaceDatasetModel::removeImages(const QList<int> &lImageIndexes)
{
	int iFirst = lImageIndexes.first();
	int iLast = lImageIndexes.last();
	beginRemoveRows(QModelIndex(), iFirst, iLast);
	for(int i = lImageIndexes.size() - 1; i >= 0; i--)
	{
		m_pFaceDataset->removeImage(lImageIndexes[i]);
		m_lCachedThumbnails.removeAt(lImageIndexes[i]);
	}
	endRemoveRows();
	return true;
}

// +-----------------------------------------------------------
QPixmap f3::FaceDatasetModel::buildThumbnail(const int iIndex)
{
	QPixmap oImage;
	if(!m_pFaceDataset->getImage(iIndex, oImage))
		oImage = QPixmap(":/images/imagemissing");

	oImage = oImage.scaled(50, 50, Qt::IgnoreAspectRatio);
	return oImage;
}