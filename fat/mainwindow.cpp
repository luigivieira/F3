/*
 * Copyright (C) 2014 Luiz Carlos Vieira (http://www.luiz.vieira.nom.br)
 *
 * This file is part of Fun-From-Face (f3).
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

// +-----------------------------------------------------------
f3::MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
	ui.setupUi(this);
	setWindowIcon(QIcon(":/resources/fat.png"));
}

// +-----------------------------------------------------------
f3::MainWindow::~MainWindow()
{

}
void f3::MainWindow::onExitActionTriggered()
{
	close();
}

void f3::MainWindow::on_m_pExitAction_triggered()
{
    close();
}
