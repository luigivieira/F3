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

	QAction *pViewDetails = new QAction(QIcon(":/icons/viewdetails"), tr("&Detalhes"), this);
	pViewDetails->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_D));
	m_pViewButton->addAction(pViewDetails);
	QAction *pViewIcons = new QAction(QIcon(":/icons/viewicons"), tr("&Ícones"), this);
	pViewIcons->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_I));
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

	// Add the action shortcuts to the tooltips (in order to make it easier for the user to know they exist)
	// P.S.: I wonder why doesn't Qt do that automatically... :)
	QObjectList lsObjects = children();
	QAction *pAction;
	for(int i = 0; i < lsObjects.size(); i++)
	{
		pAction = qobject_cast<QAction*>(lsObjects.at(i));
		if(pAction && !pAction->shortcut().isEmpty())
			pAction->setToolTip(QString("%1 (%2)").arg(pAction->toolTip()).arg(pAction->shortcut().toString()));
	}

	lsObjects = ui->groupEmotions->children();
	QRadioButton *pRadio;
	for(int i = 0; i < lsObjects.size(); i++)
	{
		pRadio = qobject_cast<QRadioButton*>(lsObjects.at(i));
		if(pRadio && !pRadio->shortcut().isEmpty())
			pRadio->setToolTip(QString("%1 (%2)").arg(pRadio->toolTip()).arg(pRadio->shortcut().toString()));
	}

	// Connect the zoom slider
	connect(ui->zoomSlider, SIGNAL(valueChanged(int)), this, SLOT(onSliderValueChanged(int)));
}

// +-----------------------------------------------------------
f3::MainWindow::~MainWindow()
{
    if(m_pAbout)
        delete m_pAbout;
	if(m_pViewButton)
		delete m_pViewButton;
    delete ui;
}

// +-----------------------------------------------------------
void f3::MainWindow::on_actionNew_triggered()
{
    ChildWindow *pChild = new ChildWindow(this);
	connect(pChild, SIGNAL(onZoomLevelChanged(int)), this, SLOT(onZoomLevelChanged(int)));

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
			showStatusMessage(QString(tr("O banco de faces anotadas [%1] já está aberto no editor")).arg(Utils::shortenPath(sFile)));
		}
		else
		{
			ChildWindow *pChild = new ChildWindow(this);

			pChild->setWindowIcon(QIcon(":/icons/face-dataset"));

			QString sMsg;
			if(!pChild->loadFromFile(sFile, sMsg))
			{
				delete pChild;
				QMessageBox::warning(this, tr("Erro carregando banco de faces anotadas"), tr("Não foi possível abrir o banco de faces anotadas:\n%1").arg(sMsg), QMessageBox::Ok);
				return;
			}

			connect(pChild, SIGNAL(onZoomLevelChanged(int)), this, SLOT(onZoomLevelChanged(int)));

			int iIndex = ui->tabWidget->addTab(pChild, pChild->windowIcon(), "");
			ui->tabWidget->setCurrentIndex(iIndex);

			if(pChild->getModel()->rowCount() > 0)
				pChild->getSelectionModel()->setCurrentIndex(pChild->getModel()->index(0, 0), QItemSelectionModel::Select);
			updateUI();
		}
	}
}

// +-----------------------------------------------------------
void f3::MainWindow::on_actionSave_triggered()
{
	saveCurrentFile();
}

// +-----------------------------------------------------------
void f3::MainWindow::on_actionSaveAs_triggered()
{
	saveCurrentFile(true);
}

