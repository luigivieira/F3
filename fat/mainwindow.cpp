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
#include "utils.h"
#include "application.h"
#include "emotiondelegate.h"

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
	QPalette oROPalette = ui->textFileName->palette();
	oROPalette.setColor(QPalette::Base, oROPalette.midlight().color());
	ui->textFileName->setPalette(oROPalette);

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

	// Connect the emotion radio selectors
	connect(ui->radioUndefined, SIGNAL(toggled(bool)), this, SLOT(onEmotionToggled(bool)));
	connect(ui->radioAnger, SIGNAL(toggled(bool)), this, SLOT(onEmotionToggled(bool)));
	connect(ui->radioContempt, SIGNAL(toggled(bool)), this, SLOT(onEmotionToggled(bool)));
	connect(ui->radioDisgust, SIGNAL(toggled(bool)), this, SLOT(onEmotionToggled(bool)));
	connect(ui->radioFear, SIGNAL(toggled(bool)), this, SLOT(onEmotionToggled(bool)));
	connect(ui->radioHappiness, SIGNAL(toggled(bool)), this, SLOT(onEmotionToggled(bool)));
	connect(ui->radioSadness, SIGNAL(toggled(bool)), this, SLOT(onEmotionToggled(bool)));
	connect(ui->radioSurprise, SIGNAL(toggled(bool)), this, SLOT(onEmotionToggled(bool)));

	// Item delegate to allow editing the emotion label directly through the TreeView
	ui->treeImages->setItemDelegate(new EmotionDelegate());
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
void f3::MainWindow::closeEvent(QCloseEvent *pEvent)
{
	QList<ChildWindow*> lModified;
	ChildWindow *pChild;

	for(int i = 0; i < ui->tabWidget->count(); i++)
	{
		pChild = (ChildWindow*) ui->tabWidget->widget(i);
		if(pChild->isWindowModified())
			lModified.append(pChild);
	}

	if(lModified.size() > 0)
	{
		QString sMsg;
		if(lModified.size() == 1)
		{
			pChild = lModified[0];
			sMsg = tr("Existem alterações pendentes no banco de faces anotadas de nome [%1]. Deseja gravar antes de fechar?").arg(QFileInfo(pChild->windowFilePath()).baseName());
		}
		else
			sMsg = tr("Existem alterações pendentes em %1 bancos de faces anotadas. Deseja gravar antes de fechar?").arg(lModified.size());

		QMessageBox::StandardButton oResp = QMessageBox::question(this, tr("Alterações pendentes"), sMsg, QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);

		// Do not close the tab if the user has chosen "cancel" or if she has chosen "yes" but then
		// cancelled the file save dialog in any of the modified tab
		if(oResp == QMessageBox::Cancel)
		{
			pEvent->ignore();
			return;
		}
		else if(oResp == QMessageBox::Yes)
		{
			for(int i = 0; i < lModified.size(); i++)
			{
				pChild = lModified[i];
				ui->tabWidget->setCurrentWidget(pChild);
				if(!saveCurrentFile())
				{
					pEvent->ignore();
					return;
				}
			}
		}
	}

	pEvent->accept();
}

// +-----------------------------------------------------------
void f3::MainWindow::showEvent(QShowEvent *pEvent)
{
	Q_UNUSED(pEvent);
	// Update the statuses of UI elements
	updateUI();

	ui->actionShowImagesList->setChecked(ui->dockImages->isVisible());
	ui->actionShowImageProperties->setChecked(ui->dockProperties->isVisible());
}

// +-----------------------------------------------------------
void f3::MainWindow::on_actionNew_triggered()
{
    createChildWindow();
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
			ChildWindow *pChild = createChildWindow(sFile, false);

			QString sMsg;
			if(!pChild->loadFromFile(sFile, sMsg))
			{
				destroyChildWindow(pChild);
				QMessageBox::warning(this, tr("Erro carregando banco de faces anotadas"), tr("Não foi possível abrir o banco de faces anotadas:\n%1").arg(sMsg), QMessageBox::Ok);
				return;
			}

			if(pChild->dataModel()->rowCount() > 0)
				pChild->selectionModel()->setCurrentIndex(pChild->dataModel()->index(0, 0), QItemSelectionModel::Select);
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

	destroyChildWindow(pChild);

	if(ui->tabWidget->count() == 0) // no more tabs
	{
		ui->textFileName->setText("");
		ui->radioUndefined->blockSignals(true);
		ui->radioUndefined->setChecked(true);
		ui->radioUndefined->blockSignals(false);
		ui->zoomSlider->blockSignals(true);
		ui->zoomSlider->setValue((ui->zoomSlider->maximum() - ui->zoomSlider->minimum()) / 2 + 1);
		ui->zoomSlider->blockSignals(false);
	}
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
		pChild->dataModel()->addImages(lsFiles);
		if(!pChild->selectionModel()->currentIndex().isValid())
			pChild->selectionModel()->setCurrentIndex(pChild->dataModel()->index(0, 0), QItemSelectionModel::Select);
	}
}

