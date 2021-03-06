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
	return 4; // There are four columns of data: [Image Name], [Emotion Label]
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
QVariant f3::FaceDatasetModel::data(const QModelIndex &oIndex, int iRole) const
{
	FaceImage *pImage = m_pFaceDataset->getImage(oIndex.row());
	if(!pImage)
		return QVariant();

	QPixmap oPixmap;

	switch(iRole)
	{
		// Data to be displayed on ModelViews
		case Qt::DisplayRole:
			switch(oIndex.column())
			{
				case 0: // [Image Name]
					return QFileInfo(pImage->fileName()).baseName();

				case 1: // [Emotion Label Name]
					return pImage->emotionLabel().getName();

				default:
					return QVariant();
			}

		// Decoration data (image thumbnails for the first column)
		case Qt::DecorationRole:
			if(oIndex.column() == 0)
				return m_lCachedThumbnails[oIndex.row()];
			else
				return QVariant();

		// Raw data from the face dataset
		case Qt::UserRole:
			switch(oIndex.column())
			{
				case 0: // The complete image file name+path
					return pImage->fileName();

				case 1: // The emotion label
					return pImage->emotionLabel().getValue();

				case 2: // The image data
					oPixmap = pImage->pixMap();
					if(oPixmap.isNull())
						oPixmap = QPixmap(":/images/brokenimage");
					return oPixmap;

				default:
					return QVariant();
			}

		// Any other role, return invalid
		default:
			return QVariant();
	}
}

// +-----------------------------------------------------------
bool f3::FaceDatasetModel::setData(const QModelIndex &oIndex, const QVariant &oValue, int iRole)
{
	FaceImage *pImage = m_pFaceDataset->getImage(oIndex.row());
	if(!pImage)
		return false;

	if(iRole == Qt::UserRole)
	{
		EmotionLabel eLabel;

		switch(oIndex.column())
		{
			case 0: // [Image Name]
				return false;

			case 1: // [Emotion Label]
				pImage->setEmotionLabel(EmotionLabel::fromValue(oValue.toInt()));
				emit dataChanged(oIndex, oIndex);
				return true;

			default:
				return false;
		}
	}
	else
		return false;
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
void f3::FaceDatasetModel::addFeature(int iID, float x, float y)
{
	m_pFaceDataset->addFeature(iID, x, y);
}

// +-----------------------------------------------------------
void f3::FaceDatasetModel::removeFeature(const int iIndex)
{
	m_pFaceDataset->removeFeature(iIndex);
}

// +-----------------------------------------------------------
std::vector<f3::FaceFeature*> f3::FaceDatasetModel::getFeatures(const int iIndex)
{
	return m_pFaceDataset->getImageFeatures(iIndex);
}

// +-----------------------------------------------------------
int f3::FaceDatasetModel::numFeatures() const
{
	return m_pFaceDataset->numFeatures();
}

// +-----------------------------------------------------------
QPixmap f3::FaceDatasetModel::buildThumbnail(const int iIndex)
{
	QPixmap oImage;
	FaceImage *pImage = m_pFaceDataset->getImage(iIndex);
	if(!pImage)
		oImage = QPixmap(":/images/imagemissing");
	else
		oImage = pImage->pixMap();

	oImage = oImage.scaled(50, 50, Qt::IgnoreAspectRatio);
	return oImage;
}

// +-----------------------------------------------------------
Qt::ItemFlags f3::FaceDatasetModel::flags(const QModelIndex &oIndex) const
{
	switch(oIndex.column())
	{
		case 0: // [Image Name]
			return Qt::ItemIsEnabled | Qt::ItemIsSelectable;

		case 1: // [Emotion Label]
			return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;

		default:
			return Qt::NoItemFlags;
	}
}
