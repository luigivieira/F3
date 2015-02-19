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

using namespace cv;
using namespace std;

// +-----------------------------------------------------------
f3::MainWindow::MainWindow(QWidget *pParent) :
    QMainWindow(pParent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowState(Qt::WindowMaximized);
    m_pAbout = NULL;

    setWindowIcon(QIcon(":/icons/fat"));
}

// +-----------------------------------------------------------
f3::MainWindow::~MainWindow()
{
    if(!m_pAbout)
        delete m_pAbout;
    delete ui;
}

// +-----------------------------------------------------------
void f3::MainWindow::on_actionNew_triggered()
{
    ChildWindow *pChild = new ChildWindow(this);

	QString sDocPath = QDir::toNativeSeparators(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)) + QDir::separator();
	pChild->setWindowTitle("");
	pChild->setWindowFilePath(QString(tr("%1novo banco de faces anotadas.afd")).arg(sDocPath));
	pChild->setWindowModified(true);
	pChild->setWindowIcon(QIcon(":/icons/face-dataset"));

	ui->tabWidget->addTab(pChild, pChild->windowIcon(), pChild->windowTitle());
}

// +-----------------------------------------------------------
void f3::MainWindow::on_actionOpen_triggered()
{
	QString sDocPath = QDir::toNativeSeparators(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)) + QDir::separator();

    QString sFile = QFileDialog::getOpenFileName(this, tr("Abrir banco de faces anotadas..."), sDocPath, tr("Arquivos de Banco de Faces Anotadas (*.afd);; Todos os arquivos (*.*)"));
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