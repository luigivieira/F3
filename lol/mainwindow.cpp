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
#include "application.h"
#include "leveldelegate.h"

#include <QApplication>
#include <QAction>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include <QComboBox>
#include <QDateTime>

using namespace std;

// +-----------------------------------------------------------
f3::MainWindow::MainWindow(QWidget *pParent) :
    QMainWindow(pParent),
    ui(new Ui::MainWindow)
{
	// Setup the UI
    ui->setupUi(this);
    m_pAbout = NULL;

    setWindowIcon(QIcon(":/icons/lol"));

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

	ui->treeData->setModel(&m_oDataModel);
	ui->treeData->setItemDelegate(new LevelDelegate());
}

// +-----------------------------------------------------------
f3::MainWindow::~MainWindow()
{
    if(m_pAbout)
        delete m_pAbout;
    delete ui;
}

// +-----------------------------------------------------------
void f3::MainWindow::on_actionRefresh_triggered()
{
    updateUI();
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
void f3::MainWindow::showStatusMessage(const QString &sMsg, const int iTimeout)
{
	ui->statusBar->showMessage(sMsg, iTimeout);
}

// +-----------------------------------------------------------
void f3::MainWindow::showEvent(QShowEvent *)
{
	updateUI();
}

// +-----------------------------------------------------------
void f3::MainWindow::updateUI()
{
	if(!m_oDataModel.updateData())
	{
		QMessageBox::warning(this, tr("Erro de atualização"), tr("Não foi possível atualizar a lista de aplicações em execução."), QMessageBox::Ok);
		return;
	}

	QString sNow = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
	QString sUpdate = tr("Última atualização: %1").arg(sNow);
	if(m_oDataModel.rowCount() == 0)
		F3Application::showStatusMessage(tr("Não há nenhuma aplicação do projeto F3 em execução. %1").arg(sUpdate), 0);
	else if(m_oDataModel.rowCount() == 1)
		F3Application::showStatusMessage(tr("Há apenas uma aplicação do projeto F3 em execução. %1").arg(sUpdate), 0);
	else
		F3Application::showStatusMessage(tr("Há %1 aplicações do projeto F3 em execução. %1").arg(m_oDataModel.rowCount()).arg(sUpdate), 0);
}