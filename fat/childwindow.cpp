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

#include "childwindow.h"
#include "emotionlabel.h"
#include "mainwindow.h"
#include "application.h"

#include <opencv2/core/core.hpp>

#include <QMessageBox>
#include <QGridLayout>
#include <QApplication>
#include <QtMath>
#include <QDebug>

#include <vector>

using namespace std;
using namespace cv;

// +-----------------------------------------------------------
f3::ChildWindow::ChildWindow(QWidget *pParent) :
    QWidget(pParent)
{
	setAutoFillBackground(true);
	setBackgroundRole(QPalette::Dark);
	setAttribute(Qt::WA_DeleteOnClose);

	QGridLayout *pLayout = new QGridLayout(this);
	pLayout->setMargin(0);
	setLayout(pLayout);

	m_pFaceWidget = new FaceWidget(this);
	pLayout->addWidget(m_pFaceWidget);

	m_pFaceWidget->setPixmap(QPixmap(":/images/noface"));

	m_pFaceDatasetModel = new FaceDatasetModel();
	m_pFaceSelectionModel = new QItemSelectionModel(m_pFaceDatasetModel);

	// Capture of relevant signals
	connect(m_pFaceWidget, SIGNAL(onScaleFactorChanged(const double)), this, SLOT(onScaleFactorChanged(const double)));
	connect(m_pFaceWidget, SIGNAL(onFaceFeaturesSelectionChanged()), this, SLOT(onFaceFeaturesSelectionChanged()));
	connect(m_pFaceWidget, SIGNAL(onFaceFeaturesChanged()), this, SLOT(onDataChanged()));
	connect(m_pFaceDatasetModel, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&, const QVector<int>&)), this, SLOT(onDataChanged()));
	connect(m_pFaceSelectionModel, SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)), this, SLOT(onCurrentChanged(const QModelIndex &, const QModelIndex &)));

	// Indicate that it is a brand new dataset (i.e. not yet saved to a file)
	setProperty("new", true);
	m_iCurrentImage = -1;
}

// +-----------------------------------------------------------
f3::ChildWindow::~ChildWindow()
{
	delete m_pFaceSelectionModel;
	delete m_pFaceDatasetModel;
}

// +-----------------------------------------------------------
f3::FaceDatasetModel* f3::ChildWindow::dataModel() const
{
	return m_pFaceDatasetModel;
}

// +-----------------------------------------------------------
QItemSelectionModel* f3::ChildWindow::selectionModel() const
{
	return m_pFaceSelectionModel;
}

// +-----------------------------------------------------------
bool f3::ChildWindow::save(QString &sMsgError)
{
	if(!m_pFaceDatasetModel->saveToFile(windowFilePath(), sMsgError))
		return false;

	onDataChanged(false);
	setProperty("new", QVariant()); // No longer a new dataset
	return true;
}

// +-----------------------------------------------------------
bool f3::ChildWindow::saveToFile(const QString &sFileName, QString &sMsgError)
{
	if(!m_pFaceDatasetModel->saveToFile(sFileName, sMsgError))
		return false;

	setWindowFilePath(sFileName);
	onDataChanged(false);
	setProperty("new", QVariant()); // No longer a new dataset
	return true;
}

// +-----------------------------------------------------------
bool f3::ChildWindow::loadFromFile(const QString &sFileName, QString &sMsgError)
{
	if(!m_pFaceDatasetModel->loadFromFile(qPrintable(sFileName), sMsgError))
		return false;

	setWindowFilePath(sFileName);
	onDataChanged(false);
	setProperty("new", QVariant()); // No longer a new dataset
	return true;
}

// +-----------------------------------------------------------
void f3::ChildWindow::setZoomLevel(const int iLevel)
{
	int iSteps = iLevel - 11; // Because 11 is the middle slider value, equivalent to the scale of 1.0 (100%)
	double dBase = iSteps < 0 ? FaceWidget::ZOOM_OUT_STEP : FaceWidget::ZOOM_IN_STEP;
	
	double dFactor = 1.0 * qPow(dBase, abs(iSteps));
	m_pFaceWidget->setScaleFactor(dFactor);
}

