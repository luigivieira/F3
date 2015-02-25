#include "facedatasetmodel.h"
#include <assert.h>

#include <QFileInfo>
#include <QApplication>

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
	return 1;
}

// +-----------------------------------------------------------
QVariant f3::FaceDatasetModel::data(const QModelIndex &oIndex, int iRole) const
{
	QImage oImage;
	QFileInfo oFile;

	switch(iRole)
	{
		case Qt::DisplayRole:
			oFile = QFileInfo(m_pFaceDataset->getImageFile(oIndex.row()));
			return oFile.baseName();

		case Qt::DecorationRole:
			oImage = m_pFaceDataset->getImage(oIndex.row());
			// Scale the image to a thumbnail size
			//oImage = oImage.scaled(800, 600).scaled(100, 100, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
			oImage = oImage.scaled(50, 50);
			return oImage;

		default:
			return QVariant();
	}
}

// +-----------------------------------------------------------
QVariant f3::FaceDatasetModel::headerData(int iSection, Qt::Orientation eOrientation, int iRole) const
{
	switch(iRole)
	{
		case Qt::DisplayRole:
			return QApplication::translate("FaceDatasetModel", "Nome da Imagem");

		case Qt::DecorationRole:
			return QApplication::translate("FaceDatasetModel", "Miniatura");

		default:
			return QVariant();
	}
}