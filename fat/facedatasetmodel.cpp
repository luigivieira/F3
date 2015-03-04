#include "facedatasetmodel.h"
#include <assert.h>

#include <QFileInfo>
#include <QApplication>
#include <QDebug>

// +-----------------------------------------------------------
f3::FaceDatasetModel::FaceDatasetModel(FaceDataset *pFaceDataset, QObject *pParent):
	QAbstractListModel(pParent)
{
	assert(pFaceDataset);
	m_pFaceDataset = pFaceDataset;
}

// +-----------------------------------------------------------
f3::FaceDatasetModel::~FaceDatasetModel(void)
{
}


// +-----------------------------------------------------------
int f3::FaceDatasetModel::rowCount(const QModelIndex &oParent) const
{
	return m_pFaceDataset->size();
}

// +-----------------------------------------------------------
int f3::FaceDatasetModel::columnCount(const QModelIndex &) const
{
	// There are two columns of data:
	// [Image], [Emotion Label]
	return 2;
}

// +-----------------------------------------------------------
QVariant f3::FaceDatasetModel::data(const QModelIndex &oIndex, int iRole) const
{
	QImage oImage;

	switch(iRole)
	{
		// Display data (the dataset contents)
		case Qt::DisplayRole:
			switch(oIndex.column())
			{
				case 0: // [Image]
					return QFileInfo(m_pFaceDataset->getImageFile(oIndex.row())).baseName();

				case 1: // [Emotion Label]
					return m_pFaceDataset->getEmotionLabel(oIndex.row()).getName();

				default:
					return QVariant();
			}

		// Decoration data (the image thumbnails)
		case Qt::DecorationRole:
			if(oIndex.column() == 0)
			{
				try
				{
					oImage = m_pFaceDataset->getImage(oIndex.row());
				}
				catch(...)
				{
					oImage = QImage(":/images/imagemissing");
				}
				//oImage = oImage.scaled(800, 600).scaled(100, 100, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
				oImage = oImage.scaled(50, 50);
				return oImage;
			}
			else
				return QVariant();

		default:
			return QVariant();
	}
}

// +-----------------------------------------------------------
QVariant f3::FaceDatasetModel::headerData(int iSection, Qt::Orientation eOrientation, int iRole) const
{
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
void f3::FaceDatasetModel::beginUpdate()
{
	beginResetModel();
}

// +-----------------------------------------------------------
void f3::FaceDatasetModel::endUpdate()
{
	endResetModel();
}