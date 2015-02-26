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

#ifndef CHILDWINDOW_H
#define CHILDWINDOW_H

#include "ui_childwindow.h"
#include "facedataset.h"
#include "facedatasetmodel.h"

#include <QtGui>
#include <QWidget>
#include <QScrollArea>
#include <QLabel>

namespace Ui
{
    class ChildWindow;
}

namespace f3
{
	/**
	 * MDI-child window class used to display and edit the face annotation datasets.
	 */
	class ChildWindow : public QWidget
	{
		Q_OBJECT

	public:
		/**
		 * Class constructor.
		 * @param pParent Instance of the widget that will be the parent of this window.
		 */
		explicit ChildWindow(QWidget* pParent = 0);

		/**
		 * Class destructor.
		 */
		virtual ~ChildWindow();

		/**
		 * Add the given images to the face annotation dataset.
		 * @param lsFiles QStringList with the list of files (including paths) to be added.
		 */
		void addImages(const QStringList &lsFiles);

		/**
		 * Gets the abstract list model used to display information about the contents of the
		 * face annotation dataset handled by this window.
		 * @return An instance of a QAbstractListModel that can be used with any subclass of QAbstractItemView
		 * to display lists, icons or trees with the face annotation dataset contents.
		 */
		QAbstractListModel* getModel() const;

		/**
		 * Requests the window to display the face image of the given index (in range [0, count-1], where
		 * count is the number of images in the face annotation dataset).
		 * @param iIndex Integer with the index of the image to be displayed.
		 */
		void showImage(const int iIndex);

	private:
		/** Instance of the ui for GUI element access. */
		Ui::ChildWindow *ui;

		/** Index of the image currently being displayed. */
		int m_iCurrentImage;

		/** Instance of the face dataset control class used to handle the annotation operations. */
		FaceDataset *m_pFaceDataset;

		/** Instance of the model used to encapsulate the access of Qt view components such as QListView. */
		FaceDatasetModel *m_pFaceDatasetModel;
	};
}

#endif // CHILDWINDOW_H
