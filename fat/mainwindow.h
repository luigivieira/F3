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

#include <QMainWindow>
#include "aboutwindow.h"

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

		/**
		 * Displays a text message in the main window status bar.
		 * @param sMsg QString with the message to be displayed.
		 * @param iTimeout Integer with the number of miliseconds
		 * by which the message will be displayed. The default is 5000
		 * (i.e. 5 seconds).
		 */
		void showStatusMessage(const QString &sMsg, int iTimeout = 5000);

	protected:

		/**
		 * Gets the page index of the file if already opened in the editor.
		 * @param sFile QString with the complete file name (path + name) of the file to be searched.
		 * @return Integer with the index (in range [0,...]) of the page where the file is opened
		 * or -1 if the file is not already opened in the editor.
		 */
		int getFilePageIndex(const QString &sFile);

    private slots:
		/**
		 * Slot for the menu New trigger event.
		 */ 
		void on_actionNew_triggered();

		/**
		 * Slot for the menu Open trigger event.
		 */ 
		void on_actionOpen_triggered();

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

    private:
        /** Instance of the ui for GUI element access. */
        Ui::MainWindow *ui;

        /** Instance of the about dialog box. */
        AboutWindow* m_pAbout;
    };

}

#endif // MAINWINDOW_H
