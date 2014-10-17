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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenu>
#include "ui_mainwindow.h"

namespace f3
{
    /**
     * Main application window.
     * @author Luiz C. Vieira
     * @version 1.0
     */
	class MainWindow : public QMainWindow
	{
		Q_OBJECT

	public:
		/**
 		 * Class constructor.
		 * @param pParent QWidget with the window parent.
		 */
		explicit MainWindow(QWidget* pParent = 0);

		/**
		 * Class destructor.
		 */
		virtual ~MainWindow();

    private slots:
        void onExitActionTriggered();

        void on_m_pExitAction_triggered();

    private:

	private:
		/** Instance of the ui for GUI element access. */
		Ui::MainWindow ui;
	};
}

#endif // MAINWINDOW_H
