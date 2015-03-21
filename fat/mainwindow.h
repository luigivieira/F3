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

	protected:

		/**
		 * Helper method to request the saving of the current face annotation dataset opened
		 * to the user.
		 * @param bAskForFileName Boolean indicating if the UI should ask the user for a file
		 * name. If the value is true, the user is requested to choose a file name, otherwise 
		 * (the default) the current file name is used. If the file has not yet been saved,
		 * a file name is requested despite the value on this parameter.
		 * @return Boolean indicating if the save was performed or false if the user cancelled
		 * the operation.
		 */
		bool saveCurrentFile(bool bAskForFileName=false);

		/**
		 * Gets the page index of the file if already opened in the editor.
		 * @param sFile QString with the complete file name (path + name) of the file to be searched.
		 * @return Integer with the index (in range [0,...]) of the page where the file is opened
		 * or -1 if the file is not already opened in the editor.
		 */
		int getFilePageIndex(const QString &sFile);

		/**
		 * Updates all the user interface components according to the current state of the application
		 * (i.e. disables action buttons when there is no opened face annotation dataset, etc).
		 */
		void updateUI(const bool bUpdateModel = false);

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
		 * Slot for the menu New trigger event.
		 */ 
		void on_actionNew_triggered();

		/**
		 * Slot for the menu Open trigger event.
		 */ 
		void on_actionOpen_triggered();

		/**
		 * Slot for the menu Save trigger event.
		 */ 
		void on_actionSave_triggered();

		/**
		 * Slot for the menu Save As trigger event.
		 */ 
		void on_actionSaveAs_triggered();

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
		 * Slot for the tab (child window) closing request (by clicking in the close button).
		 * @param iTabIndex Integer with the index of the tab requested to close.
		 */
		void on_tabCloseRequested(int iTabIndex);

		/**
		 * Slot for the tab (child window) change event (when a new tab is selected).
		 * @param iTabIndex Integer with the index of the tab now selected.
		 */
		void on_tabChanged(int iTabIndex);

		/**
		 * Slot for the button Add Image trigger event.
		 */
		void on_actionAddImage_triggered();

		/**
		 * Slot for the button Add Image trigger event.
		 */
		void on_actionRemoveImage_triggered();

		/**
		 * Define the view to be used for the list of images (icons, a list view, or "details", a tree view).
		 * @param sType String with the type to be used (either "icons" or "details").
		 */
		void setImageListView(QString sType);

		/**
		 * Helper method to toggle the current view for the list of images (used in the shortcut button).
		 */
		void toggleImageListView();

		/**
		 * Slot for the selection of an image thumbnail in either the list or the tree view components.
		 * @param oIndex QModelIndex with the item selected.
		 * @param oPrevIndex QModelIndex with the item previously selected.
		 */
		void on_thumbnailSelected(const QModelIndex &oIndex, const QModelIndex &oPrevIndex);

		/**
		 * Captures the updates in the zoom slider by user interaction.
		 * @param iValue Integer with the new value of the slider (in the range [1, 21]).
		 */
		void onSliderValueChanged(int iValue);

		/**
		 * Captures the updates in the zoom by user interaction in other sources that not the slider.
		 * @param iValue Integer with the new value of the zoom (in the range [1, 21]).
		 */
		void onZoomLevelChanged(int iValue);

    private:
        /** Instance of the ui for GUI element access. */
        Ui::MainWindow *ui;

        /** Instance of the about dialog box. */
        AboutWindow* m_pAbout;

		/** Standard path to read from/write to documents in the native OS. */
		QString m_sDocumentsPath;

		/** Instance of a dropdown button for the view mode of the image list. */
		QMenu *m_pViewButton;
    };
};

#endif // MAINWINDOW_H
