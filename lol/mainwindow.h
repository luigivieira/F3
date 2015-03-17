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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "aboutwindow.h"
#include "logcontrol.h"

#include <QMainWindow>
#include <QTreeWidget>

namespace Ui {
    class MainWindow;
}

namespace f3
{
    /**
     * Main GUI window class.
     */
    class MainWindow : public QMainWindow
    {
        Q_OBJECT

    public:
        /**
         * Class constructor.
         * @param pParent QWidget with the window parent.
         */
        explicit MainWindow(QWidget *pParent = 0);

        /**
         * Class destructor.
         */
        virtual ~MainWindow();

	protected:

		/**
		 * Updates all the user interface components according to the current state of the application
		 */
		void updateUI();

		/**
		 * Captures the event of the window being showed.
		 */
		void showEvent(QShowEvent *oEvent);

	public slots:

		/**
		 * Displays a text message in the main window status bar.
		 * @param sMsg QString with the message to be displayed.
		 * @param iTimeout Integer with the number of miliseconds
		 * by which the message will be displayed. The default is 5000
		 * (i.e. 5 seconds).
		 */
		void showStatusMessage(const QString &sMsg, const int iTimeout = 5000);

    private slots:
		/**
		 * Slot for the menu Refresh trigger event.
		 */ 
		void on_actionRefresh_triggered();

        /**
         * Slot for the menu Exit trigger event.
         */
        void on_actionExit_triggered();

        /**
         * Slot for the menu Project trigger event.
         */
        void on_actionProject_triggered();

        /**
         * Slot for the menu About trigger event.
         */
        void on_actionAbout_triggered();

		/**
		 * Slot for the 
		 */

		void onEditLevel(QTreeWidgetItem *pItem, int iColumn);

    private:
        /** Instance of the ui for GUI element access. */
        Ui::MainWindow *ui;

        /** Instance of the about dialog box. */
        AboutWindow* m_pAbout;

		/** Object used to access the log data via inter-process communication. */
		LogControl m_oLogControl;
    };

}

#endif // MAINWINDOW_H
