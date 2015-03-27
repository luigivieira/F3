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

#include "facedatasetmodel.h"
#include "facewidget.h"

#include <QtGui>
#include <QWidget>

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
		 * Adds the given images to the face annotation dataset.
		 * @param lsFiles QStringList with the list of files (including paths) to be added.
		 */
		void addImages(const QStringList &lsFiles);

		/**
		 * Removes the given images from the face annotation dataset.
		 * @param lIndexes QList with the indexes of the files to be removed.
		 */
		void removeImages(const QList<int> &lIndexes);

		/**
		 * Gets the list model used to display information about the contents of the
		 * face annotation dataset handled by this window.
		 * @return An instance of a QAbstractListModel that can be used with any subclass of QAbstractItemView
		 * to display lists, icons or trees with the face annotation dataset contents.
		 */
		QAbstractListModel* getModel() const;

		/**
		 * Gets the list selection model used to display selection information about the contents of the
		 * face annotation dataset handled by this window.
		 * @return An instance of a QItemSelectionModel that can be used with any subclass of QAbstractItemView
		 * to display lists, icons or tress with the face annotation dataset contents.
		 */
		QItemSelectionModel* getSelectionModel() const;

		/**
		 * Requests the window to display the face image of the given index (in range [-1, count-1], where
		 * count is the number of images in the face annotation dataset). If the valur of iIndex is -1 (a 
		 * especial value), the empty watermark image is displayed instead.
		 * @param iIndex Integer with the index of the image to be displayed.
		 */
		void showImage(const int iIndex);

		/**
		 * Saves the contents of the face annotation dataset in this window to the current file
		 * (stored in the windowFilePath property). The file is saved in the YAML format, as defined
		 * in the FaceDataset class.
		 */
		bool save(QString &sMsgError);

		/**
		 * Saves the contents of the face annotation dataset in this window to the given file.
		 * The file is saved in the YAML format, as defined in the FaceDataset class.
		 * @param sFileName QString with the path and name of the file to save the dataset to.
		 */
		bool saveToFile(const QString &sFileName, QString &sMsgError);

		/**
		 * Loads the contents of the face annotation dataset from the given file into this window.
		 * The file must be in the YAML format, as defined in the FaceDataset class.
		 * @param sFileName QString with the path and name of the file to load the dataset from.
		 */
		bool loadFromFile(const QString &sFileName, QString &sMsgError);

		/**
		 * Sets the zoom level of the image in display in terms of the steps defined in the zoom
		 * slider from 1 to 21, with 11 (the middle value) as "no zoom" (i.e. 100% view).
		 * @param iLevel Integer with the zoom level to set.
		 */
		void setZoomLevel(const int iLevel);

		/**
		 * Gets the current zoom level of the image in display in terms of the steps defined in the zoom
		 * slider from 1 to 21, with 11 (the middle value) as "no zoom" (i.e. 100% view).
		 * @return Integer with the current zoom level.
		 */
		int getZoomLevel() const;

	protected slots:

		/**
		 * Captures the indication of changes in the image scale factor (zoom).
		 * @param dScaleFactor Double with the new scale factor for the image.
		 */
		void onScaleFactorChanged(const double dScaleFactor);

	signals:

		/**
		 * Signal to indicate that the zoom level has changed.
		 * @param iLevel Integer with the new level of zoom.
		 */
		void onZoomLevelChanged(const int iLevel);

	private:

		/** Widget used to display face images and edit facial features. */
		FaceWidget *m_pFaceWidget;

		/** Instance of the model used to encapsulate the access of the face dataset to Qt view components such as QListView. */
		FaceDatasetModel *m_pFaceDatasetModel;

		/** Selection model used to represent the selection of items in Qt view components such as QListView. */
		QItemSelectionModel *m_pFaceSelectionModel;
	};
}

#endif // CHILDWINDOW_H
