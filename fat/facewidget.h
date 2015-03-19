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

#ifndef FACEWIDGET_H
#define FACEWIDGET_H

#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QMouseEvent>

namespace Ui {
    class MainWindow;
}

namespace f3
{
    /**
     * Implements the widget used to display face images and to edit facial features by dragging its points.
     */
    class FaceWidget : public QGraphicsView
    {
        Q_OBJECT
	public:
		/**
		 * Class constructor.
		 * @param pParent Instance of the parent widget.
		 */
		FaceWidget(QWidget *pParent = 0);

		/**
		 * Class destructor.
		 */
		virtual ~FaceWidget();

		/**
		 * Scales the widget view to emulate a zoom behaviour.
		 * @param fScaleFactor Float with the scale to be applied.
		 */
		void scaleView(float fScaleFactor);

		/**
		 * Updates the pixmap displayed at the central area.
		 * @param oPixmap Reference for a QPixmap with the new pixmap to display.
		 */
		void setPixmap(const QPixmap &oPixmap);

	protected:

	#ifndef QT_NO_WHEELEVENT
		/**
		 * Captures and handles the mouse wheel events to automate user interactions.
		 * @param pEvent Pointer to a QWheelEvent with the mouse event.
		 */
		void wheelEvent(QWheelEvent *pEvent) Q_DECL_OVERRIDE;
	#endif

		/**
		 * Captures and handles the keyboard events to automate user interactions.
		 */
		void keyPressEvent(QKeyEvent *pEvent) Q_DECL_OVERRIDE;

		/**
		 * Performs one step of zoom in.
		 */
	    void zoomIn();

		/**
		 * Performs one step of zoom out.
		 */
		void zoomOut();

	private:
		/** The scene used to render the widget contents. */
		QGraphicsScene *m_pScene;

		/** Pixmap item used to display the face image. */
		QGraphicsPixmapItem *m_pPixmapItem;
	};
};


#endif //FACEWIDGET_H
