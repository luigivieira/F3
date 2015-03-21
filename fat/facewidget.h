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

		/** Scale value for the zoom in step. */
		static const double ZOOM_IN_STEP;

		/** Scale value for the zoom out step. */
		static const double ZOOM_OUT_STEP;

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
		 * Updates the pixmap displayed at the central area.
		 * @param oPixmap Reference for a QPixmap with the new pixmap to display.
		 */
		void setPixmap(const QPixmap &oPixmap);

		/**
		 * Gets the currently applied scale factor on the image displayed.
		 * @return Double with the currently applied scale factor.
		 */
		double getScaleFactor() const;

		/**
		 * Sets the scale factor of the image displayed.
		 * @param dScaleFactor Double with the new scale factor to be applied. It must be 
		 * in the interval [0.10, 10.0].
		 * @param bEmitSignal Boolean indicating if the change signal should be emitted or not.
		 * The default is true. This parameter is intended to be used with false by widgets that change
		 * the scale factor and depend upon receiving signal updates from the mouse wheel interaction.
		 */
		void setScaleFactor(const double dScaleFactor, const bool bEmitSignal = true);

		/**
		 * Performs one step of zoom in.
		 */
	    void zoomIn();

		/**
		 * Performs one step of zoom out.
		 */
		void zoomOut();

	signals:

		/**
		 * Signal to indicate that the scale of the display image has changed.
		 * @param dScaleFactor Double with the new scale factor.
		 */
		void onScaleFactorChanged(const double dScaleFactor);

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
		 * Scales the widget view to emulate a zoom behaviour by the given factor.
		 * @param dFactorBy Double with the scale to be added/removed.
		 */
		void scaleViewBy(double dFactorBy);

	private:
		/** The scene used to render the widget contents. */
		QGraphicsScene *m_pScene;

		/** Pixmap item used to display the face image. */
		QGraphicsPixmapItem *m_pPixmapItem;

		/** The current applied scale factor. */
		double m_dScaleFactor;
	};
};


#endif //FACEWIDGET_H
