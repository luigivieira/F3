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
	connect(m_pFaceWidget, SIGNAL(onScaleFactorChanged(const double)), this, SLOT(onScaleFactorChanged(const double)));

	showImage(-1);

	m_pFaceDatasetModel = new FaceDatasetModel();
	m_pFaceSelectionModel = new QItemSelectionModel(m_pFaceDatasetModel);

	// Indicate that it is a brand new dataset (i.e. not yet saved to a file)
	setProperty("new", true);
}

// +-----------------------------------------------------------
f3::ChildWindow::~ChildWindow()
{
	delete m_pFaceSelectionModel;
	delete m_pFaceDatasetModel;
}

// +-----------------------------------------------------------
void f3::ChildWindow::addImages(const QStringList &lsFiles)
{
	if(m_pFaceDatasetModel->addImages(lsFiles))
		setWindowModified(true);
}

// +-----------------------------------------------------------
void f3::ChildWindow::removeImages(const QList<int> &lIndexes)
{
	if(m_pFaceDatasetModel->removeImages(lIndexes))
		setWindowModified(true);
}

// +-----------------------------------------------------------
QAbstractListModel* f3::ChildWindow::getModel() const
{
	return m_pFaceDatasetModel;
}

// +-----------------------------------------------------------
QItemSelectionModel* f3::ChildWindow::getSelectionModel() const
{
	return m_pFaceSelectionModel;
}


// +-----------------------------------------------------------
void f3::ChildWindow::showImage(const int iIndex)
{
	if(iIndex == -1)
		m_pFaceWidget->setPixmap(QPixmap(":/images/noface"));
	else
	{
		QVariant oData = m_pFaceDatasetModel->data(m_pFaceDatasetModel->index(iIndex, 1), Qt::UserRole);
		if(oData.isValid())
			m_pFaceWidget->setPixmap(oData.value<QPixmap>());
		else
			m_pFaceWidget->setPixmap(QPixmap(":/images/brokenimage"));
	}
}

// +-----------------------------------------------------------
bool f3::ChildWindow::save(QString &sMsgError)
{
	if(!m_pFaceDatasetModel->saveToFile(windowFilePath(), sMsgError))
		return false;

	setWindowModified(false);
	setProperty("new", QVariant()); // No longer a new dataset
	return true;
}

// +-----------------------------------------------------------
bool f3::ChildWindow::saveToFile(const QString &sFileName, QString &sMsgError)
{
	if(!m_pFaceDatasetModel->saveToFile(sFileName, sMsgError))
		return false;

	setWindowFilePath(sFileName);
	setWindowModified(false);
	setProperty("new", QVariant()); // No longer a new dataset
	return true;
}

// +-----------------------------------------------------------
bool f3::ChildWindow::loadFromFile(const QString &sFileName, QString &sMsgError)
{
	if(!m_pFaceDatasetModel->loadFromFile(qPrintable(sFileName), sMsgError))
		return false;

	setWindowFilePath(sFileName);
	setWindowModified(false);
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
void f3::ChildWindow::onScaleFactorChanged(const double dScaleFactor)
{
	Q_UNUSED(dScaleFactor);
	emit onZoomLevelChanged(getZoomLevel());
}