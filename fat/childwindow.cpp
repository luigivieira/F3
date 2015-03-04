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

#include <opencv2/core/core.hpp>

#include <QMessageBox>
#include <QScrollArea>

#include <vector>

using namespace std;
using namespace cv;

// +-----------------------------------------------------------
f3::ChildWindow::ChildWindow(QWidget* pParent) :
    QWidget(pParent),
	ui(new Ui::ChildWindow)
{
    ui->setupUi(this);
	setAutoFillBackground(true);
	setBackgroundRole(QPalette::Dark);
	setAttribute(Qt::WA_DeleteOnClose);

	showImage(-1);
	m_iCurrentImage = -1;

	m_pFaceDataset = new FaceDataset();
	m_pFaceDatasetModel = new FaceDatasetModel(m_pFaceDataset);
	m_pFaceSelectionModel = new QItemSelectionModel(m_pFaceDatasetModel);

	// Indicate that it is a brand new dataset (i.e. not yet saved to a file)
	setProperty("new", true);
}

// +-----------------------------------------------------------
f3::ChildWindow::~ChildWindow()
{
	delete m_pFaceSelectionModel;
	delete m_pFaceDatasetModel;
	delete m_pFaceDataset;
	delete ui;
}

// +-----------------------------------------------------------
void f3::ChildWindow::addImages(const QStringList &lsFiles)
{
	int iExisting = 0;
	int iError = 0;
	QString sError, sExisting;
	bool bModified = false;

	m_pFaceDatasetModel->beginUpdate();

	for(int i = 0; i < lsFiles.size(); i++)
	{
		try
		{
			m_pFaceDataset->addImage(qPrintable(lsFiles[i]));
			bModified = true;
		}
		catch(invalid_argument)
		{
			sError = lsFiles[i];
			iError++;
		}
		catch(runtime_error)
		{
			sExisting = lsFiles[i];
			iExisting++;
		}
	}

	m_pFaceDatasetModel->endUpdate();

	if(iError == 1)
		QMessageBox::warning(NULL, tr("Erro adicionando imagens"), tr("A imagem [%1] não pode ser adicionada ao banco de faces anotadas. Por favor, verifique o arquivo de log para detalhes.").arg(sError), QMessageBox::Ok);
	else if(iError > 1)
		QMessageBox::warning(NULL, tr("Erro adicionando imagens"), tr("Um total de %1 imagens não puderam ser adicionadas ao banco de faces anotadas. Por favor, verifique o arquivo de log para detalhes.").arg(iError), QMessageBox::Ok);

	if(iExisting == 1)
		((MainWindow*) parentWidget())->showStatusMessage(tr("A imagem [%1] já existia no banco de faces anotadas e não foi adicionada novamente. Por favor, verifique o arquivo de log para detalhes.").arg(sExisting));
	else if(iExisting > 1)
		((MainWindow*) parentWidget())->showStatusMessage(tr("Um total de %1 imagens já existia no banco de faces anotadas e não foram adicionadas novamente. Por favor, verifique o arquivo de log para detalhes.").arg(iExisting));

	if(bModified)
		setWindowModified(true);
}

// +-----------------------------------------------------------
void f3::ChildWindow::removeImages(const std::vector<int> &vIndexes)
{
	m_pFaceDatasetModel->beginUpdate();
	for(int i = (int) vIndexes.size() - 1; i >=0; i--)
	{
		try
		{
			m_pFaceDataset->removeImage(vIndexes[i]);
		}
		catch(...) // An exception would happen only in case of invalid indexes...
		{
		}
	}
	m_pFaceDatasetModel->endUpdate();
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
	{
		ui->image->setPixmap(QPixmap(":/images/noface"));
		ui->image->adjustSize();
	}
	else
	{
		QImage oImage;
		try
		{
			oImage = m_pFaceDataset->getImage(iIndex);
		}
		catch(...)
		{
			ui->image->setPixmap(QPixmap(":/images/brokenimage"));
			ui->image->adjustSize();
			return;
		}

		ui->image->setPixmap(QPixmap().fromImage(oImage));
		ui->image->adjustSize();
	}
}

// +-----------------------------------------------------------
void f3::ChildWindow::save()
{
	m_pFaceDataset->saveToFile(qPrintable(windowFilePath()));
	setWindowModified(false);
	setProperty("new", QVariant()); // No longer a new dataset
}

// +-----------------------------------------------------------
void f3::ChildWindow::saveToFile(const QString &sFileName)
{
	m_pFaceDataset->saveToFile(qPrintable(sFileName));
	setWindowFilePath(sFileName);
	setWindowModified(false);
	setProperty("new", QVariant()); // No longer a new dataset
}

// +-----------------------------------------------------------
void f3::ChildWindow::loadFromFile(const QString &sFileName)
{
	m_pFaceDataset->loadFromFile(qPrintable(sFileName));
	setWindowFilePath(sFileName);
	setWindowModified(false);
	setProperty("new", QVariant()); // No longer a new dataset
}