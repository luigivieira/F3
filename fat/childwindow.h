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

#include <QtGui>
#include <QWidget>
#include <QMdiSubWindow>
#include <QSize>
#include <QScrollArea>

namespace Ui
{
    class ChildWindow;
}

namespace f3
{
	class ChildWindow : public QScrollArea
	{
		Q_OBJECT

	public:
		/**
		 * Class constructor.
		 * @param pParent Instance of the widget that will be the parent of this window.
		 */
		explicit ChildWindow(QWidget* pParent = 0);

	private:
		/** Instance of the ui for GUI element access. */
		Ui::ChildWindow ui;
	};
}

#endif // CHILDWINDOW_H