// +-----------------------------------------------------------
bool f3::MainWindow::saveCurrentFile(bool bAskForFileName)
{
	ChildWindow *pChild = (ChildWindow*) ui->tabWidget->currentWidget();
	if(!pChild || !pChild->isWindowModified())
		return false;

	if(bAskForFileName)
	{
		QString sFileName = QFileDialog::getSaveFileName(this, tr("Salvar banco de faces anotadas..."), windowFilePath(), tr("Arquivos de banco de faces anotadas (*.afd);; Todos os arquivos (*.*)"));
		if(sFileName.length())
		{
			QString sMsg;
			if(!pChild->saveToFile(sFileName, sMsg))
			{
				QMessageBox::warning(this, tr("Erro gravando banco de faces anotadas"), tr("Não foi possível salvar o banco de faces anotadas:\n%1").arg(sMsg), QMessageBox::Ok);
				return false;
			}

			updateUI();
			return true;
		}
		else
			return false;
	}
	else
	{
		// Force the user to chose a file name if the dataset has not yet been saved
		if(pChild->property("new").toBool())
			return saveCurrentFile(true);
		else
		{
			QString sMsg;
			if(!pChild->save(sMsg))
			{
				QMessageBox::warning(this, tr("Erro gravando banco de faces anotadas"), tr("Não foi possível salvar o banco de faces anotadas:\n%1").arg(sMsg), QMessageBox::Ok);
				return false;
			}

			updateUI();
			return true;
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

	if(pChild->isWindowModified())
	{
		QString sMsg = tr("Existem alterações pendentes no banco de faces anotadas de nome [%1]. Deseja gravar antes de fechar?").arg(QFileInfo(pChild->windowFilePath()).baseName());
		QMessageBox::StandardButton oResp = QMessageBox::question(this, tr("Alterações pendentes"), sMsg, QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);

		// Do not close the tab if the user has chosen "cancel" or if she has chosen "yes" but then
		// cancelled the file save dialog
		ui->tabWidget->setCurrentIndex(iTabIndex);
		if(oResp == QMessageBox::Cancel || (oResp == QMessageBox::Yes && !saveCurrentFile()))
			return;
	}

	ui->tabWidget->removeTab(iTabIndex);
	disconnect(pChild, SIGNAL(onZoomLevelChanged(int)), this, SLOT(onZoomLevelChanged(int)));
	delete pChild;
}

// +-----------------------------------------------------------
void f3::MainWindow::on_tabChanged(int iTabIndex)
{
	Q_UNUSED(iTabIndex);
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
void f3::MainWindow::showStatusMessage(const QString &sMsg, const int iTimeout)
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
	Q_UNUSED(oPrevIndex);
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
		if(bFileOpened)
			ui->zoomSlider->setValue(pChild->getZoomLevel());

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
	bool bFileNotNew = bFileOpened && !pChild->property("new").toBool();

	ui->actionSave->setEnabled(bFileChanged);
	ui->actionSaveAs->setEnabled(bFileNotNew);
	ui->actionAddImage->setEnabled(bFileOpened);
	ui->actionRemoveImage->setEnabled(bItemsSelected);
	m_pViewButton->setEnabled(bFileOpened);
	ui->groupEmotions->setEnabled(bItemsSelected);
	ui->zoomSlider->setEnabled(bFileOpened);

	// Update the tab text and tooltip and the image in display (if needed)
	if(bFileOpened)
	{
		QString sTitle = QFileInfo(pChild->windowFilePath()).baseName() + (pChild->isWindowModified() ? "*" : "");
		ui->tabWidget->setTabText(ui->tabWidget->currentIndex(), sTitle);
		if(bFileNotNew) // Complete file path only if the file has been saved before
			ui->tabWidget->setTabToolTip(ui->tabWidget->currentIndex(), pChild->windowFilePath());

		QModelIndex oCurrent = pChild->getSelectionModel()->currentIndex();
		if(!oCurrent.isValid())
			pChild->showImage(-1);
	}
}

// +-----------------------------------------------------------
void f3::MainWindow::onSliderValueChanged(int iValue)
{
	ChildWindow *pChild = (ChildWindow*) ui->tabWidget->currentWidget();
	if(pChild)
		pChild->setZoomLevel(iValue, false); // the false means: do not emit the update signal in this case
}

// +-----------------------------------------------------------
void f3::MainWindow::onZoomLevelChanged(int iValue)
{
	ui->zoomSlider->setValue(iValue);
}