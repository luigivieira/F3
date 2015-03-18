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
f3::FaceDatasetModel::~FaceDatasetModel()
{
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
	return 2; // There are two columns of data: [Image], [Emotion Label]
}

// +-----------------------------------------------------------
QVariant f3::FaceDatasetModel::data(const QModelIndex &oIndex, int iRole) const
{
	QImage oImage;
	QString sFileName;
	EmotionLabel eLabel;

	switch(iRole)
	{
		// Display data (the dataset contents)
		case Qt::DisplayRole:
			switch(oIndex.column())
			{
				case 0: // [Image]
					if(!m_pFaceDataset->getImageFileName(oIndex.row(), sFileName))
						return QVariant();
					else
						return QFileInfo(sFileName).baseName();

				case 1: // [Emotion Label]
					m_pFaceDataset->getEmotionLabel(oIndex.row(), eLabel);
					return eLabel.getName(); // If previous line's call fails, the label will be Undefined anyway...

				default:
					return QVariant();
			}

		// Decoration data (the image thumbnails)
		case Qt::DecorationRole:
			if(oIndex.column() == 0)
			{
				if(!m_pFaceDataset->getImage(oIndex.row(), oImage))
					oImage = QImage(":/images/imagemissing");
				else
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
void f3::FaceDatasetModel::beginUpdate()
{
	beginResetModel();
}

// +-----------------------------------------------------------
void f3::FaceDatasetModel::endUpdate()
{
	endResetModel();
}