// +-----------------------------------------------------------
int f3::ChildWindow::getZoomLevel() const
{
	double dFactor = m_pFaceWidget->getScaleFactor();
	double dBase = dFactor < 1.0 ? FaceWidget::ZOOM_OUT_STEP : FaceWidget::ZOOM_IN_STEP;

	int iSteps = qCeil(qLn(abs(dFactor)) / qLn(dBase));
	if(dFactor > 1.0)
		return iSteps + 11;
	else
		return 11 - iSteps;
}

// +-----------------------------------------------------------
void f3::ChildWindow::zoomIn()
{
	m_pFaceWidget->zoomIn();
}

// +-----------------------------------------------------------
void f3::ChildWindow::zoomOut()
{
	m_pFaceWidget->zoomOut();
}

// +-----------------------------------------------------------
void f3::ChildWindow::onScaleFactorChanged(const double dScaleFactor)
{
	Q_UNUSED(dScaleFactor);

	QModelIndex oCurrent = m_pFaceSelectionModel->currentIndex();
	QString sImageName = m_pFaceDatasetModel->data(m_pFaceDatasetModel->index(oCurrent.row(), 0), Qt::UserRole).toString();
	EmotionLabel eEmotion = EmotionLabel::fromValue(m_pFaceDatasetModel->data(m_pFaceDatasetModel->index(oCurrent.row(), 1), Qt::UserRole).toInt());
	emit onUIUpdated(sImageName, eEmotion, getZoomLevel());
}

// +-----------------------------------------------------------
void f3::ChildWindow::updateEmotionLabel(const EmotionLabel eLabel)
{
	QModelIndex oIndex = m_pFaceSelectionModel->currentIndex();
    m_pFaceDatasetModel->setData(m_pFaceDatasetModel->index(oIndex.row(), 1), QVariant(eLabel.getValue()), Qt::UserRole);
}

// +-----------------------------------------------------------
void f3::ChildWindow::onDataChanged(const bool bModified)
{
	if(bModified)
		updateFeaturesInDataset();
	setWindowModified(bModified);
	emit onDataModified();
}

// +-----------------------------------------------------------
void f3::ChildWindow::onCurrentChanged(const QModelIndex &oCurrent, const QModelIndex &oPrevious)
{
	Q_UNUSED(oPrevious);

	if(!oCurrent.isValid())
	{
		m_iCurrentImage = -1;
		m_pFaceWidget->setPixmap(QPixmap(":/images/noface"));
		emit onUIUpdated("", EmotionLabel::UNDEFINED, 0);
	}
	else
	{
		m_iCurrentImage = oCurrent.row();
		QVariant oData = m_pFaceDatasetModel->data(m_pFaceDatasetModel->index(m_iCurrentImage, 2), Qt::UserRole);
		if(oData.isValid())
			m_pFaceWidget->setPixmap(oData.value<QPixmap>());
		else
			m_pFaceWidget->setPixmap(QPixmap(":/images/brokenimage"));

		QString sImageName = oCurrent.data(Qt::UserRole).toString();
		EmotionLabel eEmotion = EmotionLabel::fromValue(m_pFaceDatasetModel->data(m_pFaceDatasetModel->index(m_iCurrentImage, 1), Qt::UserRole).toInt());
		emit onUIUpdated(sImageName, eEmotion, getZoomLevel());

		refreshFeaturesInWidget();
	}
}

// +-----------------------------------------------------------
void f3::ChildWindow::refreshFeaturesInWidget()
{
	vector<FaceFeature*> vFeats = m_pFaceDatasetModel->getFeatures(m_iCurrentImage);
	QList<FaceFeatureNode*> lsNodes = m_pFaceWidget->getFaceFeatures(m_pFaceDatasetModel->numFeatures());
	for(int i = 0; i < (int) vFeats.size(); i++)
	{
		lsNodes[i]->setData(0, true); // Indication to avoid emitting position change event
		lsNodes[i]->setPos(vFeats[i]->x, vFeats[i]->y);
		lsNodes[i]->setData(0, false);
	}
}

// +-----------------------------------------------------------
void f3::ChildWindow::updateFeaturesInDataset()
{
	QList<FaceFeatureNode*> lsNodes = m_pFaceWidget->getFaceFeatures();
	vector<FaceFeature*> vFeats = m_pFaceDatasetModel->getFeatures(m_iCurrentImage);

	FaceFeatureNode* pNode;
	for(int i = 0; i < (int) vFeats.size(); i++)
	{
		if(i >= lsNodes.size()) // Sanity check (vFeats and lsNodes are supposed to have the same size, but who knows?)
		{
			qCritical() << tr("An update of face features in dataset was not performed due to inconsistences.");
			continue;
		}
		pNode = lsNodes.at(i);
		vFeats[i]->x = pNode->x();
		vFeats[i]->y = pNode->y();
	}
}

