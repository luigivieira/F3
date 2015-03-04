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

#ifndef FACEDATASETMODEL_H
#define FACEDATASETMODEL_H

#include "facedataset.h"

#include <QAbstractListModel>

namespace f3
{
	/**
	 * Model class that encapsulates the access to the FaceDataset in order to provide data for
	 * the view classes in Qt (such as QListView).
	 */
	class FaceDatasetModel : public QAbstractListModel
	{
	public:
		/**
		 * Class constructor.
		 */
		FaceDatasetModel(FaceDataset *pFaceDataset, QObject *pParent = NULL);

		/**
		 * Virtual class destructor.
		 */
		virtual ~FaceDatasetModel();

		/**
		 * Returns the number of rows under the given parent. When the parent is valid
		 * it means that rowCount is returning the number of children of parent.
		 * @param oParent A QModelIndex with the parent to be used in the query.
		 * @return Integer with the number of rows under the given parent.
		 */
		int rowCount(const QModelIndex &oParent = QModelIndex()) const;

		/**
		 * Returns the number of columns for the children of the given parent.
		 * @param oParent A QModelIndex with the parent to be used in the query.
		 * @return Integer with the number of columns for the children of the given parent.
		 */
		int columnCount(const QModelIndex &) const;

		/**
		 * Returns the data stored under the given role for the item referred to by the index.
		 * @param oIndex A QModelIndex with the index of the data requested.
		 * @param iRole A DispleyRole identifying the role of the data requested (the default is DisplayRole).
		 * @return A QVariant with the data requested.
		 */
		QVariant data(const QModelIndex &oIndex, int iRole = Qt::DisplayRole) const;

		QVariant headerData(int iSection, Qt::Orientation eOrientation, int iRole = Qt::DisplayRole) const;

		/**
		 * Mark that a change in the a reset in the model so the views using it will be updated.
		 */
		void beginUpdate();

		void endUpdate();

	private:
		/** Instance of the face annotation dataset for data access. */
		FaceDataset *m_pFaceDataset;
	};

}

#endif //FACEDATASETMODEL_H