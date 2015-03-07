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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "aboutwindow.h"
#include "childwindow.h"
#include "utils.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <vector>

#include <QApplication>
#include <QImage>
#include <QFileDialog>
#include <QDesktopServices>
#include <QAction>
#include <QMessageBox>

using namespace cv;
using namespace std;

// +-----------------------------------------------------------
f3::MainWindow::MainWindow(QWidget *pParent) :
    QMainWindow(pParent),
    ui(new Ui::MainWindow)
{
	// Setup the UI
    ui->setupUi(this);
    setWindowState(Qt::WindowMaximized);
    m_pAbout = NULL;

    setWindowIcon(QIcon(":/icons/fat"));
	ui->tabWidget->setAutoFillBackground(true);
	ui->tabWidget->setBackgroundRole(QPalette::Midlight);

	// Setup the TAB changing events
	connect(ui->tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(on_tabCloseRequested(int)));
	connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(on_tabChanged(int)));

	// Add the view dropdown button manually (because Qt designer does not allow it...)
	m_pViewButton = new QMenu(ui->imagesToolbar);
	ui->imagesToolbar->addSeparator();
	ui->imagesToolbar->addAction(m_pViewButton->menuAction());

	QAction *pViewDetails = new QAction(QIcon(":/icons/viewdetails"), tr("Detalhes"), this);
	m_pViewButton->addAction(pViewDetails);
	QAction *pViewIcons = new QAction(QIcon(":/icons/viewicons"), tr("Ícones"), this);
	m_pViewButton->addAction(pViewIcons);

	QSignalMapper *pMap = new QSignalMapper(ui->imagesToolbar);
	connect(pViewDetails, SIGNAL(triggered()), pMap, SLOT(map()));
	connect(pViewIcons, SIGNAL(triggered()), pMap, SLOT(map()));
	pMap->setMapping(pViewDetails, QString("details"));
	pMap->setMapping(pViewIcons, QString("icons"));

	connect(pMap, SIGNAL(mapped(QString)), this, SLOT(setImageListView(QString)));
	connect(m_pViewButton->menuAction(), SIGNAL(triggered()), this, SLOT(toggleImageListView()));

	m_pViewButton->setIcon(QIcon(":/icons/viewicons")); // By default display the image thumbnails
	ui->treeImages->setVisible(false);

	// Update the statuses of UI elements
	updateUI();

	// Initialize other variables
	m_sDocumentsPath = QDir::toNativeSeparators(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)) + QDir::separator();
}

// +-----------------------------------------------------------
f3::MainWindow::~MainWindow()
{
    if(!m_pAbout)
        delete m_pAbout;
	if(m_pViewButton)
		delete m_pViewButton;
    delete ui;
}

// +-----------------------------------------------------------
void f3::MainWindow::on_actionNew_triggered()
{
    ChildWindow *pChild = new ChildWindow(this);

	pChild->setWindowFilePath(QString(tr("%1novo banco de faces anotadas.afd")).arg(m_sDocumentsPath));
	pChild->setWindowModified(true);
	pChild->setWindowIcon(QIcon(":/icons/face-dataset"));

	int iIndex = ui->tabWidget->addTab(pChild, pChild->windowIcon(), "");
	ui->tabWidget->setCurrentIndex(iIndex);
}

// +-----------------------------------------------------------
void f3::MainWindow::on_actionOpen_triggered()
{
    QString sFile = QFileDialog::getOpenFileName(this, tr("Abrir banco de faces anotadas..."), m_sDocumentsPath, tr("Arquivos de banco de faces anotadas (*.afd);; Todos os arquivos (*.*)"));
    if(sFile.length())
	{
		sFile = QDir::toNativeSeparators(sFile);
		int iPage = getFilePageIndex(sFile);
		if(iPage != -1)
		{
			ui->tabWidget->setCurrentIndex(iPage);
			showStatusMessage(QString(tr("O arquivo [%1] já está aberto no editor")).arg(Utils::shortenPath(sFile)));
		}
		else
		{
			ChildWindow *pChild = new ChildWindow(this);

			QString sDocPath = QDir::toNativeSeparators(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)) + QDir::separator();
			pChild->setWindowFilePath(sFile);
			pChild->setWindowModified(false);
			pChild->setWindowIcon(QIcon(":/icons/face-dataset"));

			int iIndex = ui->tabWidget->addTab(pChild, pChild->windowIcon(), "");
			ui->tabWidget->setCurrentIndex(iIndex);
		}
	}
}

