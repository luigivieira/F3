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

#ifndef DATAMODEL_H
#define DATAMODEL_H

#include "logcontrol.h"

#include <QAbstractListModel>
#include <QMap>

namespace f3
{
	/**
	 * Model class that encapsulates the access to the Log Control data for the view
	 * classes in Qt (such as QListView).
	 */
	class DataModel : public QAbstractListModel
	{
	public:
		/**
		 * Class constructor.
		 * @param pParent Instance of the parent object for the data model (default is 0).
		 */
		DataModel(QObject *pParent = 0);

		/**
		 * Virtual class destructor.
		 */
		virtual ~DataModel();

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
		int columnCount(const QModelIndex &oParent = QModelIndex()) const;

		/**
		 * Returns the data stored under the given role for the item referred by the index.
		 * @param oIndex A QModelIndex with the index of the data requested.
		 * @param iRole A DisplayRole identifying the role of the data requested.
		 * @return A QVariant with the data requested.
		 */
		QVariant data(const QModelIndex &oIndex, int iRole = Qt::DisplayRole) const;

		/**
		 * Updates the data stored under the given role for the item referred by the index.
		 * @param oIndex A QModelIndex with the index of the data to be updated.
		 * param oValue A QVariant with the new value for the data under the index.
		 * @param iRole A DisplayRole identifying the role of the data to be updated.
		 * @return A boolean indicating if the update was successful (true) or not (false).
		 */
		bool setData(const QModelIndex &oIndex, const QVariant &oValue, int iRole = Qt::EditRole);

		/**
		 * Returns the header data stored under the given role and orientation for the section referred.
		 * @param iSection Integer with the index of the section (column).
		 * @param eOrientation Orientation of the header data requested (not used).
		 * @param iRole A DisplayRole identifying the role of the header data requested.
		 */
		QVariant headerData(int iSection, Qt::Orientation eOrientation, int iRole = Qt::DisplayRole) const;

		/**
		 * Queries the display/edit flags for the given index.
		 * @param oIndex A QModelIndex with the index to be queried.
		 * @return A Qt::ItemFlags with the flags to be applied to the given index.
		 */
		Qt::ItemFlags flags(const QModelIndex &oIndex) const;

		/**
		 * Update the map used to provide the data from the Log Control.
		 * @return Boolean indicating if the update was successful (true) or not (false).
		 */
		bool updateData();

	private:

		/** Map with the data from the applications running. */
		QMap<QString, QtMsgType> m_mData;

		/** Instance of the face annotation dataset for data access. */
		LogControl m_oLogControl;
	};

}

#endif //DATAMODEL_H