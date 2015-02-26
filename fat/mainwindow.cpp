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
#include <QListView>

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

	// Setup the list/tree item selection events
	connect(ui->listImages, SIGNAL(clicked(const QModelIndex &)), this, SLOT(on_listImagesClicked(const QModelIndex &)));
	connect(ui->treeImages, SIGNAL(clicked(const QModelIndex &)), this, SLOT(on_treeImagesClicked(const QModelIndex &)));

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

	pChild->setWindowTitle("");
	pChild->setWindowFilePath(QString(tr("%1novo banco de faces anotadas.afd")).arg(m_sDocumentsPath));
	pChild->setWindowModified(true);
	pChild->setWindowIcon(QIcon(":/icons/face-dataset"));

	ui->tabWidget->addTab(pChild, pChild->windowIcon(), pChild->windowTitle());
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
			pChild->setWindowTitle("");
			pChild->setWindowFilePath(sFile);
			pChild->setWindowModified(false);
			pChild->setWindowIcon(QIcon(":/icons/face-dataset"));

			int iPage = ui->tabWidget->addTab(pChild, pChild->windowIcon(), pChild->windowTitle());
			ui->tabWidget->setTabToolTip(iPage, sFile);
		}
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
	updateUI();
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
		updateUI();
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
	}
	else if(sType == "icons")
	{
		m_pViewButton->setIcon(QIcon(":/icons/viewicons"));
		ui->treeImages->setVisible(false);
		ui->listImages->setVisible(true);
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
void f3::MainWindow::on_listImagesClicked(const QModelIndex &oIndex)
{
	ChildWindow *pChild = (ChildWindow*) ui->tabWidget->currentWidget();
	if(!pChild)
		return;
	
	pChild->showImage(oIndex.row());
}

// +-----------------------------------------------------------
void f3::MainWindow::on_treeImagesClicked(const QModelIndex &oIndex)
{
	ChildWindow *pChild = (ChildWindow*) ui->tabWidget->currentWidget();
	if(!pChild)
		return;
	
	pChild->showImage(oIndex.row());
}

// +-----------------------------------------------------------
void f3::MainWindow::updateUI()
{
	ChildWindow *pChild = (ChildWindow*) ui->tabWidget->currentWidget();
	bool bFileOpened = pChild != NULL;
	bool bFileChanged = bFileOpened ? pChild->isWindowModified() : false;

	// Update action statuses
	ui->actionSave->setEnabled(bFileChanged);
	ui->actionAddImage->setEnabled(bFileOpened);
	ui->actionRemoveImage->setEnabled(bFileOpened);

	// Update button statuses
	m_pViewButton->setEnabled(bFileOpened);

	// Update display of thumbnails
	if(pChild)
	{
		ui->listImages->setModel(NULL);
		ui->listImages->setModel(pChild->getModel());
		ui->treeImages->setModel(NULL);
		ui->treeImages->setModel(pChild->getModel());
	}
}