// +-----------------------------------------------------------
void f3::MainWindow::on_actionSave_triggered()
{
	ChildWindow *pChild = (ChildWindow*) ui->tabWidget->currentWidget();
	if(!pChild || !pChild->isWindowModified())
		return;

	// Force the user to chose a file name if the dataset has not yet been saved
	if(pChild->property("new").toBool())
	{
		on_actionSaveAs_triggered();
		return;
	}
		
	pChild->save();
	updateUI();
}

// +-----------------------------------------------------------
void f3::MainWindow::on_actionSaveAs_triggered()
{
	ChildWindow *pChild = (ChildWindow*) ui->tabWidget->currentWidget();
	if(!pChild)
		return;

	QString sFileName = QFileDialog::getSaveFileName(this, tr("Salvar banco de faces anotadas..."), windowFilePath(), tr("Arquivos de banco de faces anotadas (*.afd);; Todos os arquivos (*.*)"));
    if(sFileName.length())
	{
		pChild->saveToFile(sFileName);
		updateUI();
	}
}

// +-----------------------------------------------------------
void f3::MainWindow::on_actionExit_triggered()
{
    QApplication::exit(0);
}

// +-----------------------------------------------------------
void f3::MainWindow::on_actionProject_triggered()
{
	QDesktopServices::openUrl(QUrl("https://github.com/luigivieira/F3"));
}

// +-----------------------------------------------------------
void f3::MainWindow::on_actionAbout_triggered()
{
	(new AboutWindow(this))->show();
}

// +-----------------------------------------------------------
void f3::MainWindow::on_tabCloseRequested(int iTabIndex)
{
	ChildWindow* pChild = (ChildWindow*) ui->tabWidget->widget(iTabIndex);
	ui->tabWidget->removeTab(iTabIndex);
	delete pChild;
}

// +-----------------------------------------------------------
void f3::MainWindow::on_tabChanged(int iTabIndex)
{
	updateUI(true); // In case the current tab is changed, force the update of the view model
}

// +-----------------------------------------------------------
void f3::MainWindow::on_actionAddImage_triggered()
{
	ChildWindow *pChild = (ChildWindow*) ui->tabWidget->currentWidget();
	if(!pChild)
		return;

    QStringList lsFiles = QFileDialog::getOpenFileNames(this, tr("Selecionar imagens de faces..."), m_sDocumentsPath, tr("Arquivos comuns de imagens (*.bmp *.png *.jpg *.gif);; Todos os arquivos (*.*)"));
	if(lsFiles.size())
	{
		pChild->addImages(lsFiles);
		if(!pChild->getSelectionModel()->currentIndex().isValid())
			pChild->getSelectionModel()->setCurrentIndex(pChild->getModel()->index(0, 0), QItemSelectionModel::Select);
	}
}

// +-----------------------------------------------------------
void f3::MainWindow::on_actionRemoveImage_triggered()
{
	ChildWindow *pChild = (ChildWindow*) ui->tabWidget->currentWidget();
	if(!pChild)
		return;

	QModelIndexList lsSelected = pChild->getSelectionModel()->selectedRows();
	if(lsSelected.size() > 0)
	{
		QString sMsg;
		if(lsSelected.size() == 1)
			sMsg = tr("Você confirma a remoção da imagem selecionada?");
		else
			sMsg = tr("Você confirma a remoção das %1 imagens selecionadas?").arg(lsSelected.size());
		if(QMessageBox::question(this, tr("Confirmação da remoção"), sMsg, QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes)
		{
			vector<int> vIndexes;
			for(int i = 0; i < lsSelected.size(); i++)
				vIndexes.push_back(lsSelected[i].row());
			pChild->removeImages(vIndexes);
			updateUI();
		}
	}
}

// +-----------------------------------------------------------
void f3::MainWindow::showStatusMessage(const QString &sMsg, int iTimeout)
{
	ui->statusBar->showMessage(sMsg, iTimeout);
}

// +-----------------------------------------------------------
int f3::MainWindow::getFilePageIndex(const QString &sFile)
{
	int iRet = -1;
	for(int iPage = 0; iPage < ui->tabWidget->count(); iPage++)
	{
		ChildWindow* pChild = (ChildWindow*) ui->tabWidget->widget(iPage);
		if(pChild->windowFilePath() == sFile)
		{
			iRet = iPage;
			break;
		}
	}
	return iRet;
}

// +-----------------------------------------------------------
void f3::MainWindow::setImageListView(QString sType)
{
	if(sType == "details")
	{
		m_pViewButton->setIcon(QIcon(":/icons/viewdetails"));
		ui->listImages->setVisible(false);
		ui->treeImages->setVisible(true);
		disconnect(ui->listImages->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)), this, SLOT(on_thumbnailSelected(const QModelIndex &, const QModelIndex &)));
		connect(ui->treeImages->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)), this, SLOT(on_thumbnailSelected(const QModelIndex &, const QModelIndex &)));
	}
	else if(sType == "icons")
	{
		m_pViewButton->setIcon(QIcon(":/icons/viewicons"));
		ui->treeImages->setVisible(false);
		ui->listImages->setVisible(true);
		disconnect(ui->treeImages->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)), this, SLOT(on_thumbnailSelected(const QModelIndex &, const QModelIndex &)));
		connect(ui->listImages->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)), this, SLOT(on_thumbnailSelected(const QModelIndex &, const QModelIndex &)));
	}
}