// +-----------------------------------------------------------
void f3::ChildWindow::onFaceFeaturesSelectionChanged()
{
	emit onFeaturesSelectionChanged();
}

// +-----------------------------------------------------------
bool f3::ChildWindow::displayFaceFeatures() const
{
	return m_pFaceWidget->displayFaceFeatures();
}
// +-----------------------------------------------------------
void f3::ChildWindow::setDisplayFaceFeatures(const bool bValue)
{
	m_pFaceWidget->setDisplayFaceFeatures(bValue);
}

// +-----------------------------------------------------------
bool f3::ChildWindow::displayConnections() const
{
	return m_pFaceWidget->displayConnections();
}

// +-----------------------------------------------------------
void f3::ChildWindow::setDisplayConnections(const bool bValue)
{
	m_pFaceWidget->setDisplayConnections(bValue);
}

// +-----------------------------------------------------------
bool f3::ChildWindow::displayFeatureIDs() const
{
	return m_pFaceWidget->displayFeatureIDs();
}

// +-----------------------------------------------------------
void f3::ChildWindow::setDisplayFeatureIDs(const bool bValue)
{
	m_pFaceWidget->setDisplayFeatureIDs(bValue);
}

// +-----------------------------------------------------------
const QList<f3::FaceFeatureNode*>& f3::ChildWindow::getFaceFeatures() const
{
	return m_pFaceWidget->getFaceFeatures();
}

// +-----------------------------------------------------------
QList<f3::FaceFeatureNode*> f3::ChildWindow::getSelectedFeatures() const
{
	return m_pFaceWidget->getSelectedFeatures();
}

// +-----------------------------------------------------------
QList<f3::FaceFeatureEdge*> f3::ChildWindow::getSelectedConnections() const
{
	return m_pFaceWidget->getSelectedConnections();
}

// +-----------------------------------------------------------
void f3::ChildWindow::setContextMenu(QMenu *pMenu)
{
	m_pFaceWidget->setContextMenu(pMenu);
}

// +-----------------------------------------------------------
void f3::ChildWindow::addFeature(const QPoint &oPos)
{
	FaceFeatureNode *pNode = m_pFaceWidget->addFaceFeature(oPos, true);
	m_pFaceDatasetModel->addFeature(pNode->x(), pNode->y());
	onDataChanged();
}

// +-----------------------------------------------------------
void f3::ChildWindow::removeSelectedFeatures()
{
	bool bUpdated = false;
	QList<FaceFeatureNode*> lsFeats = m_pFaceWidget->getSelectedFeatures();
	foreach(FaceFeatureNode *pNode, lsFeats)
	{
		m_pFaceWidget->removeFaceFeature(pNode);
		bUpdated = true;
	}
	if(bUpdated)
		onDataChanged();
}

// +-----------------------------------------------------------
void f3::ChildWindow::connectFeatures()
{
	bool bUpdated = false;
	QList<FaceFeatureNode*> lsFeats = m_pFaceWidget->getSelectedFeatures();
	QList<FaceFeatureNode*>::iterator oFirst, oSecond;

	for(oFirst = lsFeats.begin(); oFirst != lsFeats.end(); oFirst++)
	{
		for(oSecond = oFirst + 1; oSecond != lsFeats.end(); oSecond++)
		{
			m_pFaceWidget->connectFaceFeatures(*oFirst, *oSecond);
			bUpdated = true;
		}
	}
	if(bUpdated)
		onDataChanged();
}

// +-----------------------------------------------------------
void f3::ChildWindow::disconnectFeatures()
{
	bool bUpdated = false;
	QList<FaceFeatureNode*> lsFeats = m_pFaceWidget->getSelectedFeatures();
	QList<FaceFeatureNode*>::iterator oFirst, oSecond;

	for(oFirst = lsFeats.begin(); oFirst != lsFeats.end(); oFirst++)
	{
		for(oSecond = oFirst + 1; oSecond != lsFeats.end(); oSecond++)
		{
			m_pFaceWidget->disconnectFaceFeatures(*oFirst, *oSecond);
			bUpdated = true;
		}
	}
	if(bUpdated)
		onDataChanged();
}