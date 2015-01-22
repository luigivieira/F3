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
#include "childwindow.h"

// +-----------------------------------------------------------
f3::MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
	ui.setupUi(this);
	setWindowIcon(QIcon(":/resources/fat.png"));
	menuBar()->setNativeMenuBar(false);

	m_pMdiArea = new QMdiArea(this);
	m_pMdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_pMdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	setCentralWidget(m_pMdiArea);
}

// +-----------------------------------------------------------
f3::MainWindow::~MainWindow()
{
}

// +-----------------------------------------------------------
void f3::MainWindow::on_action_Exit_triggered()
{
	close();
}

// +-----------------------------------------------------------
void f3::MainWindow::on_action_Open_triggered()
{

}

// +-----------------------------------------------------------
void f3::MainWindow::on_action_New_triggered()
{
	ChildWindow* pChild = new ChildWindow();
	m_pMdiArea->addSubWindow(pChild);
    pChild->show();
	pChild->adjustSize();
}

// +-----------------------------------------------------------
void f3::MainWindow::on_action_Save_triggered()
{

}

// +-----------------------------------------------------------
void f3::MainWindow::on_actionS_ave_As_triggered()
{

}

// +-----------------------------------------------------------
void f3::MainWindow::on_action_About_triggered()
{

}
