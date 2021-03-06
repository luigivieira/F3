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

#ifndef EMOTIONDELEGATE_H
#define EMOTIONDELEGATE_H

#include "logcontrol.h"

#include <QItemDelegate>
#include <QComboBox>

namespace f3
{
	/**
	 * Item delegate used to allow editing the emotion label directly from ModelViews.
	 */
	class EmotionDelegate : public QItemDelegate
	{
		Q_OBJECT
	public:
		/**
		 * Class constructor.
		 * @param pParent Instance of the parent object for the item delegate (default is 0).
		 */
		EmotionDelegate(QObject *pParent = 0);

		/**
		 * Virtual class destructor.
		 */
		virtual ~EmotionDelegate();

		/**
		 * Creates the editor used for editing the emotion label.
		 * @param pParent Instance of the widget to be used as parent of the editor.
		 * @param oOption Reference of the QStyleOptionViewItem with the style options to be used in the editor.
		 * @param oIndex Reference of the QModelIndex with the data to be edited.
		 * @return Pointer to a QWidget with the created editor widget.
		 */
		QWidget *createEditor(QWidget *pParent, const QStyleOptionViewItem &oOption, const QModelIndex &oIndex) const;

		/**
		 * Updates the current data for the editor of the emotion label.
		 * @param pEditor Instance of the editor widget.
		 * @param oIndex Reference of the QModelIndex with the data to be edited.
		 */
		void setEditorData(QWidget *pEditor, const QModelIndex &oIndex) const;

		/**
		 * Updates the model after the user finished editing the emotion label.
		 * @param pEditor Instance of the editor widget.
		 * @param pModel Instance of the model for data updating.
		 * @param oIndex Reference of the QModelIndex with the new data edited.
		 */
		void setModelData(QWidget *pEditor, QAbstractItemModel *pModel, const QModelIndex &oIndex) const;

	protected slots:

		/**
		 * Slot for the item selection on the combobox (emotion label editor).
		 * @param iIndex Integer with the index of the item selected in the combobox.
		 */
		void setData(int iIndex);
	};

}

#endif //EMOTIONDELEGATE_H