// +-----------------------------------------------------------
void f3::MainWindow::on_actionRemoveImage_triggered()
{
	ChildWindow *pChild = (ChildWindow*) ui->tabWidget->currentWidget();
	if(!pChild)
		return;

	QModelIndexList lsSelected = pChild->selectionModel()->selectedRows();
	if(lsSelected.size() > 0)
	{
		QString sMsg;
		if(lsSelected.size() == 1)
			sMsg = tr("Você confirma a remoção da imagem selecionada?");
		else
			sMsg = tr("Você confirma a remoção das %1 imagens selecionadas?").arg(lsSelected.size());
		if(QMessageBox::question(this, tr("Confirmação da remoção"), sMsg, QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes)
		{
			QList<int> lIndexes;
			for(int i = 0; i < lsSelected.size(); i++)
				lIndexes.append(lsSelected[i].row());
			pChild->dataModel()->removeImages(lIndexes);
		}
	}
}

// +-----------------------------------------------------------
void f3::MainWindow::on_actionShowImagesList_triggered(bool bChecked)
{
	ui->dockImages->setVisible(bChecked);
}

// +-----------------------------------------------------------
void f3::MainWindow::on_actionShowImageProperties_triggered(bool bChecked)
{
	ui->dockProperties->setVisible(bChecked);
}

// +-----------------------------------------------------------
void f3::MainWindow::on_actionShowFeatures_triggered(bool bChecked)
{
	if(!bChecked)
	{
		ui->actionShowConnections->setChecked(false);
		ui->actionShowFeatureIDs->setChecked(false);
	}
	updateUI();
}

// +-----------------------------------------------------------
void f3::MainWindow::on_actionShowConnections_triggered(bool bChecked)
{
	if(bChecked && !ui->actionShowFeatures->isChecked())
		ui->actionShowFeatures->setChecked(true);
	updateUI();
}

// +-----------------------------------------------------------
void f3::MainWindow::on_actionShowFeatureIDs_triggered(bool bChecked)
{
	if(bChecked && !ui->actionShowFeatures->isChecked())
		ui->actionShowFeatures->setChecked(true);
	updateUI();
}

// +-----------------------------------------------------------
void f3::MainWindow::on_actionAddFeature_triggered()
{
	ChildWindow *pChild = (ChildWindow*) ui->tabWidget->currentWidget();
	if(!pChild)
		return;

	// The position of the mouse  is stored in this action data
	// if the action is called from a context menu in the face features
	// editor (see method FaceWidget::contextMenuEvent)
	QVariant vPos = ui->actionAddFeature->data();
	QPoint oPos;
	if(vPos.isValid())
		oPos = vPos.value<QPoint>();
	else
		oPos = QCursor::pos();

	pChild->addFeature(oPos);
}

// +-----------------------------------------------------------
void f3::MainWindow::on_actionRemoveFeature_triggered()
{
	ChildWindow *pChild = (ChildWindow*) ui->tabWidget->currentWidget();
	if(!pChild)
		return;

	QString sMsg = tr("Tem certeza de que deseja remover as marcas faciais selecionadas? Essa ação não pode ser desfeita.");
	QMessageBox::StandardButton oResp = QMessageBox::question(this, tr("Confirmação de Remoção"), sMsg, QMessageBox::Yes|QMessageBox::No);

	if(oResp == QMessageBox::Yes)
		pChild->removeSelectedFeatures();
}

// +-----------------------------------------------------------
void f3::MainWindow::on_actionConnectFeatures_triggered()
{
	ChildWindow *pChild = (ChildWindow*) ui->tabWidget->currentWidget();
	if(!pChild)
		return;

	pChild->connectFeatures();
}

// +-----------------------------------------------------------
void f3::MainWindow::on_actionDisconnectFeatures_triggered()
{
	ChildWindow *pChild = (ChildWindow*) ui->tabWidget->currentWidget();
	if(!pChild)
		return;

	pChild->disconnectFeatures();
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
		//disconnect(ui->listImages->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)), this, SLOT(onthumbnailSelected(const QModelIndex &, const QModelIndex &)));
		//connect(ui->treeImages->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)), this, SLOT(onthumbnailSelected(const QModelIndex &, const QModelIndex &)));
	}
	else if(sType == "icons")
	{
		m_pViewButton->setIcon(QIcon(":/icons/viewicons"));
		ui->treeImages->setVisible(false);
		ui->listImages->setVisible(true);
		//disconnect(ui->treeImages->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)), this, SLOT(onthumbnailSelected(const QModelIndex &, const QModelIndex &)));
		//connect(ui->listImages->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)), this, SLOT(onthumbnailSelected(const QModelIndex &, const QModelIndex &)));
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
void f3::MainWindow::onChildUIUpdated(const QString sImageName, const EmotionLabel eEmotion, const int iZoomLevel)
{
	// Image file name
	ui->textFileName->setText(sImageName);
	ui->textFileName->moveCursor(QTextCursor::End);
	ui->textFileName->ensureCursorVisible();
			
	// Emotion label
	QObjectList lsObjects = ui->groupEmotions->children();
	QRadioButton *pRadio = qobject_cast<QRadioButton*>(lsObjects.at(eEmotion.getValue() + 1)); // The +1 is to ignore the layout widget
	pRadio->blockSignals(true);
	pRadio->setChecked(true);
	pRadio->blockSignals(false);

	// Zoom level
	ui->zoomSlider->blockSignals(true);
	ui->zoomSlider->setValue(iZoomLevel);
	ui->zoomSlider->blockSignals(false);

	updateUI();
}

// +-----------------------------------------------------------
void f3::MainWindow::updateUI(const bool bCompleteUpdate)
{
	// Setup the control variables
	ChildWindow *pChild = (ChildWindow*) ui->tabWidget->currentWidget();

	bool bFileOpened = pChild != NULL;
	bool bFileChanged = bFileOpened ? pChild->isWindowModified() : false;
	bool bItemsSelected = bFileOpened && (pChild->selectionModel()->currentIndex().isValid() || pChild->selectionModel()->selectedIndexes().size() > 0);
	bool bFileNotNew = bFileOpened && !pChild->property("new").toBool();

	QList<FaceFeatureNode*> lFeats;
	QList<FaceFeatureEdge*> lConns;
	if(bFileOpened)
	{
		lFeats = pChild->getSelectedFeatures();
		lConns = pChild->getSelectedConnections();
	}

	bool bFeaturesSelected = lFeats.size() > 0;
	bool bConnectionsSelected = lConns.size() > 0;
	bool bFeaturesConnectable = lFeats.size() == 2 && lConns.size() == 0;

	// Update the data and selection models
	if(bFileOpened)
	{
		ui->listImages->setModel(pChild->dataModel());
		ui->listImages->setSelectionModel(pChild->selectionModel());
		ui->treeImages->setModel(pChild->dataModel());
		ui->treeImages->setSelectionModel(pChild->selectionModel());
	}
	else
	{
		ui->listImages->setModel(NULL);
		ui->treeImages->setModel(NULL);
	}

	// Update the UI availability
	ui->actionSave->setEnabled(bFileChanged);
	ui->actionSaveAs->setEnabled(bFileNotNew);
	ui->actionAddImage->setEnabled(bFileOpened);
	ui->actionRemoveImage->setEnabled(bItemsSelected);
	ui->actionAddFeature->setEnabled(bFileOpened);
	ui->actionRemoveFeature->setEnabled(bFeaturesSelected);
	ui->actionConnectFeatures->setEnabled(bFeaturesConnectable);
	ui->actionDisconnectFeatures->setEnabled(bConnectionsSelected);
	m_pViewButton->setEnabled(bFileOpened);
	ui->groupEmotions->setEnabled(bItemsSelected);
	ui->zoomSlider->setEnabled(bFileOpened);

	// Update the tab title and tooltip
	if(bFileOpened)
	{
		QString sTitle = QFileInfo(pChild->windowFilePath()).baseName() + (pChild->isWindowModified() ? "*" : "");
		ui->tabWidget->setTabText(ui->tabWidget->currentIndex(), sTitle);
		if(bFileNotNew) // Complete file path only if the file has been saved before
			ui->tabWidget->setTabToolTip(ui->tabWidget->currentIndex(), pChild->windowFilePath());
	}

	// NLA 
	//ui->textFileName->setText("");
	//ui->radioUndefined->blockSignals(true);
	//ui->radioUndefined->setChecked(true);
	//ui->radioUndefined->blockSignals(false);

	// Display of face features
	if(bFileOpened)
	{
		pChild->setDisplayFaceFeatures(ui->actionShowFeatures->isChecked());
		pChild->setDisplayConnections(ui->actionShowFeatures->isChecked() && ui->actionShowConnections->isChecked());
		pChild->setDisplayFeatureIDs(ui->actionShowFeatures->isChecked() && ui->actionShowFeatureIDs->isChecked());
	}

}

// +-----------------------------------------------------------
void f3::MainWindow::onSliderValueChanged(int iValue)
{
	ChildWindow *pChild = (ChildWindow*) ui->tabWidget->currentWidget();
	if(pChild)
		pChild->setZoomLevel(iValue);
}

// +-----------------------------------------------------------
void f3::MainWindow::onZoomLevelChanged(int iValue)
{
	ui->zoomSlider->blockSignals(true);
	ui->zoomSlider->setValue(iValue);
	ui->zoomSlider->blockSignals(false);
}

// +-----------------------------------------------------------
void f3::MainWindow::onUpdateUI()
{
	updateUI();
}

// +-----------------------------------------------------------
void f3::MainWindow::keyPressEvent(QKeyEvent *pEvent)
{
	ChildWindow *pChild = (ChildWindow*) ui->tabWidget->currentWidget();
	if(!pChild)
	{
		QMainWindow::keyPressEvent(pEvent);
		return;
	}

    switch(pEvent->key())
	{
		case Qt::Key_Plus:
			pChild->zoomIn();
			pEvent->accept();
			break;

		case Qt::Key_Minus:
			pChild->zoomOut();
			pEvent->accept();
			break;

		default:
			QMainWindow::keyPressEvent(pEvent);
    }
}

// +-----------------------------------------------------------
void f3::MainWindow::onEmotionToggled(bool bValue)
{
	ChildWindow *pChild = (ChildWindow*) ui->tabWidget->currentWidget();
	if(!bValue || !pChild)
		return;

	EmotionLabel eLabel = EmotionLabel::UNDEFINED;
	QObjectList lsObjects = ui->groupEmotions->children();
	QRadioButton *pRadio;
	for(int i = lsObjects.size() - 1; i >= 0; i--)
	{
		pRadio = qobject_cast<QRadioButton*>(lsObjects.at(i));
		if(pRadio && pRadio == sender())
		{
			eLabel = EmotionLabel::fromValue(i-1);
			break;
		}
	}

	pChild->updateEmotionLabel(eLabel);
}

// +-----------------------------------------------------------
f3::ChildWindow* f3::MainWindow::createChildWindow(QString sFileName, bool bModified)
{
	ChildWindow *pChild = new ChildWindow(this);

	// Define the window attributes
	if(!sFileName.length())
	{
		sFileName = QString(tr("%1novo banco de faces anotadas.afd")).arg(m_sDocumentsPath);
		bModified = true;
	}

	pChild->setWindowIcon(QIcon(":/icons/face-dataset"));
	pChild->setWindowFilePath(sFileName);
	pChild->setWindowModified(bModified);

	// Connect to its signals
	connect(pChild, SIGNAL(onUIUpdated(const QString, const EmotionLabel, const int)), this, SLOT(onChildUIUpdated(const QString, const EmotionLabel, const int)));
	connect(pChild, SIGNAL(onDataModified()), this, SLOT(onUpdateUI()));
	connect(pChild, SIGNAL(onFeaturesSelectionChanged()), this, SLOT(onUpdateUI()));

	// Create the context menu for the features editor, using the same actions from the main window
	QMenu *pContextMenu = new QMenu(pChild);
	pContextMenu->addAction(ui->actionAddFeature);
	pContextMenu->addAction(ui->actionRemoveFeature);
	pContextMenu->addAction(ui->actionConnectFeatures);
	pContextMenu->addAction(ui->actionDisconnectFeatures);
	pChild->setContextMenu(pContextMenu);

	// Add the window to the tab widget
	int iIndex = ui->tabWidget->addTab(pChild, pChild->windowIcon(), "");
	ui->tabWidget->setCurrentIndex(iIndex);

	return pChild;
}

// +-----------------------------------------------------------
void f3::MainWindow::destroyChildWindow(ChildWindow *pChild)
{
	int iTabIndex = ui->tabWidget->indexOf(pChild);
	ui->tabWidget->removeTab(iTabIndex);

	disconnect(pChild, SIGNAL(onUIUpdated(const QString, const EmotionLabel, const int)), this, SLOT(onChildUIUpdated(const QString, const EmotionLabel, const int)));
	disconnect(pChild, SIGNAL(onDataModified()), this, SLOT(onUpdateUI()));
	disconnect(pChild, SIGNAL(onFeaturesSelectionChanged()), this, SLOT(onUpdateUI()));

	delete pChild;
}