// +-----------------------------------------------------------
void f3::MainWindow::toggleImageListView()
{
	if(ui->treeImages->isVisible())
		setImageListView("icons");
	else
		setImageListView("details");
}

// +-----------------------------------------------------------
void f3::MainWindow::on_thumbnailSelected(const QModelIndex &oIndex, const QModelIndex &oPrevIndex)
{
	ChildWindow *pChild = (ChildWindow*) ui->tabWidget->currentWidget();
	if(!pChild)
		return;
	
	if(oIndex.isValid())
		pChild->showImage(oIndex.row());
	else
		pChild->showImage(-1);

	updateUI();
}

// +-----------------------------------------------------------
void f3::MainWindow::updateUI(const bool bUpdateModel)
{
	ChildWindow *pChild = (ChildWindow*) ui->tabWidget->currentWidget();
	bool bFileOpened = pChild != NULL;

	// Update display of thumbnails and selection events if requested
	if(bUpdateModel)
	{
		if(ui->listImages->selectionModel() || ui->treeImages->selectionModel())
		{
			disconnect(ui->listImages->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)), this, SLOT(on_thumbnailSelected(const QModelIndex &, const QModelIndex &)));
			disconnect(ui->treeImages->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)), this, SLOT(on_thumbnailSelected(const QModelIndex &, const QModelIndex &)));
		}
		ui->listImages->setModel(NULL);
		ui->treeImages->setModel(NULL);

		if(bFileOpened)
		{
			ui->listImages->setModel(pChild->getModel());
			ui->listImages->setSelectionModel(pChild->getSelectionModel());
			ui->treeImages->setModel(pChild->getModel());
			ui->treeImages->setSelectionModel(pChild->getSelectionModel());

			if(ui->listImages->isVisible())
				connect(ui->listImages->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)), this, SLOT(on_thumbnailSelected(const QModelIndex &, const QModelIndex &)));
			else
				connect(ui->treeImages->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)), this, SLOT(on_thumbnailSelected(const QModelIndex &, const QModelIndex &)));
		}
	}

	// Update actions and buttons statuses
	bool bFileChanged = bFileOpened ? pChild->isWindowModified() : false;
	bool bItemsSelected = bFileOpened && (pChild->getSelectionModel()->currentIndex().isValid() || pChild->getSelectionModel()->selectedIndexes().size() > 0);
	ui->actionSave->setEnabled(bFileChanged);
	ui->actionAddImage->setEnabled(bFileOpened);
	ui->actionRemoveImage->setEnabled(bItemsSelected);
	m_pViewButton->setEnabled(bFileOpened);

	// Update the tab text and tooltip and the image in display (if needed)
	if(bFileOpened)
	{
		QString sTitle = QFileInfo(pChild->windowFilePath()).baseName() + (pChild->isWindowModified() ? "*" : "");
		ui->tabWidget->setTabText(ui->tabWidget->currentIndex(), sTitle);
		if(!pChild->property("new").toBool()) // Complete file path only if the file has been saved before
			ui->tabWidget->setTabToolTip(ui->tabWidget->currentIndex(), pChild->windowFilePath());

		QModelIndex oCurrent = pChild->getSelectionModel()->currentIndex();
		if(!oCurrent.isValid())
			pChild->showImage(-1);
	}
}